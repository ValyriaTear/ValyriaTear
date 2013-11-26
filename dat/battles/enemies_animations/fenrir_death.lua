-- Set the namespace
local ns = {};
setmetatable(ns, {__index = _G});
fenrir_death = ns;
setfenv(1, ns);

-- local references
local enemy = {};
local enemy_pos_x = 0.0;
local enemy_pos_y = 0.0;
local enemy_origin_x = 0.0;
local enemy_origin_y = 0.0;

local elapsed_time = 0;

local Battle = {};
local Script = {};
local Effects = {};

local light1_image = 0;
local light2_image = 0;
local light3_image = 0;

local lightning_1_triggered = false;
local lightning_1_stopped = false;
local lightning_2_triggered = false;
local lightning_2_stopped = false;
local lightning_3_triggered = false;
local lightning_3_stopped = false;

local death_particles_triggered = false;
local rumble_triggered = false;

-- battle, the Game mode
-- _enemy, the BattleEnemy dying
function Initialize(battle, _enemy)
    -- Keep the reference in memory
    Battle = battle;
    enemy = _enemy;
    Script = Battle:GetScriptSupervisor();
    Effects = Battle:GetEffectSupervisor();

    enemy_pos_x = 0.0;
    enemy_pos_y = 0.0;
    enemy_origin_x = enemy:GetXLocation();
    enemy_origin_y = enemy:GetYLocation();

    elapsed_time = 0;

    lightning_1_triggered = false;
    lightning_2_triggered = false;
    lightning_3_triggered = false;
    lightning_1_stopped = false;
    lightning_2_stopped = false;
    lightning_3_stopped = false;

    death_particles_triggered = false;
    rumble_triggered = false;

    light1_image = Script:CreateImage("dat/battles/enemies_animations/death_light_1.png");
    light2_image = Script:CreateImage("dat/battles/enemies_animations/death_light_2.png");
    light3_image = Script:CreateImage("dat/battles/enemies_animations/death_light_3.png");

    -- Fade out the battle music to make the event more dramatic
    AudioManager:FadeOutAllMusic(2000);
end

local white_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local overlay_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function Update()

    enemy_pos_x = enemy_origin_x;
    enemy_pos_y = enemy_origin_y;

    -- Make the enemy shake while dying
    if (rumble_triggered == true) then
        enemy:SetXLocation(enemy_pos_x + (math.random() * 6.0) - 3.0);
    end

    elapsed_time = elapsed_time + SystemManager:GetUpdateTime();

    if (lightning_1_triggered == false and elapsed_time > 800) then
        Effects:EnableLightingOverlay(white_color);
        AudioManager:PlaySound("snd/magic_blast.ogg");
        lightning_1_triggered = true;
    elseif (lightning_1_stopped == false and elapsed_time > 1000 ) then
        Effects:DisableLightingOverlay();
        lightning_1_stopped = true;
    end

    if (lightning_2_triggered == false and elapsed_time > 1700) then
        Effects:EnableLightingOverlay(white_color);
        AudioManager:PlaySound("snd/magic_blast.ogg");
        lightning_2_triggered = true;
    elseif (lightning_2_stopped == false and elapsed_time > 1900 ) then
        Effects:DisableLightingOverlay();
        lightning_2_stopped = true;
    end

    if (lightning_3_triggered == false and elapsed_time > 3000) then
        Effects:EnableLightingOverlay(white_color);
        AudioManager:PlaySound("snd/magic_blast.ogg");
        lightning_3_triggered = true;
    elseif (lightning_3_stopped == false and elapsed_time > 3400) then
        Effects:DisableLightingOverlay();
        lightning_3_stopped = true;
    end

    if (elapsed_time >= 4000 and elapsed_time < 8000) then
        overlay_color:SetAlpha(0.7 * (elapsed_time - 4000.0) / (8000.0 - 4000.0));
        Effects:EnableLightingOverlay(overlay_color);

        -- Trigger the rumble and sprite shaking
        if (rumble_triggered == false) then
            AudioManager:PlaySound("snd/rumble.wav");
            rumble_triggered = true;
        end
    elseif (elapsed_time >= 8000 and elapsed_time < 10000) then
        overlay_color:SetAlpha(1.0 - (elapsed_time - 8000.0) / (10000.0 - 8000.0));
        Effects:EnableLightingOverlay(overlay_color);
    end

    -- Triggers snow representing the boss body ashes...
    if (death_particles_triggered == false and elapsed_time > 8000) then
        -- Make the wolf howls at the end
        AudioManager:PlaySound("snd/wolf_howl.ogg");

        Battle:TriggerBattleParticleEffect("dat/effects/particles/boss_death_particle.lua",
                                           enemy_pos_x + 200, enemy_pos_y - 255);
        Battle:TriggerBattleParticleEffect("dat/effects/particles/boss_death_particle.lua",
                                           enemy_pos_x - 185, enemy_pos_y - 321);
        Battle:TriggerBattleParticleEffect("dat/effects/particles/boss_death_particle.lua",
                                           enemy_pos_x - 146, enemy_pos_y - 149);
        Battle:TriggerBattleParticleEffect("dat/effects/particles/boss_death_particle.lua",
                                           enemy_pos_x + 55, enemy_pos_y - 140);
        death_particles_triggered = true;
    end

    if (elapsed_time >= 7000 and elapsed_time <= 9000.0) then
        -- Also fade out the enemy
        enemy:SetSpriteAlpha(1.0 - ((elapsed_time - 7000.0) / (9000.0 - 7000.0)));
    end

    -- end the boss death sequence
    if (elapsed_time > 13000) then
        return true;
    end

    return false;
end

local light1_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local light2_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local light3_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawOnSprite()
    if (elapsed_time < 7800) then
        light1_color:SetAlpha(0.5 * elapsed_time / 4000.0);
        VideoManager:Move(enemy_pos_x, enemy_pos_y + 30);
        light2_image:Draw(light1_color);
    end
    if (elapsed_time >= 3000 and elapsed_time < 7800) then
        light2_color:SetAlpha(0.5 * (elapsed_time - 3000.0) / (7800.0 - 3000.0));
        VideoManager:Move(enemy_pos_x, enemy_pos_y + 30);
        light1_image:Draw(light2_color);
    end
    if (elapsed_time >= 7800 and elapsed_time < 10000) then
        light3_color:SetAlpha(0.5 - 0.5 * ((elapsed_time - 8000.0) / (9000.0 - 8000.0)));
        VideoManager:Move(enemy_pos_x, enemy_pos_y + 30);
        light3_image:Draw(light3_color);
    end
end
