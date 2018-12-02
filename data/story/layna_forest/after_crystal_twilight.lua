local ns = {}
setmetatable(ns, {__index = _G})
after_crystal_twilight = ns;
setfenv(1, ns);

local Script = nil
local light_layer = nil

local twilight_step = 0;

local twilight_red = 0.0;
local twilight_green = 0.0;
local twilight_blue = 0.0;
local twilight_alpha = 0.0;

local next_twilight_red = 0.0;
local next_twilight_green = 0.0;
local next_twilight_blue = 0.0;
local next_twilight_alpha = 0.0;
local total_time = 0.0;

function _ResetTwilightValues()
    twilight_step = GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_twilight_value");

    if (twilight_step == 0) then
        twilight_red = 0.0;
        twilight_green = 0.0;
        twilight_blue = 0.0;
        twilight_alpha = 0.0;

        next_twilight_red = 0.84;
        next_twilight_green = 0.93;
        next_twilight_blue = 0.49;
        next_twilight_alpha = 0.09;
    elseif (twilight_step == 1) then
        twilight_red = 0.84;
        twilight_green = 0.93;
        twilight_blue = 0.49;
        twilight_alpha = 0.09;

        next_twilight_red = 0.71;
        next_twilight_green = 0.66;
        next_twilight_blue = 0.0;
        next_twilight_alpha = 0.09;
    elseif (twilight_step == 2) then
        twilight_red = 0.71;
        twilight_green = 0.66;
        twilight_blue = 0.0;
        twilight_alpha = 0.09;

        next_twilight_red = 0.62;
        next_twilight_green = 0.35;
        next_twilight_blue = 0.0;
        next_twilight_alpha = 0.20;
    elseif (twilight_step == 3) then
        twilight_red = 0.62;
        twilight_green = 0.35;
        twilight_blue = 0.0;
        twilight_alpha = 0.20;

        next_twilight_red = 0.44;
        next_twilight_green = 0.07;
        next_twilight_blue = 0.0;
        next_twilight_alpha = 0.29;
    elseif (twilight_step == 4) then
        twilight_red = 0.44;
        twilight_green = 0.07;
        twilight_blue = 0.0;
        twilight_alpha = 0.29;

        next_twilight_red = 0.35;
        next_twilight_green = 0.0;
        next_twilight_blue = 0.20;
        next_twilight_alpha = 0.40;
    elseif (twilight_step == 5) then
        twilight_red = 0.35;
        twilight_green = 0.0;
        twilight_blue = 0.20;
        twilight_alpha = 0.40;

        next_twilight_red = 0.24;
        next_twilight_green = 0.0;
        next_twilight_blue = 0.18;
        next_twilight_alpha = 0.49;
    elseif (twilight_step == 6) then
        twilight_red = 0.24;
        twilight_green = 0.0;
        twilight_blue = 0.18;
        twilight_alpha = 0.49;

        next_twilight_red = 0.18;
        next_twilight_green = 0.0;
        next_twilight_blue = 0.30;
        next_twilight_alpha = 0.59;
    elseif (twilight_step == 7) then
        twilight_red = 0.18;
        twilight_green = 0.0;
        twilight_blue = 0.30;
        twilight_alpha = 0.59;

        next_twilight_red = 0.0;
        next_twilight_green = 0.0;
        next_twilight_blue = 0.24;
        next_twilight_alpha = 0.60;
    elseif (twilight_step >= 8) then
        twilight_red = 0.0;
        twilight_green = 0.0;
        twilight_blue = 0.24;
        twilight_alpha = 0.60;

        next_twilight_red = 0.0;
        next_twilight_green = 0.0;
        next_twilight_blue = 0.24;
        next_twilight_alpha = 0.60;
    end
end

-- add a evening light layer
function Initialize(battle_instance)
    Script = battle_instance:GetScriptSupervisor();

    -- Load a white empty image
    light_layer = Script:CreateImage("");
    light_layer:SetDimensions(1024.0, 768.0);

    _ResetTwilightValues();
end

function Update()
    -- The night has fallen, no need to update anything anymore
    if (twilight_step >= 8) then
        return;
    end

    local elapsed_time = SystemManager:GetUpdateTime();
    local change = elapsed_time * 0.00003;

    --debug
    --total_time = total_time + elapsed_time;
    --print(total_time, twilight_red, twilight_green, twilight_blue, twilight_alpha)

    -- Update smoothly the colors to make the night fall.
    if (twilight_red > next_twilight_red) then
        twilight_red = twilight_red - change;
        if (twilight_red < next_twilight_red) then
            twilight_red = next_twilight_red;
        end
    elseif (twilight_red < next_twilight_red) then
        twilight_red = twilight_red + change;
        if (twilight_red > next_twilight_red) then
            twilight_red = next_twilight_red;
        end
    end

    if (twilight_green > next_twilight_green) then
        twilight_green = twilight_green - change;
        if (twilight_green < next_twilight_green) then
            twilight_green = next_twilight_green;
        end
    elseif (twilight_green < next_twilight_green) then
        twilight_green = twilight_green + change;
        if (twilight_green > next_twilight_green) then
            twilight_green = next_twilight_green;
        end
    end

    if (twilight_blue > next_twilight_blue) then
        twilight_blue = twilight_blue - change;
        if (twilight_blue < next_twilight_blue) then
            twilight_blue = next_twilight_blue;
        end
    elseif (twilight_blue < next_twilight_blue) then
        twilight_blue = twilight_blue + change;
        if (twilight_blue > next_twilight_blue) then
            twilight_blue = next_twilight_blue;
        end
    end

    if (twilight_alpha > next_twilight_alpha) then
        twilight_alpha = twilight_alpha - change;
        if (twilight_alpha < next_twilight_alpha) then
            twilight_alpha = next_twilight_alpha;
        end
    elseif (twilight_alpha < next_twilight_alpha) then
        twilight_alpha = twilight_alpha + change;
        if (twilight_alpha > next_twilight_alpha) then
            twilight_alpha = next_twilight_alpha;
        end
    end

    -- When enough time has been made to reach the next step, simply move to it
    if (twilight_red == next_twilight_red and twilight_green == next_twilight_green
            and twilight_blue == next_twilight_blue and twilight_alpha == next_twilight_alpha) then
        GlobalManager:GetGameEvents():SetEventValue("story", "layna_forest_twilight_value", twilight_step + 1);
        _ResetTwilightValues();
    end
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
