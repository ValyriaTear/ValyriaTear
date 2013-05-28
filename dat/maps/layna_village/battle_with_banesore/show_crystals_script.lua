local ns = {}
setmetatable(ns, {__index = _G})
show_crystals_script = ns;
setfenv(1, ns);

local crystal_id = -1;
local black_layer_id = -1;
local display_time = 0;

local camera_x_position = 0.0;
local camera_y_position = 0.0;

local lord_x_position = 0.0;
local lord_y_position = 0.0;

local crystal1_alpha = 0.0;
local crystal2_alpha = 0.0;

local script_triggered = false;

-- c++ objects instances
local Map = {};
local Script = {};

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();

    crystal_id = Script:AddImage("img/sprites/map/npcs/crystal_spritesheet.png", 19.0, 37.0);
    black_layer_id = Script:AddImage("", 1024.0, 768.0);

    camera_x_position = 512.0;
    camera_y_position = 384.0 - 40.0;

    -- We don't compute the lord screen coordinates now as they will have changed when the script will actually start.

    crystal1_alpha = 0.0;
    crystal2_alpha = 0.0;

    script_triggered = false;
end

function Update()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("scripts_events", "layna_village_riverbank_show_crystals") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("scripts_events", "layna_village_riverbank_show_crystals") == 0) then
        return;
    end

    if (script_triggered == false) then
        -- Compute the lord position once: 97.8, 54.0
        lord_x_position = Map:GetScreenXCoordinate(101.0);
        lord_y_position = Map:GetScreenYCoordinate(54.0) - 50.0;

        -- Play crystal sound
        AudioManager:PlaySound("snd/crystal_appearance.ogg");

        script_triggered = true;
    end

    local time_expired = SystemManager:GetUpdateTime();

    -- Handle the timer
    display_time = display_time + time_expired;

    if (display_time >= 0 and display_time <= 2500) then
        crystal1_alpha = display_time / 2500;
    elseif (display_time > 2500 and display_time <= 4500) then
        crystal1_alpha = 1.0;
    elseif (display_time > 4500 and display_time <= 6000) then
        crystal1_alpha = 1.0 - (display_time - 4500) / (6000 - 4500);
    elseif (display_time > 6000) then
        crystal1_alpha = 0.0;
    end

    if (display_time >= 0 and display_time <= 4500) then
        crystal2_alpha = display_time / 4500;
    elseif (display_time > 4500 and display_time <= 5500) then
        crystal2_alpha = 1.0;
    elseif (display_time > 5500 and display_time <= 7000) then
        crystal2_alpha = 1.0 - (display_time - 5500) / (7000 - 5500);
    elseif (display_time > 7000) then
        crystal2_alpha = 0.0;
    end

    -- Stop the event
    if (display_time > 8000) then
        display_time = 0
        -- Disable the event at the end of it
        GlobalManager:SetEventValue("scripts_events", "layna_village_riverbank_show_crystals", 0);
    end
end

local black_layer_color = vt_video.Color(0.0, 0.0, 0.0, 1.0);
local white_crystal_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local red_crystal_color = vt_video.Color(1.0, 0.0, 0.0, 1.0);

function DrawPostEffects()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("scripts_events", "layna_village_riverbank_show_crystals") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("scripts_events", "layna_village_riverbank_show_crystals") == 0) then
        return;
    end

    -- black layer
    -- Current alignment: X_CENTER, Y_BOTTOM
    black_layer_color:SetAlpha(0.9 * crystal1_alpha);
    Script:DrawImage(black_layer_id, 512.0, 768.0, black_layer_color);

    -- White
    white_crystal_color:SetAlpha(0.7 * crystal1_alpha);
    Script:DrawImage(crystal_id, camera_x_position, camera_y_position + math.sin(0.003 * display_time + 0.785) * 3, white_crystal_color);
    -- red one
    red_crystal_color:SetAlpha(0.7 * crystal2_alpha);
    Script:DrawImage(crystal_id, lord_x_position, lord_y_position + math.sin(0.003 * display_time + 3.14) * 3, red_crystal_color);
end
