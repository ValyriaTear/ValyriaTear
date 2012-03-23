-- Boot animation script file
-- Set the boot tablespace name.
local ns = {}
setmetatable(ns, {__index = _G})
boot = ns;
setfenv(1, ns);


-- The Boot instance
local Boot;

-- Sequence time steps
local SEQUENCE_ONE = 0.0;
local SEQUENCE_TWO = SEQUENCE_ONE + 1000.0; -- 1000
local SEQUENCE_THREE = SEQUENCE_TWO + 2000.0; -- 3000
local SEQUENCE_FOUR = SEQUENCE_THREE + 575.0; -- 3575
local SEQUENCE_FIVE = SEQUENCE_FOUR + 1900.0; -- 5475
local SEQUENCE_SIX = SEQUENCE_FIVE + 1400.0; -- 6875
local SEQUENCE_SEVEN = SEQUENCE_SIX + 3500.0; -- 10375

local animation_timer;

-- Init all the needed variables
function Initialize(boot_instance)
    Boot = boot_instance;

    boot_state = Boot:GetState();

    -- Load the necessary files
    bckgrnd_id = Boot:AddImage("img/backdrops/boot_screen00.jpg", 1024, 768);
    logo_bckgrnd_id = Boot:AddImage("img/logos/main_logo_background.png", 666, 239);
    logo_sword_id = Boot:AddImage("img/logos/main_logo_sword.png", 130, 282);
    logo_text_id = Boot:AddImage("img/logos/main_logo_text.png", 666, 239);

    -- Init the timer
    animation_timer = hoa_system.SystemTimer(SEQUENCE_SEVEN, 0);
end

function Reset()
	if (Boot:GetState() == hoa_boot.BootMode.BOOT_STATE_MENU) then
		AudioManager:PlayMusic("mus/Allacrost_Opening_Theme.ogg");
	end
end

-- The logo image alpha channel value
local logo_alpha = 0.0;
-- Images coordinates and angle
local sword_x = 670.0;
local sword_y = 360.0;
local rotation = -90.0;
local sword_fall_x = 0.0;
local sword_fall_y = 0.0;

-- Used for sword rotation computation
local ROTATIONS = 720.0 + 90.0;
local SPEED_LEFT = 35.0;
local SPEED_UP = 750.0;
local GRAVITY = 120.0;

-- Sounds to play
local sword_unsheathe_played = false;
local sword_swipe1_played = false;
local sword_swipe2_played = false;
local sword_fall_played = false;

function UpdateAnimation()
    -- After one second of black, start fade in the logo
    if (animation_timer:GetTimeExpired() > SEQUENCE_TWO
            and animation_timer:GetTimeExpired() <= SEQUENCE_THREE) then

        logo_alpha = (animation_timer:GetTimeExpired() - SEQUENCE_TWO) / (SEQUENCE_THREE - SEQUENCE_TWO);

    elseif (animation_timer:GetTimeExpired() > SEQUENCE_THREE
            and animation_timer:GetTimeExpired() <= SEQUENCE_FOUR) then

        local dt = (animation_timer:GetTimeExpired() - SEQUENCE_THREE) * 0.001;
		sword_x = 670.0 + (dt * dt) * 660.0; -- s = s0 + 0.5 * a * t^2

		-- Play the unsheathe sound once
		if (sword_unsheathe_played == false) then
			AudioManager:PlaySound("snd/opening_sword_unsheathe.wav");
			sword_unsheathe_played = true;
		end
    elseif (animation_timer:GetTimeExpired() > SEQUENCE_FOUR
            and animation_timer:GetTimeExpired() <= SEQUENCE_FIVE) then

        local delta = ((animation_timer:GetTimeExpired() - SEQUENCE_FOUR) / (SEQUENCE_FIVE - SEQUENCE_FOUR));
		local dt = (animation_timer:GetTimeExpired() - SEQUENCE_FOUR) * 0.001;
		sword_x = 885.941 - dt * dt * SPEED_LEFT; -- Small accelerated movement to left
		sword_y = 360.0 - dt * dt * GRAVITY + SPEED_UP * delta;
		rotation = -90.0 + delta * ROTATIONS;

		-- Play the swipe1 sound once at beginning
		if (sword_swipe1_played == false) then
			AudioManager:PlaySound("snd/opening_sword_swipe.wav");
			sword_swipe1_played = true;
		end
		-- Play the swipe2 sound once at bit later
		if (sword_swipe2_played == false
				and animation_timer:GetTimeExpired() - SEQUENCE_FOUR > 1000) then
			AudioManager:PlaySound("snd/opening_sword_swipe.wav");
			sword_swipe2_played = true;
		end

    elseif (animation_timer:GetTimeExpired() > SEQUENCE_FIVE
            and animation_timer:GetTimeExpired() <= SEQUENCE_SIX) then

        -- Keep track of the x, y fall origin coords
        if (sword_fall_x == 0.0) then
            sword_fall_x = sword_x;
            sword_fall_y = sword_y;
        end
        local delta_root = (animation_timer:GetTimeExpired() - SEQUENCE_FIVE) / (SEQUENCE_SIX - SEQUENCE_FIVE);
		local delta = delta_root * delta_root * delta_root * delta_root;
		sword_x = (1.0 - delta) * sword_fall_x + 762.0 * delta;
        sword_y = (1.0 - delta) * sword_fall_y + 310.0 * delta;
        rotation = 0.0;


		-- Play the fall sound once
		if (sword_fall_played == false
			and animation_timer:GetTimeExpired() - SEQUENCE_FIVE > 500) then
			AudioManager:PlaySound("snd/opening_sword_fall.wav");
			sword_fall_played = true;
		end
    elseif (animation_timer:GetTimeExpired() > SEQUENCE_SIX
            and animation_timer:GetTimeExpired() <= SEQUENCE_SEVEN) then

        local delta = (animation_timer:GetTimeExpired() - SEQUENCE_SIX) / (SEQUENCE_SEVEN - SEQUENCE_SIX);
		delta = 1.0 - delta * delta;
        VideoManager:EnableLightingOverlay(hoa_video.Color(1.0, 1.0, 1.0, delta));
    end

