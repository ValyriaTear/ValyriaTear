------------------------------------------------------------------------------[[
-- Filename: ep1/to_be_continued_anim.lua
--
-- Description: Display the "To be continued..." text in front of a black background
------------------------------------------------------------------------------]]

local ns = {}
setmetatable(ns, {__index = _G})
to_be_continued_anim = ns;
setfenv(1, ns);

local dark_overlay = nil

local to_be_continued_text = nil

-- c++ objects instances
local Map = nil
local Script = nil
local Effects = nil

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();
    Effects = Map:GetEffectSupervisor();

    dark_overlay = Script:CreateImage("data/visuals/ambient/black.png");
    dark_overlay:SetDimensions(1024.0, 768.0);

    display_time = 0;

    to_be_continued_text = Script:CreateText(vt_system.Translate("To be continued..."), vt_video.TextStyle("text26"));
end

function Update()
    -- Only show the image if requested by the events
    if (GlobalManager:GetGameEvents():DoesEventExist("game", "to_be_continued") == false) then
        return;
    end

    if (GlobalManager:GetGameEvents():GetEventValue("game", "to_be_continued") == 0) then
        return;
    end

    local time_expired = SystemManager:GetUpdateTime();

    -- Handle the timer
    display_time = display_time + time_expired;
end

local text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawPostEffects()
    -- Only show the image if requested by the events
    if (GlobalManager:GetGameEvents():DoesEventExist("game", "to_be_continued") == false) then
        return;
    end

    if (GlobalManager:GetGameEvents():GetEventValue("game", "to_be_continued") == 0) then
        return;
    end

    -- Apply a dark overlay first.
    local overlay_alpha = 1.0;
    if (display_time >= 0 and display_time <= 2500) then
        overlay_alpha = display_time / 2500;
        text_color:SetAlpha(overlay_alpha);
    end
    VideoManager:Move(512.0, 768.0);
    dark_overlay:Draw(text_color);

    VideoManager:Move(612.0, 448.0);
    to_be_continued_text:Draw(text_color);
end
