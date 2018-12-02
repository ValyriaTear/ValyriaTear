------------------------------------------------------------------------------[[
-- Filename: layna_forest_crystal_appearance_anim.lua
--
-- Description: Display an image of the stone sign text, in the actual
-- scripture seen by the characters for 5 seconds with fade in/out.
------------------------------------------------------------------------------]]

local ns = {}
setmetatable(ns, {__index = _G})
layna_forest_crystal_appearance_anim = ns;
setfenv(1, ns);

local crystal_shadow = nil
local lightning1 = nil
local lightning2 = nil
local lightning3 = nil
local vortex = nil
local vortex_effect = nil

local display_time = 0;

local lightning_time = 0;
local lightning1_triggered = false;
local lightning2_triggered = false;
local lightning3_triggered = false;
local lightning4_triggered = false;
local lightning5_triggered = false;
local lightning6_triggered = false;
local lightning7_triggered = false;
local lightning8_triggered = false;
local lightning9_triggered = false;

local lightning2_pos_x = 0.0;
local lightning2_pos_y = 0.0;
local lightning3_pos_x = 0.0;
local lightning3_pos_y = 0.0;
local lightning4_pos_x = 0.0;
local lightning4_pos_y = 0.0;
local lightning5_pos_x = 0.0;
local lightning5_pos_y = 0.0;
local lightning6_pos_x = 0.0;
local lightning6_pos_y = 0.0;
local lightning7_pos_x = 0.0;
local lightning7_pos_y = 0.0;
local lightning8_pos_x = 0.0;
local lightning8_pos_y = 0.0;
local lightning9_pos_x = 0.0;
local lightning9_pos_y = 0.0;

-- random ids for the last lightnings
local lightning4 = nil
local lightning5 = nil
local lightning6 = nil
local lightning7 = nil
local lightning8 = nil
local lightning9 = nil

local vortex_angle = 0.0;

local tremor_triggered = false;
local crystal_music_triggered = false;

-- The position of the crystal object, all the effects are drawn around it.
local crystal_position_x = 41.0;
local crystal_position_y = 44.0;

-- The actual screen position of the effects
local pos_x = 0.0;
local pos_y = 0.0;

-- c++ objects instances
local Map = nil
local Script = nil
local Effects = nil

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();
    Effects = Map:GetEffectSupervisor();

    crystal_shadow = Script:CreateImage("data/story/layna_forest/crystal_appearance/crystal_shadow.png");
    crystal_shadow:SetDimensions(20.0, 31.0);

    lightning1 = Script:CreateImage("data/story/layna_forest/crystal_appearance/blue_lightning1.png");
    lightning1:SetDimensions(248.0, 400.0);
    lightning2 = Script:CreateImage("data/story/layna_forest/crystal_appearance/blue_lightning2.png");
    lightning2:SetDimensions(118.0, 400.0);
    lightning3 = Script:CreateImage("data/story/layna_forest/crystal_appearance/blue_lightning3.png");
    lightning3:SetDimensions(171.0, 400.0);

    vortex = Script:CreateImage("data/story/layna_forest/crystal_appearance/vortex.png");
    vortex:SetDimensions(430.0, 210.0);

    -- Load the spring heal effect.
    vortex_effect = vt_map.ParticleObject.Create("data/visuals/particle_effects/crystal_appearance.lua",
                                            crystal_position_x, crystal_position_y, vt_map.MapMode.SKY_OBJECT);
    vortex_effect:Stop(); -- Don't run it until the whole animation starts

    -- Preload the crystal music
    AudioManager:LoadMusic("data/music/Soliloquy_1-OGA-mat-pablo.ogg", Map);

    display_time = 0;

    -- Init the random seed
    math.randomseed(os.time());
end

