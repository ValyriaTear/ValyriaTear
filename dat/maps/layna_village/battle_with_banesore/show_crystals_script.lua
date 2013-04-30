local ns = {}
setmetatable(ns, {__index = _G})
show_crystals_script = ns;
setfenv(1, ns);

local crystal_id = 0;
local display_time = 0;

local camera_x_position = 0.0;
local camera_y_position = 0.0;

local lord_x_position = 0.0;
local lord_y_position = 0.0;

local script_triggered = false;

-- c++ objects instances
local Map = {};
local Script = {};

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();

    white_crystal_id = Script:AddImage("img/sprites/map/npcs/crystal_spritesheet.png", 19.0, 37.0);

    camera_x_position = 512.0;
    camera_y_position = 384.0 - 40.0;

    -- We don't compute the lord screen coordinates now as they will have changed when the script will actually start.

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
        lord_x_position = Map:GetScreenXCoordinate(97.8);
        lord_y_position = Map:GetScreenYCoordinate(54.0) - 50.0;

        -- TODO: Play crystal sound

        script_triggered = true;
    end

    local time_expired = SystemManager:GetUpdateTime();

    -- Handle the timer
    display_time = display_time + time_expired;

    -- Start the timer
    if (display_time > 8000) then
        display_time = 0
        -- Disable the event at the end of it
        GlobalManager:SetEventValue("scripts_events", "layna_village_riverbank_show_crystals", 0);
    end
end

function DrawPostEffects()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("scripts_events", "layna_village_riverbank_show_crystals") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("scripts_events", "layna_village_riverbank_show_crystals") == 0) then
        return;
    end

    local flash_alpha = 0.0;
    if (display_time >= 0 and display_time <= 2500) then
		flash_alpha = display_time / 2500;
    elseif (display_time > 2500 and display_time <= 4500) then
        flash_alpha = 1.0;
    elseif (display_time > 4500 and display_time <= 6000) then
        flash_alpha = 1.0 - (display_time - 4500) / (6000 - 4500);
    elseif (display_time > 6000) then
        flash_alpha = 0.0;
        return;
    end

    -- White
    Script:DrawImage(crystal_id, camera_x_position, camera_y_position, vt_video.Color(1.0, 1.0, 1.0, 0.7 * flash_alpha));
    -- red one
    Script:DrawImage(crystal_id, lord_x_position, lord_y_position, vt_video.Color(1.0, 0.0, 0.0, 0.7 * flash_alpha));
end
