-- Boot animation script file
-- Set the boot tablespace name.
local ns = {}
setmetatable(ns, {__index = _G})
boot = ns;
setfenv(1, ns);


-- The Boot instance
local Boot = nil
local Script = nil

local animation_timer = nil
local boot_state = nil

local background_image = nil
local logo_image = nil
local cloud_image = nil
local mist_image = nil
local fog_image = nil
local crystal_image = nil
local crystal_shadow_image = nil
local satellite_image = nil
local satellite_shadow_image = nil
local flare_image = nil

-- Init all the needed variables
function Initialize(boot_instance)
    Boot = boot_instance;
    Script = Boot:GetScriptSupervisor();

    boot_state = Boot:GetState();

    -- Load the necessary files
    background_image = Script:CreateImage("img/backdrops/boot/background.png");
    background_image:SetDimensions(1024.0, 768.0);
    logo_image = Script:CreateImage("img/logos/valyria_logo_black.png");
    cloud_image = Script:CreateImage("img/backdrops/boot/cloudfield.png");
    mist_image = Script:CreateImage("img/backdrops/boot/cloudy_mist.png");
    mist_image:SetDimensions(1024.0, 768.0);
    fog_image = Script:CreateImage("img/backdrops/boot/fog.png");
    fog_image:SetDimensions(1024.0, 768.0);
    crystal_image = Script:CreateImage("img/backdrops/boot/crystal.png");
    crystal_shadow_image = Script:CreateImage("img/backdrops/boot/crystal_shadow.png");
    crystal_shadow_image:SetDimensions(192.0, 168.0);
    satellite_image = Script:CreateImage("img/backdrops/boot/satellite.png");
    satellite_image:SetDimensions(34.0, 34.0);
    satellite_shadow_image = Script:CreateImage("img/backdrops/boot/satellite_shadow.png");
    satellite_shadow_image:SetDimensions(48.0, 32.0);
    flare_image = Script:CreateImage("img/backdrops/boot/flare.png");

    -- Init the timer
    animation_timer = vt_system.SystemTimer(7000, 0);
end

function Reset()
    if (Boot:GetState() == vt_boot.BootMode.BOOT_STATE_MENU) then
        AudioManager:PlayMusic("music/Soliloquy_1-OGA-mat-pablo.ogg");
    end
end

-- The image alpha channel values
local logo_alpha = 0.0;
local bckgrnd_alpha = 0.0;

-- cloud field members
local x_positions1 = { -110.0, 0.0, 110.0, 220.0 , 330.0, 440.0, 550.0, 660.0, 770.0, 880.0, 990.0};
local y_position1 = 368.0;
local x_positions2 = { -110.0, 0.0, 110.0, 220.0 , 330.0, 440.0, 550.0, 660.0, 770.0, 880.0, 990.0};
local y_position2 = 438.0;
local x_positions3 = { -110.0, 0.0, 110.0, 220.0 , 330.0, 440.0, 550.0, 660.0, 770.0, 880.0, 990.0};
local y_position3 = 508.0;
local x_positions4 = { -110.0, 0.0, 110.0, 220.0 , 330.0, 440.0, 550.0, 660.0, 770.0, 880.0, 990.0};
local y_position4 = 578.0;
local x_positions5 = { -110.0, 0.0, 110.0, 220.0 , 330.0, 440.0, 550.0, 660.0, 770.0, 880.0, 990.0};
local y_position5 = 648.0;
local x_positions6 = { -110.0, 0.0, 110.0, 220.0 , 330.0, 440.0, 550.0, 660.0, 770.0, 880.0, 990.0};
local y_position6 = 718.0;

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
        x_positions1[i] = fix_pos(x_positions1[i]) - 0.025 * time_expired;
    end

    for i=1, #x_positions2 do
        x_positions2[i] = fix_pos(x_positions2[i]) - 0.05 * time_expired;
    end

    for i=1, #x_positions3 do
        x_positions3[i] = fix_pos(x_positions3[i]) - 0.075 * time_expired;
    end

    for i=1, #x_positions4 do
        x_positions4[i] = fix_pos(x_positions4[i]) - 0.1 * time_expired;
    end

    for i=1, #x_positions5 do
        x_positions5[i] = fix_pos(x_positions5[i]) - 0.125 * time_expired;
    end

    for i=1, #x_positions6 do
        x_positions6[i] = fix_pos(x_positions6[i]) - 0.15 * time_expired;
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


local music_started = false;
local snow_started = false;

