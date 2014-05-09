local ns = {}
setmetatable(ns, {__index = _G})
mt_elbrus_background_anim = ns;
setfenv(1, ns);

-- Animated image members
local background = {};

-- Scrolling offsets
local offset_y = 0;
-- Must be > 768.0
local image_height = 1024;
local first_update_done = false;

-- c++ objects instances
local Map = {};
local Script = {};
local Effects = {};

function Initialize(map_instance)
    Map = map_instance;
    Script = Map:GetScriptSupervisor();
    Effects = Map:GetEffectSupervisor();
    -- Load the creatures animated background
    background = Script:CreateImage("img/backdrops/cliff_background.png");
    background:SetDimensions(1024.0, image_height);

    first_update_done = false;
end


function Update()

    if (first_update_done == false) then
        first_update_done = true;
        -- 24.0 is the number of tiles on the y axis in the map mode.
        -- 768 is the area that is always shown on screen.
        if (Map:GetMapYOffset() >= ((Map:GetMapHeight() * 2.0) - 24.0)) then
            offset_y = 0;
        else
            offset_y = ((Map:GetMapHeight() * 2.0) - Map:GetMapYOffset() - 24.0) / (2.0 * Map:GetMapHeight() * (image_height - 768));
        end
    end

    offset_y = offset_y + Effects:GetCameraYMovement() * Map:GetMapHeight() / image_height
    -- Trouble-Shooting: This prevents certain ugly edge-cases.
    if (offset_y < 0.0) then
        offset_y = 0.0;
    end

    --print(offset_y,  Map:GetMapYOffset(), Map:GetMapHeight())

end

local white_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawBackground()
    -- Draw background animation
    VideoManager:Move(512.0, 768.0 + offset_y);
    background:Draw(white_color);
end
