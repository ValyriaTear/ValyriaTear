local ns = {}
setmetatable(ns, {__index = _G})
at_night = ns;
setfenv(1, ns);

local Script = {};
local night_layer_id = {};

-- TODO: Add support for multiple light layers, and get rid of this script.

-- add a evening light layer
function Initialize(battle_instance)
    Script = battle_instance:GetScriptSupervisor();

    -- Load a white empty image
    night_layer_id = Script:AddImage("", 1024.0, 768.0);
end

function DrawForeground()
    -- Useful for the map mode
    Script:SetDrawFlag(hoa_video.GameVideo.VIDEO_X_LEFT);
    Script:SetDrawFlag(hoa_video.GameVideo.VIDEO_Y_TOP);
    Script:DrawImage(night_layer_id, 0.0, 0.0, hoa_video.Color(0.0, 0.0, 0.24, 0.6));
end
