local ns = {}
setmetatable(ns, {__index = _G})
mt_elbrus_landscape_anim = ns;
setfenv(1, ns);

-- Animated image members
local background = {};

-- Scrolling offsets
local offset_x = 0;
-- Must be > 1024.0
local image_width = 1411;
local first_update_done = false;

-- c++ objects instances
local Map = nil
local Script = nil
local Effects = nil

function Initialize(map_instance)
    Map = map_instance;
    Script = Map:GetScriptSupervisor();
    Effects = Map:GetEffectSupervisor();
    -- Load the creatures animated background
    background = Script:CreateImage("data/story/ep1/mt_elbrus/elbrus_landscape.png");
    background:SetDimensions(image_width, 768);

    first_update_done = false;
end


function Update()

    if (first_update_done == false) then
        first_update_done = true;
        -- 32.0 is the number of tiles on the x axis in the map mode.
        -- 1024 is the area that is always shown on screen.
        if (Map:GetMapXOffset() >= ((Map:GetMapWidth() * 2.0) - 32.0)) then
            offset_x = 0;
        else
            offset_x = ((Map:GetMapWidth() * 2.0) - Map:GetMapXOffset() - 32.0) / (2.0 * Map:GetMapWidth() * (image_width - 1024));
        end
    end

    offset_x = offset_x + Effects:GetCameraXMovement() * Map:GetMapWidth() / image_width
    -- Trouble-Shooting: This prevents certain ugly edge-cases.
    if (offset_x < 0.0) then
        offset_x = 0.0;
    end

    --print(offset_x,  Map:GetMapXOffset(), Map:GetMapWidth())
end

local white_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawBackground()
    -- Draw background animation
    VideoManager:Move(512.0 + offset_x, 768.0);
    background:Draw(white_color);
end
