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
	bckgrnd_id = Boot:AddImage("img/backdrops/boot/background.png", 1024, 768);
	logo_id = Boot:AddImage("img/logos/valyria_logo_black.png", 630, 318);
	cloud_field_id = Boot:AddImage("img/backdrops/boot/cloudfield.png", 248, 120);
	mist_id = Boot:AddImage("img/backdrops/boot/cloudy_mist.png", 1024, 768);
	fog_id = Boot:AddImage("img/backdrops/boot/fog.png", 1024, 768);
	crystal_id = Boot:AddImage("img/backdrops/boot/crystal.png", 140, 220);
	crystal_shadow_id = Boot:AddImage("img/backdrops/boot/crystal_shadow.png", 192, 168);
	satellite_id = Boot:AddImage("img/backdrops/boot/satellite.png", 34, 34);
	satellite_shadow_id = Boot:AddImage("img/backdrops/boot/satellite_shadow.png", 48, 32);
	flare_id = Boot:AddImage("img/backdrops/boot/flare.png", 256, 256);

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

-- cloud field members
local x_positions1 = { -110.0, 0.0, 110.0, 220.0 , 330.0, 440.0, 550.0, 660.0, 770.0, 880.0, 990.0};
local y_position1 = 400.0;
local x_positions2 = { -110.0, 0.0, 110.0, 220.0 , 330.0, 440.0, 550.0, 660.0, 770.0, 880.0, 990.0};
local y_position2 = 330.0;
local x_positions3 = { -110.0, 0.0, 110.0, 220.0 , 330.0, 440.0, 550.0, 660.0, 770.0, 880.0, 990.0};
local y_position3 = 260.0;
local x_positions4 = { -110.0, 0.0, 110.0, 220.0 , 330.0, 440.0, 550.0, 660.0, 770.0, 880.0, 990.0};
local y_position4 = 190.0;
local x_positions5 = { -110.0, 0.0, 110.0, 220.0 , 330.0, 440.0, 550.0, 660.0, 770.0, 880.0, 990.0};
local y_position5 = 120.0;
local x_positions6 = { -110.0, 0.0, 110.0, 220.0 , 330.0, 440.0, 550.0, 660.0, 770.0, 880.0, 990.0};
local y_position6 = 50.0;

-- crystal members
local crystal_decay = 0.0;
local crystal_time = 0;

-- satellite members
local sat1_decay = 0.0;
local sat1_x_position = -15.0;
local sat1_time = 0;
local sat1_behind = false;

local sat2_decay = 20.0;
local sat2_x_position = 80.0;
local sat2_time = 0;
local sat2_behind = false;

local sat3_decay = 10.0;
local sat3_x_position = 40.0;
local sat3_time = 0;
local sat3_behind = true;

function UpdateIntroFade()
    -- After one second of black, start fade in the logo
    if (animation_timer:GetTimeExpired() > 1000
            and animation_timer:GetTimeExpired() <= 4000) then

        logo_alpha = (animation_timer:GetTimeExpired() - 1000) / (4000 - 1000);

	elseif (animation_timer:GetTimeExpired() > 4000
            and animation_timer:GetTimeExpired() <= 7000) then

		bckgrnd_alpha = (animation_timer:GetTimeExpired() - 4000) / (7000 - 4000);
	end
end

-- Put the x coord on screen
function fix_pos(position)
	if (position <= -248.0) then
		return position + 1224.0;
	else
		return position;
	end
end

function UpdateBackgroundAnimation()
	local time_expired = SystemManager:GetUpdateTime();

	-- deal with all the clouds
	for i=1, #x_positions1 do
		x_positions1[i] = fix_pos(x_positions1[i]) - 0.1 * time_expired;
	end

	for i=1, #x_positions2 do
		x_positions2[i] = fix_pos(x_positions2[i]) - 0.2 * time_expired;
	end

	for i=1, #x_positions3 do
		x_positions3[i] = fix_pos(x_positions3[i]) - 0.3 * time_expired;
	end

	for i=1, #x_positions4 do
		x_positions4[i] = fix_pos(x_positions4[i]) - 0.4 * time_expired;
	end

	for i=1, #x_positions5 do
		x_positions5[i] = fix_pos(x_positions5[i]) - 0.5 * time_expired;
	end

	for i=1, #x_positions6 do
		x_positions6[i] = fix_pos(x_positions6[i]) - 0.6 * time_expired;
	end

	-- Compute the crystal and shadow movement
	crystal_time = crystal_time + time_expired
	if (crystal_time >= 31400) then
		crystal_time = crystal_time - 31400;
	end
	crystal_decay = 10 + math.sin(0.002 * crystal_time) * 10;

	-- compute the satellites movement
	sat1_time = sat1_time + time_expired
	if (sat1_time >= 31400) then
		sat1_time = sat1_time - 31400;
	end

	sat1_decay = -5 + math.sin(0.003 * sat1_time) * 10;
	sat1_x_position = 50 + (math.sin(0.0008 * sat1_time - 0.785) * 75);

	if (sat1_behind) then
		if (sat1_x_position < -24.0) then
			sat1_behind = false;
		end
	else
		if (sat1_x_position > 124.0) then
			sat1_behind = true;
		end
	end

	sat2_time = sat2_time + time_expired
	if (sat2_time >= 31400) then
		sat2_time = sat2_time - 31400;
	end

	sat2_decay = -5 + math.sin(0.003 * sat2_time + 1.57) * 10;
	sat2_x_position = 50 + (math.sin(0.0008 * sat2_time + 3.14) * 75);

	if (sat2_behind) then
		if (sat2_x_position < -24.0) then
			sat2_behind = false;
		end
	else
		if (sat2_x_position > 124.0) then
			sat2_behind = true;
		end
	end

	sat3_time = sat3_time + time_expired
	if (sat3_time >= 31400) then
		sat3_time = sat3_time - 31400;
	end

	sat3_decay = -5 + math.sin(0.003 * sat3_time + 0.785) * 10;
	sat3_x_position = 50 + (math.sin(0.0008 * sat3_time + 0.785) * 75);

	if (sat3_behind) then
		if (sat3_x_position < -24.0) then
			sat3_behind = false;
		end
	else
		if (sat3_x_position > 124.0) then
			sat3_behind = true;
		end
	end
