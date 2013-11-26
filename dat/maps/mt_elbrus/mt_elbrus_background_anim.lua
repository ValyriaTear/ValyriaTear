local ns = {}
setmetatable(ns, {__index = _G})
mt_elbrus_background_anim = ns;
setfenv(1, ns);

-- Animated image members
local background = {};

-- Scrolling offsets
local offset_y = 0;

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
    background:SetDimensions(1024.0, 1024.0);

    if (Map.camera:GetYPosition() >= 70) then
    offset_y = 0;
    elseif (Map.camera:GetYPosition() <= 10) then
        offset_y = 1024 - 768;
    end
end


function Update()

    -- TODO: Get the map height from the map class
    offset_y = offset_y - Effects:GetCameraYMovement() * 80 / 700

end

local white_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawBackground()
    -- Draw background animation
    VideoManager:Move(512.0, 768.0 + offset_y);
    background:Draw(white_color);
end