function Update()
    -- Only show the image if requested by the events
    if (GlobalManager:GetGameEvents():DoesEventExist("story", "layna_forest_crystal_appearance") == false) then
        return;
    end

    if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_crystal_appearance") == 0) then
        return;
    end

    local time_expired = SystemManager:GetUpdateTime();

    -- Handle the timer
    display_time = display_time + time_expired;
    lightning_time = lightning_time + time_expired;

    -- Start the timer
    if (display_time > 17000) then
        display_time = 0;
        -- Disable the event at the end of it
        GlobalManager:GetGameEvents():SetEventValue("story", "layna_forest_crystal_appearance", 0);
        return;
    end

    if (tremor_triggered == false and display_time >= 2000) then
        -- Trigger a tremor
        Effects:ShakeScreen(10.0, 13000, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_GRADUAL);
        AudioManager:PlaySound("data/sounds/rumble.wav");

        -- trigger also the particle effect
        vortex_effect:Start();

        tremor_triggered = true;
    end
    if (crystal_music_triggered == false and display_time >= 5000) then
        AudioManager:PlayMusic("data/music/Soliloquy_1-OGA-mat-pablo.ogg");

        crystal_music_triggered = true;
    end

    -- Update the effects position
    pos_x = Map:GetScreenXCoordinate(crystal_position_x);
    pos_y = Map:GetScreenYCoordinate(crystal_position_y);

    vortex_angle = vortex_angle - (time_expired / 20.0);
end

-- Get a x and y position around the crystal
function _GetRandomLightningXPlacement()
    return pos_x + (math.random() * 170.0) - 112.0;
end

function _GetRandomLightningYPlacement()
    return pos_y + (math.random() * 100.0) - 50.0;
end

function _GetRandomLightning()
    local lightning_chosen_id = math.random(1, 3);
    if (lightning_chosen_id == 1) then
        return lightning1;
    elseif (lightning_chosen_id == 2) then
        return lightning2;
    end
    return lightning3;
end

local white_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

