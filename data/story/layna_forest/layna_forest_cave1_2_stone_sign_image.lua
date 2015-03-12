------------------------------------------------------------------------------[[
-- Filename: layna_forest_cave1_2_stone_sign_image.lua
--
-- Description: Display an image of the stone sign text, in the actual
-- scripture seen by the characters for 5 seconds with fade in/out.
------------------------------------------------------------------------------]]

local ns = {}
setmetatable(ns, {__index = _G})
layna_forest_cave1_2_stone_sign_image = ns;
setfenv(1, ns);

local stone_sign = {};
local display_time = 0;

-- c++ objects instances
local Map = {};
local Script = {};

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();
    stone_sign = Script:CreateImage("data/story/layna_forest/stone_sign.png");
    stone_sign:SetDimensions(512.0, 256.0);
end

function Update()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("story", "layna_forest_cave1_2_show_sign_image") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("story", "layna_forest_cave1_2_show_sign_image") == 0) then
        return;
    end

    local time_expired = SystemManager:GetUpdateTime();

    -- Handle the timer
    display_time = display_time + time_expired;

    -- Start the timer
    if (display_time > 8000) then
        display_time = 0
        -- Disable the event at the end of it
        GlobalManager:SetEventValue("story", "layna_forest_cave1_2_show_sign_image", 0);
    end



end

local stone_sign_color = vt_video.Color(1.0, 1.0, 1.0, 0.9);

function DrawPostEffects()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("story", "layna_forest_cave1_2_show_sign_image") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("story", "layna_forest_cave1_2_show_sign_image") == 0) then
        return;
    end

    local text_alpha = 1.0;
    if (display_time >= 0 and display_time <= 2500) then
		text_alpha = display_time / 2500;
    elseif (display_time > 2500 and display_time <= 4500) then
        text_alpha = 1.0;
    elseif (display_time > 4500 and display_time <= 6000) then
        text_alpha = 1.0 - (display_time - 4500) / (6000 - 4500);
    elseif (display_time > 6000) then
        text_alpha = 0.0;
        return;
    end

    stone_sign_color:SetAlpha(0.9 * text_alpha);
    VideoManager:Move(512.0, 384.0);
    stone_sign:Draw(stone_sign_color);
end
