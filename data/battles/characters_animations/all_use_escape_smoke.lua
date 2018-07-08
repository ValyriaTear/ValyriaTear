-- Set the namespace
local ns = {};
setmetatable(ns, {__index = _G});
all_use_escape_smoke = ns;
setfenv(1, ns);

-- local references
local character = nil
local target = nil
local target_actor = nil
local battle_item = nil

local item_step = 0;
local item_time = 0.0;

local effect_triggered = false;

local Battle = nil
local Effects = nil

local flash_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

-- character, the BattleActor using the item
-- target, the BattleEnemy target
-- The battle item used on target
function Initialize(_character, _target, _battle_item)
    -- Keep the reference in memory
    character = _character
    target = _target
    target_actor = _target:GetActor()
    battle_item = _battle_item

    Battle = ModeManager:GetTop();
    Effects = Battle:GetEffectSupervisor();

    flash_alpha = 0.0;

    item_step = 0;
    item_time = 0;

    effect_triggered = false;
end

function Update()
    -- Handle the timer
    item_time = item_time + SystemManager:GetUpdateTime()

    -- triggers the icon display
    if (item_step == 0) then
        Battle:GetIndicatorSupervisor():AddItemIndicator(character:GetXLocation() + (character:GetSpriteWidth() / 2.0),
                                                         character:GetYLocation() - character:GetSpriteHeight(),
                                                         battle_item:GetGlobalItem());
        item_step = 1;
        return false
    end

    -- Trigger the throw escape smoke animation
    if (item_step == 1 and item_time > 600) then
        character:ChangeSpriteAnimation("escape_smoke")
        item_step = 2
    end

    if (item_step == 2 and item_time > 1500) then
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/smoke.lua", 452, 424);
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/smoke.lua", 350, 550);
        Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/smoke.lua", 555, 560);
        AudioManager:PlaySound("data/sounds/steam_hisses.ogg");

        item_step = 3
    end

    -- Trigger smoke and fade out
    if (item_step == 3 and item_time > 2000) then

        local display_time = item_time - 2000

        -- The flash alpha
        if (display_time >= 0 and display_time <= 500) then
            flash_alpha = display_time / 500;
        elseif (display_time > 500 and display_time <= 1500) then
            flash_alpha = 1.0;
            character:ChangeSpriteAnimation("idle")
        elseif (display_time > 1500 and display_time <= 4000) then
            flash_alpha = 1.0 - (display_time - 1500) / (4000 - 1500);
        elseif (display_time > 4000) then
            flash_alpha = 0.0;
        end
        flash_color:SetAlpha(flash_alpha)
        Effects:EnableLightingOverlay(flash_color);

        -- Triggers the effect in the middle of the flashbang animation
        if (effect_triggered == false and item_time > 3000) then
            battle_item:ExecuteBattleFunction(character, target)
            effect_triggered = true;
        end

        -- When it fails ...
        if (item_time > 5500) then
            character:RegisterMiss(false);
            AudioManager:PlaySound("data/sounds/cancel.wav");
            Effects:DisableLightingOverlay()
            return true
        end

        return false
    end

    return false;
end