end


-- Update the animation
function Update()

	animation_timer:Update();

	UpdateBackgroundAnimation();

    if (Boot:GetState() == hoa_boot.BootMode.BOOT_STATE_INTRO) then
        -- Start the timer
        if (animation_timer:IsInitial() == true and animation_timer:IsRunning() ~= true) then
            animation_timer:Run();
        elseif (animation_timer:IsFinished() == true) then
            -- Show the menu once the presentation is done
            Boot:ChangeState(hoa_boot.BootMode.BOOT_STATE_MENU);
        end

        -- Update the starting animation
        UpdateIntroFade();
    else
		logo_alpha = 1.0;
		bckgrnd_alpha = 1.0;
        animation_timer:Finish();
    end

    -- Nothing to do in menu mode.

end


function DrawCloudFieldLine(x_positions, y_position)

	for _,v in pairs(x_positions) do
		VideoManager:Move(v, y_position);
		Boot:DrawImage(cloud_field_id, hoa_video.Color(1.0, 1.0, 1.0, 0.6 * bckgrnd_alpha));
	end
end


function DrawMenuBackground()
	-- The background image
    VideoManager:Move(0.0, 769.0);
    Boot:DrawImage(bckgrnd_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha));

	-- The passing clouds
	DrawCloudFieldLine(x_positions1, y_position1);
	DrawCloudFieldLine(x_positions2, y_position2);
	DrawCloudFieldLine(x_positions3, y_position3);
	DrawCloudFieldLine(x_positions4, y_position4);
	DrawCloudFieldLine(x_positions5, y_position5);
	DrawCloudFieldLine(x_positions6, y_position6);

	-- front mist + fog
    VideoManager:Move(0.0, 769.0);
    Boot:DrawImage(mist_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.6));
    Boot:DrawImage(fog_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.8));

	-- satellite behind
	if (sat1_behind) then
		VideoManager:Move(640.0 + sat1_decay + (sat1_x_position / 2.0), 330.0 - (sat1_x_position / 3.0) );
		Boot:DrawImage(satellite_shadow_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.3));

		VideoManager:Move(448.0 + sat1_x_position, 400.0 + sat1_decay);
		Boot:DrawImage(satellite_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.7));
	end
	if (sat2_behind) then
		VideoManager:Move(640.0 + sat2_decay + (sat2_x_position / 2.0), 330.0 - (sat2_x_position / 3.0) );
		Boot:DrawImage(satellite_shadow_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.3));

		VideoManager:Move(448.0 + sat2_x_position, 400.0 + sat2_decay);
		Boot:DrawImage(satellite_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.7));
	end
	if (sat3_behind) then
		VideoManager:Move(640.0 + sat3_decay + (sat3_x_position / 2.0), 330.0 - (sat3_x_position / 3.0) );
		Boot:DrawImage(satellite_shadow_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.3));

		VideoManager:Move(448.0 + sat3_x_position, 400.0 + sat3_decay);
		Boot:DrawImage(satellite_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.7));
	end

	-- Crystal
	VideoManager:Move(498.0 + crystal_decay, 330.0);
	Boot:DrawImage(crystal_shadow_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.3));
	VideoManager:Move(448.0, 400.0 + crystal_decay);
	Boot:DrawImage(crystal_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.7));
	VideoManager:Move(384.0, 440.0 + crystal_decay);
	Boot:DrawImage(flare_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.6));

	-- satellite in front
	if (sat1_behind == false) then
		VideoManager:Move(640.0 + sat1_decay + (sat1_x_position / 2.0), 330.0 - (sat1_x_position / 3.0) );
		Boot:DrawImage(satellite_shadow_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.3));

		VideoManager:Move(448.0 + sat1_x_position, 400.0 + sat1_decay);
		Boot:DrawImage(satellite_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.7));
	end
	if (sat2_behind == false) then
		VideoManager:Move(640.0 + sat2_decay + (sat2_x_position / 2.0), 330.0 - (sat2_x_position / 3.0) );
		Boot:DrawImage(satellite_shadow_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.3));

		VideoManager:Move(448.0 + sat2_x_position, 400.0 + sat2_decay);
		Boot:DrawImage(satellite_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.7));
	end
	if (sat3_behind == false) then
		VideoManager:Move(640.0 + sat3_decay + (sat3_x_position / 2.0), 330.0 - (sat3_x_position / 3.0) );
		Boot:DrawImage(satellite_shadow_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.3));

		VideoManager:Move(448.0 + sat3_x_position, 400.0 + sat3_decay);
		Boot:DrawImage(satellite_id, hoa_video.Color(1.0, 1.0, 1.0, bckgrnd_alpha * 0.7));
	end

	-- Logo
    VideoManager:Move(198.0, 750.0);
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
