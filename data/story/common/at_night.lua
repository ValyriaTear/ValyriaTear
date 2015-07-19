local ns = {}
setmetatable(ns, {__index = _G})
at_night = ns;
setfenv(1, ns);

local Script = nil
local night_layer = nil

-- TODO: Add support for multiple light layers, and get rid of this script.

-- add a evening light layer
function Initialize(battle_instance)
    Script = battle_instance:GetScriptSupervisor();

    -- Load a white empty image
    night_layer = Script:CreateImage("");
    night_layer:SetDimensions(1024.0, 768.0);
end

local night_color = vt_video.Color(0.0, 0.0, 0.24, 0.6);

function DrawForeground()
    -- Useful for the map mode
    Script:SetDrawFlag(vt_video.GameVideo.VIDEO_X_LEFT);
    Script:SetDrawFlag(vt_video.GameVideo.VIDEO_Y_TOP);
    VideoManager:Move(0.0, 0.0);
    night_layer:Draw(night_color);
end