-- Update the animation
function Update()
    animation_timer:Update();

    UpdateBackgroundAnimation();

    if (Boot:GetState() == vt_boot.BootMode.BOOT_STATE_INTRO) then
        -- Start the timer
        if (animation_timer:IsInitial() == true and animation_timer:IsRunning() ~= true) then
            animation_timer:Run();
        elseif (animation_timer:IsFinished() == true) then
            -- Show the menu once the presentation is done
            Boot:ChangeState(vt_boot.BootMode.BOOT_STATE_MENU);
        end

        -- Update the starting animation
        UpdateIntroFade();
    else
        logo_alpha = 1.0;
        bckgrnd_alpha = 1.0;
        animation_timer:Finish();
    end

    if (music_started == false) then
        AudioManager:PlayMusic("music/Soliloquy_1-OGA-mat-pablo.ogg");
        music_started = true;
    end

    -- Update the menu bar alpha in menu mode.

    if (Boot:GetState() == vt_boot.BootMode.BOOT_STATE_MENU) then
        if (snow_started == false) then
            Boot:GetParticleManager():AddParticleEffect("dat/effects/particles/snow.lua", 512.0, 384.0);
            snow_started = true;
        end
    end
end

local cloud_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawCloudFieldLine(x_positions, y_position)

    for _,v in pairs(x_positions) do
        VideoManager:Move(v, y_position);
        cloud_image:Draw(cloud_color);
    end
end

local background_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawBackground()
    -- The background image
    background_color:SetAlpha(bckgrnd_alpha);
    VideoManager:Move(0.0, 0.0);
    background_image:Draw(background_color);

    -- The passing clouds
    cloud_color:SetAlpha(0.6 * bckgrnd_alpha);
    DrawCloudFieldLine(x_positions1, y_position1);
    DrawCloudFieldLine(x_positions2, y_position2);
    DrawCloudFieldLine(x_positions3, y_position3);
    DrawCloudFieldLine(x_positions4, y_position4);
    DrawCloudFieldLine(x_positions5, y_position5);
    DrawCloudFieldLine(x_positions6, y_position6);
end

local mist_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local fog_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local logo_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

local sat_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local sat_shadow_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawPostEffects()
    -- front mist + fog
    mist_color:SetAlpha(bckgrnd_alpha * 0.6);
    fog_color:SetAlpha(bckgrnd_alpha * 0.8);
    VideoManager:Move(0.0, 0.0);
    mist_image:Draw(mist_color);
    fog_image:Draw(fog_color);

    sat_shadow_color:SetAlpha(bckgrnd_alpha * 0.3);
    sat_color:SetAlpha(bckgrnd_alpha * 0.7);

    -- satellite behind
    if (sat1_behind) then
        VideoManager:Move(640.0 + sat1_decay + (sat1_x_position / 2.0), 438.0 + (sat1_x_position / 3.0));
        satellite_shadow_image:Draw(sat_shadow_color);

        VideoManager:Move(448.0 + sat1_x_position, 368.0 - sat1_decay);
        satellite_image:Draw(sat_color);
    end
    if (sat2_behind) then
        VideoManager:Move(640.0 + sat2_decay + (sat2_x_position / 2.0), 438.0 + (sat2_x_position / 3.0));
        satellite_shadow_image:Draw(sat_shadow_color);

        VideoManager:Move(448.0 + sat2_x_position, 368.0 - sat2_decay);
        satellite_image:Draw(sat_color);
    end
    if (sat3_behind) then
        VideoManager:Move(640.0 + sat3_decay + (sat3_x_position / 2.0), 438.0 + (sat3_x_position / 3.0));
        satellite_shadow_image:Draw(sat_shadow_color);

        VideoManager:Move(448.0 + sat3_x_position, 368.0 - sat3_decay);
        satellite_image:Draw(sat_color);
    end

    -- Crystal
    VideoManager:Move(498.0 + crystal_decay, 438.0);
    crystal_shadow_image:Draw(sat_shadow_color);

    VideoManager:Move(448.0, 368.0 - crystal_decay);
    crystal_image:Draw(sat_color);

    VideoManager:Move(384.0, 328.0 - crystal_decay);
    flare_image:Draw(mist_color);

    -- satellite in front
    if (sat1_behind == false) then
        VideoManager:Move(640.0 + sat1_decay + (sat1_x_position / 2.0), 438.0 + (sat1_x_position / 3.0));
        satellite_shadow_image:Draw(sat_shadow_color);

        VideoManager:Move(448.0 + sat1_x_position, 368.0 - sat1_decay);
        satellite_image:Draw(sat_color);
    end
    if (sat2_behind == false) then
        VideoManager:Move(640.0 + sat2_decay + (sat2_x_position / 2.0), 438.0 + (sat2_x_position / 3.0));
        satellite_shadow_image:Draw(sat_shadow_color);

        VideoManager:Move(448.0 + sat2_x_position, 368.0 - sat2_decay);
        satellite_image:Draw(sat_color);
    end
    if (sat3_behind == false) then
        VideoManager:Move(640.0 + sat3_decay + (sat3_x_position / 2.0), 438.0 + (sat3_x_position / 3.0));
        satellite_shadow_image:Draw(sat_shadow_color);

        VideoManager:Move(448.0 + sat3_x_position, 368.0 - sat3_decay);
        satellite_image:Draw(sat_color);
    end

    -- Logo
    logo_color:SetAlpha(logo_alpha);
    VideoManager:Move(198.0, 18.0);
    logo_image:Draw(logo_color);
end
