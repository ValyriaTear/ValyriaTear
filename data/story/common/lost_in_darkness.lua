local ns = {}
setmetatable(ns, {__index = _G})
lost_in_darkness = ns;
setfenv(1, ns);

local Script = nil
local darkness_layer = nil

-- TODO #611: Add support for multiple light layers, and get rid of this script.

-- add a evening light layer
function Initialize(battle_instance)
    Script = battle_instance:GetScriptSupervisor();

    -- Load a white empty image
    darkness_layer = Script:CreateImage("");
    darkness_layer:SetDimensions(1024.0, 768.0);
end

local darkness_color = vt_video.Color(0.0, 0.0, 0.0, 0.8);

function DrawForeground()
    -- Useful for the map mode
    Script:SetDrawFlag(vt_video.GameVideo.VIDEO_X_LEFT);
    Script:SetDrawFlag(vt_video.GameVideo.VIDEO_Y_TOP);
    VideoManager:Move(0.0, 0.0);
    darkness_layer:Draw(darkness_color);
end