end


-- Update the animation
function Update()

    animation_timer:Update();

    if (Boot:GetState() == hoa_boot.BootMode.BOOT_STATE_INTRO) then
        -- Start the timer
        if (animation_timer:IsInitial() == true and animation_timer:IsRunning() ~= true) then
            animation_timer:Run();
        elseif (animation_timer:IsFinished() == true) then
            -- Show the menu once the presentation is done
            Boot:ChangeState(hoa_boot.BootMode.BOOT_STATE_MENU);
        end

        -- Update the starting animation
        UpdateAnimation();
    else
        animation_timer:Finish();
    end

    -- Nothing to do in menu mode.

end


function DrawMenuBackground()
    Boot:SetDrawFlag(hoa_video.GameVideo.VIDEO_X_LEFT);
    Boot:SetDrawFlag(hoa_video.GameVideo.VIDEO_Y_TOP);
    VideoManager:Move(0.0, 769.0);
    Boot:DrawImage(bckgrnd_id, hoa_video.Color(1.0, 1.0, 1.0, 1.0));
    VideoManager:Move(179.0, 767.0);
    Boot:DrawImage(logo_bckgrnd_id, hoa_video.Color(1.0, 1.0, 1.0, 1.0));
    VideoManager:Move(697.0, 719.0);
    Boot:DrawImage(logo_sword_id, hoa_video.Color(1.0, 1.0, 1.0, 1.0));
    VideoManager:Move(179.0, 767.0);
    Boot:DrawImage(logo_text_id, hoa_video.Color(1.0, 1.0, 1.0, 1.0));
end


function DrawAnimation()
    Boot:SetDrawFlag(hoa_video.GameVideo.VIDEO_X_CENTER);
    Boot:SetDrawFlag(hoa_video.GameVideo.VIDEO_Y_CENTER);
    -- After one second of black, start fade in the logo
    if (animation_timer:GetTimeExpired() > SEQUENCE_TWO
            and animation_timer:GetTimeExpired() <= SEQUENCE_SIX) then

        -- Draw the logo
        VideoManager:Move(512.0, 385.0);
        Boot:DrawImage(logo_bckgrnd_id, hoa_video.Color(logo_alpha, logo_alpha, logo_alpha, 1.0));
        -- The sword
        VideoManager:Move(sword_x, sword_y);
        VideoManager:Rotate(rotation);
        Boot:DrawImage(logo_sword_id, hoa_video.Color(logo_alpha, logo_alpha, logo_alpha, 1.0));
        -- Text after the sword
        VideoManager:Move(512, 385.0);
        Boot:DrawImage(logo_text_id, hoa_video.Color(logo_alpha, logo_alpha, logo_alpha, 1.0));
    elseif (animation_timer:GetTimeExpired() > SEQUENCE_SIX) then
        DrawMenuBackground()
    end

end

local menu_started = false;
-- Draw the animation in progress
function Draw()
    if (Boot:GetState() == hoa_boot.BootMode.BOOT_STATE_INTRO) then
        -- Draw the starting animation
        DrawAnimation();
    elseif (Boot:GetState() == hoa_boot.BootMode.BOOT_STATE_MENU) then
        if (menu_started == false) then
            VideoManager:DisableLightingOverlay();
            AudioManager:PlayMusic("mus/Allacrost_Opening_Theme.ogg");

            menu_started = true;
        end
        DrawMenuBackground();
    end
end
