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

local stone_sign_id = 0;
local display_time = 0;

-- c++ objects instances
local Map = {};
local Script = {};

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();
    stone_sign_id = Script:AddImage("dat/maps/layna_forest/stone_sign.png", 512.0, 256.0);
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
    elseif (display_time > 2500 and display_time <= 6500) then
        text_alpha = 1.0;
    elseif (display_time > 6500 and display_time <= 8000) then
        text_alpha = 1.0 - (display_time - 6500) / (8000 - 6500);
    elseif (display_time > 8000) then
        text_alpha = 0.0;
        return;
    end

    Script:DrawImage(stone_sign_id, 512, 384.0, hoa_video.Color(1.0, 1.0, 1.0, 0.9 * text_alpha));
end
