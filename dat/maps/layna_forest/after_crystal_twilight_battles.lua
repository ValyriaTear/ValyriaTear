local ns = {}
setmetatable(ns, {__index = _G})
after_crystal_twilight_battles = ns;
setfenv(1, ns);

local Script = {};
local light_layer = {};

local twilight_red = 0.0;
local twilight_green = 0.0;
local twilight_blue = 0.0;
local twilight_alpha = 0.0;

function _SetTwilightValues()
    local twilight_step = GlobalManager:GetEventValue("story", "layna_forest_twilight_value");

    if (twilight_step == 0) then
        twilight_red = 0.0;
        twilight_green = 0.0;
        twilight_blue = 0.0;
        twilight_alpha = 0.0;
    elseif (twilight_step == 1) then
        twilight_red = 0.84;
        twilight_green = 0.93;
        twilight_blue = 0.49;
        twilight_alpha = 0.09;

    elseif (twilight_step == 2) then
        twilight_red = 0.71;
        twilight_green = 0.66;
        twilight_blue = 0.0;
        twilight_alpha = 0.09;

    elseif (twilight_step == 3) then
        twilight_red = 0.62;
        twilight_green = 0.35;
        twilight_blue = 0.0;
        twilight_alpha = 0.20;

    elseif (twilight_step == 4) then
        twilight_red = 0.44;
        twilight_green = 0.07;
        twilight_blue = 0.0;
        twilight_alpha = 0.29;

    elseif (twilight_step == 5) then
        twilight_red = 0.35;
        twilight_green = 0.0;
        twilight_blue = 0.20;
        twilight_alpha = 0.40;

    elseif (twilight_step == 6) then
        twilight_red = 0.24;
        twilight_green = 0.0;
        twilight_blue = 0.18;
        twilight_alpha = 0.49;

    elseif (twilight_step == 7) then
        twilight_red = 0.18;
        twilight_green = 0.0;
        twilight_blue = 0.30;
        twilight_alpha = 0.59;

    elseif (twilight_step >= 8) then
        twilight_red = 0.0;
        twilight_green = 0.0;
        twilight_blue = 0.24;
        twilight_alpha = 0.50;
    end
end

-- add a evening light layer
function Initialize(battle_instance)
    Script = battle_instance:GetScriptSupervisor();

    -- Load a white empty image
    light_layer = Script:CreateImage("");
    light_layer:SetDimensions(1024.0, 768.0);

    _SetTwilightValues();
end

local twilight_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawForeground()
    -- Useful for the map mode
    Script:SetDrawFlag(vt_video.GameVideo.VIDEO_X_LEFT);
    Script:SetDrawFlag(vt_video.GameVideo.VIDEO_Y_TOP);
    twilight_color:SetColor(twilight_red, twilight_green, twilight_blue, twilight_alpha);
    VideoManager:Move(0.0, 0.0);
    light_layer:Draw(twilight_color);
end
