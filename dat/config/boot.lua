-- Boot animation script file
-- Set the boot tablespace name.
local ns = {}
setmetatable(ns, {__index = _G})
boot = ns;
setfenv(1, ns);


-- The Boot instance
local Boot;

local animation_timer;

-- Init all the needed variables
function Initialize(boot_instance)
    Boot = boot_instance;

    boot_state = Boot:GetState();

    -- Load the necessary files
    bckgrnd_id = Boot:AddImage("img/backdrops/boot_background.png", 1024, 769);
    logo_id = Boot:AddImage("img/logos/valyria_logo_black.png", 630, 318);

    -- Init the timer
    animation_timer = hoa_system.SystemTimer(7000, 0);
end

function Reset()
	if (Boot:GetState() == hoa_boot.BootMode.BOOT_STATE_MENU) then
		AudioManager:PlayMusic("mus/Soliloquy_1-OGA-mat-pablo.ogg");
	end
end

-- The image alpha channel value
local logo_alpha = 0.0;
local bckgrnd_alpha = 0.0;


function UpdateAnimation()
    -- After one second of black, start fade in the logo
    if (animation_timer:GetTimeExpired() > 1000
            and animation_timer:GetTimeExpired() <= 4000) then

        logo_alpha = (animation_timer:GetTimeExpired() - 1000) / (4000 - 1000);

	elseif (animation_timer:GetTimeExpired() > 4000
            and animation_timer:GetTimeExpired() <= 7000) then

		bckgrnd_alpha = (animation_timer:GetTimeExpired() - 4000) / (7000 - 4000);
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
		logo_alpha = 1.0;
		bckgrnd_alpha = 1.0;
        animation_timer:Finish();
    end

    -- Nothing to do in menu mode.

end


function DrawMenuBackground()
    VideoManager:Move(0.0, 769.0);
    Boot:DrawImage(bckgrnd_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha));
    VideoManager:Move(198.0, 667.0);
    Boot:DrawImage(logo_id, hoa_video.Color(1.0, 1.0, 1.0, logo_alpha));
end



local menu_started = false;
-- Draw the animation in progress
function Draw()
	DrawMenuBackground();
	if (menu_started == false) then
		AudioManager:PlayMusic("mus/Soliloquy_1-OGA-mat-pablo.ogg");

		menu_started = true;
	end
end
