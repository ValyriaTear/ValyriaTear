local ns = {}
setmetatable(ns, {__index = _G})
shrine_entrance_show_crystal_script = ns;
setfenv(1, ns);

local crystal = nil
local black_layer = nil
local ancient_sign = nil

local display_time = 0;

local crystal_x_position = 0.0;
local crystal_y_position = 0.0;
local ancient_sign_x_position = 0.0;
local ancient_sign_y_position = 0.0;

local crystal_alpha = 0.0;
local black_layer_alpha = 0.0;
local ancient_sign_alpha = 0.0;

local script_triggered = false;

-- c++ objects instances
local Map = nil
local Script = nil

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();

    crystal = Script:CreateImage("data/entities/map/npcs/crystal_spritesheet.png");
    crystal:SetDimensions(19.0, 37.0);
    black_layer = Script:CreateImage("");
    black_layer:SetDimensions(1024.0, 768.0);

    ancient_sign = Script:CreateImage("dat/maps/mt_elbrus/ancient_sign_big_bright.png");

    crystal_alpha = 0.0;
    black_layer_alpha = 0.0;
    ancient_sign_alpha = 0.0;

    script_triggered = false;
end

function Update()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("scripts_events", "shrine_entrance_show_crystal") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("scripts_events", "shrine_entrance_show_crystal") == 0) then
        return;
    end

    if (script_triggered == false) then
        -- Compute the positions once started
        crystal_x_position = Map:GetScreenXCoordinate(42.0);
        crystal_y_position = Map:GetScreenYCoordinate(8.0) - 40.0;

        ancient_sign_x_position = Map:GetScreenXCoordinate(42.0);
        ancient_sign_y_position = Map:GetScreenYCoordinate(10.0);

        -- Play crystal sound
        AudioManager:PlaySound("sounds/crystal_appearance.ogg");

        script_triggered = true;
    end

    local time_expired = SystemManager:GetUpdateTime();

    -- Handle the timer
    display_time = display_time + time_expired;

    if (display_time >= 0 and display_time <= 2500) then
        crystal_alpha = display_time / 2500;
        black_layer_alpha = (display_time / 2500) * 0.6;
    elseif (display_time > 2500 and display_time <= 4500) then
        crystal_alpha = 1.0;
        black_layer_alpha = 0.6;
    elseif (display_time > 4500 and display_time <= 6000) then
        crystal_alpha = 1.0 - (display_time - 4500) / (6000 - 4500);
    elseif (display_time > 6000) then
        crystal_alpha = 0.0;
    end

    if (display_time > 4500 and display_time <= 6000) then
        ancient_sign_alpha = 0.8 * (display_time - 4500) / (6000 - 4500);
    elseif (display_time > 6000 and display_time <= 12000) then
        ancient_sign_alpha = 0.8;
    elseif (display_time > 12000 and display_time <= 14500) then
        ancient_sign_alpha = 0.8 - (0.8 * (display_time - 12000) / (14500 - 12000));
    end

    -- Stop the event
    if (display_time > 15000) then
        display_time = 0
        -- Disable the event at the end of it
        GlobalManager:SetEventValue("scripts_events", "shrine_entrance_show_crystal", 0);
    end
end

local black_layer_color = vt_video.Color(0.0, 0.0, 0.0, 1.0);
local white_crystal_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local ancient_sign_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawPostEffects()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("scripts_events", "shrine_entrance_show_crystal") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("scripts_events", "shrine_entrance_show_crystal") == 0) then
        return;
    end

    -- black layer
    -- Current alignment: X_CENTER, Y_BOTTOM
    black_layer_color:SetAlpha(black_layer_alpha);
    VideoManager:Move(512.0, 768.0);
    black_layer:Draw(black_layer_color);

    VideoManager:Move(ancient_sign_x_position, ancient_sign_y_position);
    ancient_sign_color:SetAlpha(ancient_sign_alpha);
    ancient_sign:Draw(ancient_sign_color);

    white_crystal_color:SetAlpha(0.7 * crystal_alpha);
    VideoManager:Move(crystal_x_position, crystal_y_position + math.sin(0.003 * display_time + 0.785) * 3);
    crystal:Draw(white_crystal_color);
end
