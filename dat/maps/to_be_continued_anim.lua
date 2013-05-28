------------------------------------------------------------------------------[[
-- Filename: to_be_continued_anim.lua
--
-- Description: Display the "To be continued..." text in front of a black background
------------------------------------------------------------------------------]]

local ns = {}
setmetatable(ns, {__index = _G})
to_be_continued_anim = ns;
setfenv(1, ns);

local dark_overlay_id = -1;

local to_be_continued_text = {};

-- c++ objects instances
local Map = {};
local Script = {};
local Effects = {};

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();
    Effects = Map:GetEffectSupervisor();

    dark_overlay_id = Script:AddImage("img/ambient/black.png", 1024.0, 768.0);

    display_time = 0;

    to_be_continued_text = vt_system.Translate("To be continued...");
end

function Update()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("game", "to_be_continued") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("game", "to_be_continued") == 0) then
        return;
    end

    local time_expired = SystemManager:GetUpdateTime();

    -- Handle the timer
    display_time = display_time + time_expired;


end

local text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawPostEffects()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("game", "to_be_continued") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("game", "to_be_continued") == 0) then
        return;
    end

    -- Apply a dark overlay first.
    local overlay_alpha = 1.0;
    if (display_time >= 0 and display_time <= 2500) then
        overlay_alpha = display_time / 2500;
        text_color:SetAlpha(overlay_alpha);
    end
    Script:DrawImage(dark_overlay_id, 512.0, 768.0, text_color);

    VideoManager:DrawText(to_be_continued_text, 612.0, 448.0, text_color);

end