-- Draw the lightning effects
function _DrawLightnings()
    -- trigger the lightnings
    -- The first one is always placed at the same place.
    if (lightning_time >= 2000 and lightning_time <= 2300) then
        if (lightning1_triggered == false) then
            lightning1_triggered = true;
            AudioManager:PlaySound("data/sounds/lightning.wav");
        end
        VideoManager:Move(pos_x + 42.0, pos_y - 2.0);
        lightning1:Draw(white_color);
    end

    if (lightning_time >= 3000 and lightning_time <= 3300) then
        if (lightning2_triggered == false) then
            lightning2_triggered = true;
            AudioManager:PlaySound("data/sounds/thunder.wav");
            lightning2_pos_x = _GetRandomLightningXPlacement();
            lightning2_pos_y = _GetRandomLightningYPlacement();
        end
        VideoManager:Move(lightning2_pos_x, lightning2_pos_y);
        lightning2:Draw(white_color);
    end

    if (lightning_time >= 3500 and lightning_time <= 3800) then
        if (lightning3_triggered == false) then
            lightning3_triggered = true;
            AudioManager:PlaySound("data/sounds/lightning.wav");
            lightning3_pos_x = _GetRandomLightningXPlacement();
            lightning3_pos_y = _GetRandomLightningYPlacement();
        end
        VideoManager:Move(lightning3_pos_x, lightning3_pos_y);
        lightning3:Draw(white_color);
    end

    if (lightning_time >= 4000 and lightning_time <= 4300) then
        if (lightning4_triggered == false) then
            lightning4_triggered = true;
            AudioManager:PlaySound("data/sounds/thunder.wav");
            lightning4_pos_x = _GetRandomLightningXPlacement();
            lightning4_pos_y = _GetRandomLightningYPlacement();
            lightning4 = _GetRandomLightning();
        end
        VideoManager:Move(lightning4_pos_x, lightning4_pos_y);
        lightning4:Draw(white_color);
    end

    if (lightning_time >= 4200 and lightning_time <= 4500) then
        if (lightning5_triggered == false) then
            lightning5_triggered = true;
            AudioManager:PlaySound("data/sounds/lightning.wav");
            lightning5_pos_x = _GetRandomLightningXPlacement();
            lightning5_pos_y = _GetRandomLightningYPlacement();
            lightning5 = _GetRandomLightning();
        end
        VideoManager:Move(lightning5_pos_x, lightning5_pos_y);
        lightning5:Draw(white_color);
    end

    if (lightning_time >= 4400 and lightning_time <= 4700) then
        if (lightning6_triggered == false) then
            lightning6_triggered = true;
            AudioManager:PlaySound("data/sounds/lightning.wav");
            lightning6_pos_x = _GetRandomLightningXPlacement();
            lightning6_pos_y = _GetRandomLightningYPlacement();
            lightning6 = _GetRandomLightning();
        end
        VideoManager:Move(lightning6_pos_x, lightning6_pos_y);
        lightning6:Draw(white_color);
    end

    if (lightning_time >= 4800 and lightning_time <= 5200) then
        if (lightning7_triggered == false) then
            lightning7_triggered = true;
            AudioManager:PlaySound("data/sounds/thunder.wav");
            lightning7_pos_x = _GetRandomLightningXPlacement();
            lightning7_pos_y = _GetRandomLightningYPlacement();
            lightning7 = _GetRandomLightning();
        end
        VideoManager:Move(lightning7_pos_x, lightning7_pos_y);
        lightning7:Draw(white_color);
    end

    if (lightning_time >= 5000 and lightning_time <= 5300) then
        if (lightning8_triggered == false) then
            lightning8_triggered = true;
            AudioManager:PlaySound("data/sounds/lightning.wav");
            lightning8_pos_x = _GetRandomLightningXPlacement();
            lightning8_pos_y = _GetRandomLightningYPlacement();
            lightning8 = _GetRandomLightning();
        end
        VideoManager:Move(lightning8_pos_x, lightning8_pos_y);
        lightning8:Draw(white_color);
    end

    if (lightning_time >= 5200 and lightning_time <= 5400) then
        if (lightning9_triggered == false) then
            lightning9_triggered = true;
            AudioManager:PlaySound("data/sounds/thunder.wav");
            lightning9_pos_x = _GetRandomLightningXPlacement();
            lightning9_pos_y = _GetRandomLightningYPlacement();
            lightning9 = _GetRandomLightning();
        end
        VideoManager:Move(lightning9_pos_x, lightning9_pos_y);
        lightning9:Draw(white_color);
    elseif (lightning_time > 5400) then
        -- loop the last lightnings until the effect is gone
        lightning_time = 4000;
        lightning4_triggered = false;
        lightning5_triggered = false;
        lightning6_triggered = false;
        lightning7_triggered = false;
        lightning8_triggered = false;
        lightning9_triggered = false;
    end
end

local overlay_color = vt_video.Color(0.0, 0.0, 0.0, 0.0);
local vortex_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local crystal_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local flash_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawPostEffects()
    -- Only show the image if requested by the events
    if (GlobalManager:GetGameEvents():DoesEventExist("story", "layna_forest_crystal_appearance") == false) then
        return;
    end

    if (GlobalManager:GetGameEvents():GetEventValue("story", "layna_forest_crystal_appearance") == 0) then
        return;
    end

    _DrawLightnings();

    -- Apply a dark overlay first.
    local overlay_alpha = 0.8;
    if (display_time >= 0 and display_time <= 2500) then
        overlay_alpha = 0.8 * (display_time / 2500);
        overlay_color:SetAlpha(overlay_alpha);
        Map:GetEffectSupervisor():EnableLightingOverlay(overlay_color);
    elseif (display_time > 2500 and display_time <= 6500) then
        overlay_alpha = 0.8;
        overlay_color:SetAlpha(overlay_alpha);
        Map:GetEffectSupervisor():EnableLightingOverlay(overlay_color);
    elseif (display_time > 6500 and display_time <= 8000) then
        overlay_alpha = 0.8 - (display_time - 6500) / (8000 - 6500);
        overlay_color:SetAlpha(overlay_alpha);
        Map:GetEffectSupervisor():EnableLightingOverlay(overlay_color);
    elseif (overlay_alpha > 0.0 and display_time > 8000) then
        overlay_alpha = 0.0;
        Map:GetEffectSupervisor():DisableLightingOverlay();
    end

    -- Then show the vortex
    local vortex_alpha = 0.0;
    local crystal_alpha = 0.0;
    if (display_time >= 4000 and display_time <= 5000) then
        vortex_alpha = 0.2 * (display_time - 4000) / (5000 - 4000);
        crystal_alpha = (display_time - 4000) / (5000 - 4000);
        Script:SetDrawFlag(vt_video.GameVideo.VIDEO_X_CENTER);
        Script:SetDrawFlag(vt_video.GameVideo.VIDEO_Y_CENTER);
        vortex_color:SetAlpha(vortex_alpha);
        crystal_color:SetAlpha(crystal_alpha);
        VideoManager:Move(pos_x, pos_y - 20.0);
        VideoManager:Rotate(vortex_angle);
        vortex:Draw(vortex_color);
        Script:SetDrawFlag(vt_video.GameVideo.VIDEO_Y_BOTTOM);
        VideoManager:Move(pos_x, pos_y - 4.0);
        crystal_shadow:Draw(crystal_color);
    elseif (display_time > 5000 and display_time <= 6500) then
        vortex_alpha = 0.2;
        crystal_alpha = 1.0;
        Script:SetDrawFlag(vt_video.GameVideo.VIDEO_X_CENTER);
        Script:SetDrawFlag(vt_video.GameVideo.VIDEO_Y_CENTER);
        vortex_color:SetAlpha(vortex_alpha);
        crystal_color:SetAlpha(crystal_alpha);
        VideoManager:Move(pos_x, pos_y - 20.0);
        VideoManager:Rotate(vortex_angle);
        vortex:Draw(vortex_color);
        Script:SetDrawFlag(vt_video.GameVideo.VIDEO_Y_BOTTOM);
        VideoManager:Move(pos_x, pos_y - 4.0);
        crystal_shadow:Draw(crystal_color);
    elseif (display_time > 6500 and display_time <= 10600) then
        vortex_alpha = 0.2 - 0.2 * (display_time - 6500) / (10600 - 6500);
        crystal_alpha = 1.0 - (display_time - 6500) / (10600 - 6500);
        vortex_color:SetAlpha(vortex_alpha);
        crystal_color:SetAlpha(crystal_alpha);
        Script:SetDrawFlag(vt_video.GameVideo.VIDEO_X_CENTER);
        Script:SetDrawFlag(vt_video.GameVideo.VIDEO_Y_CENTER);
        VideoManager:Move(pos_x, pos_y - 20.0);
        VideoManager:Rotate(vortex_angle);
        vortex:Draw(vortex_color);
        Script:SetDrawFlag(vt_video.GameVideo.VIDEO_Y_BOTTOM);
        VideoManager:Move(pos_x, pos_y - 4.0);
        crystal_shadow:Draw(crystal_color);
    elseif (vortex_alpha > 0.0 and display_time > 10600) then
        vortex_alpha = 0.0;
        crystal_alpha = 0.0;
    end

    -- Then apply a white flash
    local flash_alpha = 0.0;
    if (display_time >= 8000 and display_time <= 8500) then
        flash_alpha = 1.0 * (display_time - 8000) / (8500 - 8000);
        flash_color:SetAlpha(flash_alpha);
        Map:GetEffectSupervisor():EnableLightingOverlay(flash_color);
    elseif (display_time > 8500 and display_time <= 11000) then
        flash_alpha = 1.0;
        flash_color:SetAlpha(flash_alpha);
        Map:GetEffectSupervisor():EnableLightingOverlay(flash_color);
    elseif (display_time > 11000 and display_time <= 17000) then
        flash_alpha = 1.0 - (display_time - 11000) / (17000 - 11000);
        flash_color:SetAlpha(flash_alpha);
        Map:GetEffectSupervisor():EnableLightingOverlay(flash_color);
    elseif (flash_alpha > -1.0 and display_time > 17000) then
        flash_alpha = -1.0;
        Map:GetEffectSupervisor():DisableLightingOverlay();
        Map:GetParticleManager():StopAll(false);
    end
end
