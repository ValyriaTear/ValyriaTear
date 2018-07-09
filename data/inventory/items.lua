------------------------------------------------------------------------------[[
-- Filename: items.lua
--
-- Description: This file contains the definitions of all items.
-- Each item has a unique integer identifier that is used
-- as its key in the items table below. Item IDs are unique not only among
-- each other, but among other inventory game objects as well (weapons, armor,
-- etc).
--
-- Object IDs 1 through 10,000 are reserved for items. Do not break this
-- limit, because other value ranges correspond to other types of inventory objects.
--
-- Item IDs do -not- need to be sequential. When you make a new item, keep it
-- grouped with similar item types (potions, scrolls, etc.) and keep a buffer of
-- space between group types. This way we won't get a mess of random items all over
-- this file.
--
-- All item entries needs the following data to be defined:
-- {name}: Text that defines the name of the item.
-- {description}: A brief description about the item.
-- {icon}: The filepath to the image icon representing this icon.
-- {target_type}: The type of target the item affects, which may be an attack point, actor, or party.
-- {standard_price}: The standard asking price of this weapon from merchants.
-- {warmup_time}: The time needed before using that item in battles.
-- {cooldown_time}: The time needed after using that item in battles.
-- {key_item}: Tells whether the item is a key item, preventing it from being consumed or sold.
--
-- Each item entry requires a function called {BattleUse} to be defined. This function implements the
-- use of item in battle, healing damage, causing status changes, playing sounds, and animating
-- sprites.
--
-- If an item is to be permitted for use outside of battle, an additional function {FieldUse} should
-- be defined. Defintion of this function is optional, but required for any items which are intended
-- to be used in the field. Their implementations will be similar to but more simple than their BattleUse
-- cousins as they do not need to be concerned with certain things such as sprite animations.
------------------------------------------------------------------------------]]

-- All item definitions are stored in this table
if (items == nil) then
    items = {}
end

--------------------------------------------------------------------------------
-- IDs 1 - 1,000 are reserved for healing potions
--------------------------------------------------------------------------------

function battle_healing_potion(target_actor, hit_points)
    if (target_actor:IsAlive() and target_actor:GetHitPoints() < target_actor:GetMaxHitPoints()) then
        target_actor:RegisterHealing(hit_points, true);
        AudioManager:PlaySound("data/sounds/potion_drink.wav");
        local battle_instance = ModeManager:GetTop();
        battle_instance:TriggerBattleParticleEffect("data/visuals/particle_effects/heal_particle.lua",
                                                    target_actor:GetXLocation(),
                                                    target_actor:GetYLocation());
        return true;
    else
        target_actor:RegisterMiss(false);
        return false;
    end
end

-- The return value tells the inventory whether the item was used successfully,
-- and then whether it can be removed from it.
function field_healing_potion(target, hit_points)
    if (target:IsAlive() and target:GetHitPoints() < target:GetMaxHitPoints()) then
        target:AddHitPoints(hit_points);
        AudioManager:PlaySound("data/sounds/potion_drink.wav");
        -- Trigger a particle effect on the menu character
        local y_pos = 218.0 + GlobalManager:GetPartyPosition(target) * 118.0;
        local menu_mode = ModeManager:GetTop();
        menu_mode:GetParticleManager():AddParticleEffect("data/visuals/particle_effects/heal_particle.lua", 145.0, y_pos);
        return true;
    else
        AudioManager:PlaySound("data/sounds/cancel.wav");
        return false;
    end
end

items[1] = {
    name = vt_system.Translate("Tiny Healing Potion"),
    description = vt_system.Translate("Restores a small amount of hit points to an ally."),
    icon = "data/inventory/items/potion_green_small.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,
    standard_price = 30,
    warmup_time = 1000,
    cooldown_time = 700,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 10,  -- Drunes
        [3100] = 5 -- Slimy material
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        return battle_healing_potion(target_actor, 60);
    end,

    FieldUse = function(target)
        return field_healing_potion(target, 75);
    end
}

items[2] = {
    name = vt_system.Translate("Small Healing Potion"),
    description = vt_system.Translate("Restores a reasonable amount of hit points to an ally."),
    icon = "data/inventory/items/potion_green_medium.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,
    standard_price = 200,
    warmup_time = 1200,
    cooldown_time = 900,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 60, -- Drunes
        [1] = 3   -- Tiny Potions
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        return battle_healing_potion(target_actor, 150);
    end,

    FieldUse = function(target)
        return field_healing_potion(target, 200);
    end
}

items[3] = {
    name = vt_system.Translate("Healing Potion"),
    description = vt_system.Translate("Restores a large amount of hit points to an ally."),
    icon = "data/inventory/items/potion_green_large.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,
    standard_price = 900,
    warmup_time = 1200,
    cooldown_time = 900,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 60, -- Drunes
        [2] = 4   -- Small healing potions
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        return battle_healing_potion(target_actor, 500);
    end,

    FieldUse = function(target)
        return field_healing_potion(target, 620);
    end
}

items[4] = {
    name = vt_system.Translate("Mega Healing Potion"),
    description = vt_system.Translate("Restores a very high amount of hit points to an ally."),
    icon = "data/inventory/items/potion_green_large.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,
    standard_price = 5000,
    warmup_time = 1200,
    cooldown_time = 1200,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 500, -- Drunes
        [3] = 4    -- Healing potions
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        return battle_healing_potion(target_actor, 9000);
    end,

    FieldUse = function(target)
        return field_healing_potion(target, 12000);
    end
}

-- Moon juices : Skill points
function battle_skill_potion(target_actor, skill_points)
    if (target_actor:IsAlive() and target_actor:GetSkillPoints() < target_actor:GetMaxSkillPoints()) then
        target_actor:RegisterHealing(skill_points, false);
        AudioManager:PlaySound("data/sounds/potion_drink.wav");
        local battle_instance = ModeManager:GetTop();
        battle_instance:TriggerBattleParticleEffect("data/visuals/particle_effects/heal_sp_particle.lua",
                                                    target_actor:GetXLocation(),
                                                    target_actor:GetYLocation());
        return true;
    else
        target_actor:RegisterMiss(false);
        return false;
    end
end

-- The return value tells the inventory whether the item was used successfully,
-- and then whether it can be removed from it.
function field_skill_potion(target, skill_points)
    if (target:IsAlive() and target:GetSkillPoints() < target:GetMaxSkillPoints()) then
        target:AddSkillPoints(skill_points);
        AudioManager:PlaySound("data/sounds/potion_drink.wav");
        return true;
    else
        AudioManager:PlaySound("data/sounds/cancel.wav");
        return false;
    end
end


items[11] = {
    name = vt_system.Translate("Tiny Moon Juice Potion"),
    description = vt_system.Translate("Restores a small amount of skill points to an ally."),
    icon = "data/inventory/items/potion_blue_small.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,
    standard_price = 90,
    use_warmup_time = 1000,
    cooldown_time = 1200,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 20,    -- Drunes
        [3102] = 1,  -- Insect material
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        return battle_skill_potion(target_actor, 40);
    end,

    FieldUse = function(target)
        return field_skill_potion(target, 45);
    end
}

items[12] = {
    name = vt_system.Translate("Small Moon Juice Potion"),
    description = vt_system.Translate("Restores a reasonable amount of skill points to an ally."),
    icon = "data/inventory/items/potion_blue_medium.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,
    standard_price = 380,
    use_warmup_time = 1000,
    cooldown_time = 1200,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 100,   -- Drunes
        [11] = 3,    -- Tiny Moon Juice
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        return battle_skill_potion(target_actor, 150);
    end,

    FieldUse = function(target)
        return field_skill_potion(target, 200);
    end
}

items[13] = {
    name = vt_system.Translate("Moon Juice Potion"),
    description = vt_system.Translate("Restores a large amount of skill points to an ally."),
    icon = "data/inventory/items/potion_blue_large.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,
    standard_price = 1300,
    use_warmup_time = 1000,
    cooldown_time = 1200,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 300,   -- Drunes
        [12] = 3,    -- Small Moon Juice
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        return battle_skill_potion(target_actor, 300);
    end,

    FieldUse = function(target)
        return field_skill_potion(target, 420);
    end
}

items[14] = {
    name = vt_system.Translate("Mega Moon Juice Potion"),
    description = vt_system.Translate("Restores a very high amount of skill points to an ally."),
    icon = "data/inventory/items/potion_blue_huge.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,
    standard_price = 6100,
    use_warmup_time = 1200,
    cooldown_time = 1300,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 1000,  -- Drunes
        [13] = 4,    -- Tiny Moon Juice
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        return battle_skill_potion(target_actor, 999);
    end,

    FieldUse = function(target)
        return field_skill_potion(target, 1200);
    end
}

items[15] = {
    name = vt_system.Translate("Lotus Petal"),
    description = vt_system.Translate("Cures moderate poisons from an ally."),
    icon = "data/inventory/items/lotus.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,
    standard_price = 100,
    use_warmup_time = 1200,
    cooldown_time = 1300,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        if (target_actor:IsAlive() == false) then
            return false;
        end

        local intensity = target_actor:GetActiveStatusEffectIntensity(vt_global.GameGlobal.GLOBAL_STATUS_HP);
        -- Can't heal somebody not poisoned
        if (intensity >= vt_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
            return false;
        elseif (intensity <= vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_GREATER) then
            -- Removes a bit of a bigger poison.
            target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_HP,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE, 120000);
        else
            target_actor:RemoveActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_HP);
        end

        return true;
    end,

    FieldUse = function(global_character)
        if (global_character:IsAlive() == false) then
            return false;
        end

        local intensity = global_character:GetActiveStatusEffectIntensity(vt_global.GameGlobal.GLOBAL_STATUS_HP);
        -- Can't heal somebody not poisoned
        if (intensity >= vt_global.GameGlobal.GLOBAL_INTENSITY_NEUTRAL) then
            return false;
        elseif (intensity <= vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_GREATER) then
            -- Removes a bit of a bigger poison.
            global_character:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_HP,
                                                     vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                                     80000);
        else
            global_character:RemoveActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_HP);
        end

        return true;
    end
}

items[16] = {
    name = vt_system.Translate("Candy"),
    description = vt_system.Translate("Makes an ally's health regenerate moderately."),
    icon = "data/inventory/items/candy.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,
    standard_price = 1300,
    use_warmup_time = 1200,
    cooldown_time = 1300,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 500,   -- Drunes
        [3101] = 15, -- Refined Slimy Material
        [3111] = 5,  -- Soft Powder
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        if (target_actor:IsAlive() == false) then
            return false;
        end

        target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_HP,
                                             vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE, 100000);
        return true;
    end,

    FieldUse = function(global_character)
        if (global_character:IsAlive() == false) then
            return false;
        end

        global_character:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_HP,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                                 60000);
        return true;
    end
}

--------------------------------------------------------------------------------
-- IDs 1,001 - 2,000 are reserved for status improvement potions
--------------------------------------------------------------------------------

_battle_apply_elixir_status_effects = function(target_actor, intensity)
    if (target_actor:IsAlive() == true) then
        -- decrement all the basic negative effects, or put positive effects depending on the intensity
        target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK, intensity, 120000);
        target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_MAG_ATK, intensity, 120000);
        target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_PHYS_DEF, intensity, 120000);
        target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_MAG_DEF, intensity, 120000);
        target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STAMINA, intensity, 120000);
        target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_EVADE, intensity, 120000);
        AudioManager:PlaySound("data/sounds/potion_drink.wav");
        local battle_instance = ModeManager:GetTop();
        battle_instance:TriggerBattleParticleEffect("data/visuals/particle_effects/elixir_particle.lua",
                                                    target_actor:GetXLocation(),
                                                    target_actor:GetYLocation());
        return true;
    else
        target_actor:RegisterMiss(false);
        return false;
    end
end

_field_apply_elixir_status_effects = function(global_character, intensity)
    if (global_character:IsAlive() == true) then
        -- decrement all the basic negative effects, or put positive effects depending on the intensity
        global_character:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK, intensity, 120000);
        global_character:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_MAG_ATK, intensity, 120000);
        global_character:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_PHYS_DEF, intensity, 120000);
        global_character:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_MAG_DEF, intensity, 120000);
        global_character:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STAMINA, intensity, 120000);
        global_character:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_EVADE, intensity, 120000);
        AudioManager:PlaySound("data/sounds/potion_drink.wav");
        return true;
    else
        return false;
    end
end

items[1001] = {
    name = vt_system.Translate("Minor Elixir"),
    description = vt_system.Translate("Revives a character, or improves the character's status when he/she is alive by a limited degree."),
    icon = "data/inventory/items/potion_red_small.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY_EVEN_DEAD,
    standard_price = 150,
    use_warmup_time = 1200,
    cooldown_time = 1800,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 10,   -- Drunes
        [3116] = 1, -- Orb
        [1] = 2,    -- Tiny Potions
        [3102] = 1, -- Insect Material
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        -- Decrement all base stats active negative status effects slightly
        if (target_actor:GetHitPoints() > 0) then
            -- Decrement any active negative base stats status effects when alive
            return _battle_apply_elixir_status_effects(target_actor, vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER);
        else
            -- When dead, revive the character
            target_actor:RegisterRevive(1);
            AudioManager:PlaySound("data/sounds/potion_drink.wav");
        end
        return true;
    end,

    FieldUse = function(target)
        if (target:GetHitPoints() > 0) then
            -- increment active base stats status effects when alive.
            return _field_apply_elixir_status_effects(target, vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER);
        else
            -- When dead, revive the character
            target:SetHitPoints(1);
            AudioManager:PlaySound("data/sounds/potion_drink.wav");
        end
        return true;
    end
}

items[1003] = {
    name = vt_system.Translate("Elixir"),
    description = vt_system.Translate("Revives a character to half of their hit points, or reasonably improves the character's status while he/she is alive."),
    icon = "data/inventory/items/potion_red_large.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY_EVEN_DEAD,
    standard_price = 1200,
    use_warmup_time = 1600,
    cooldown_time = 2100,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 200,  -- Drunes
        [1001] = 5, -- Minor Elixirs
        [3107] = 3, -- Saurian Material
        [3114] = 5, -- Fish Meat
        [15] = 1,   -- Lotus Petal
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        if (target_actor:GetHitPoints() > 0) then
            -- Decrement any active negative base stats status effects when alive
            return _battle_apply_elixir_status_effects(target_actor, vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE);
        else
            -- When dead, revive the character
            target_actor:RegisterRevive(target_actor:GetMaxHitPoints() / 2.0);
        end
        return true;
    end,

    FieldUse = function(target)
        if (target:GetHitPoints() > 0) then
            -- increment active base stats status effects when alive.
            return _field_apply_elixir_status_effects(target, vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE);
        else
            -- When dead, revive the character
            target:SetHitPoints(target_actor:GetMaxHitPoints() / 2.0);
        end
        return true;
    end
}

items[1004] = {
    name = vt_system.Translate("Periwinkle Potion"),
    description = vt_system.Translate("Gives a reasonable boost in Physical Attack to an ally for a large amount of time."),
    icon = "data/inventory/items/strength_potion.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,
    standard_price = 1600,
    use_warmup_time = 3600,
    cooldown_time = 2100,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 400,   -- Drunes
        [3113] = 10, -- Red Meat
        [1001] = 2,  -- Minor Elixirs
        [11] = 2,    -- Tiny Moon Juice
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        if (target_actor:IsAlive() == true) then
            -- Decrement any active negative base stats status effects when alive
            target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                                 120000);
            AudioManager:PlaySound("data/sounds/potion_drink.wav");
            return true;
        end
        return false;
    end,

    FieldUse = function(target)
        if (target:IsAlive() == true) then
            -- increment active base stats status effects when alive.
            target:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK,
                                           vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                           90000);
            AudioManager:PlaySound("data/sounds/potion_drink.wav");
            return true;
        end
        return false;
    end
}

items[1005] = {
    name = vt_system.Translate("Haste Potion"),
    description = vt_system.Translate("Gives a reasonable boost in stamina to an ally for a small amount of time."),
    icon = "data/inventory/items/haste_potion.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALLY,
    standard_price = 1400,
    use_warmup_time = 1600,
    cooldown_time = 2100,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 300,   -- Drunes
        [3113] = 5,  -- Red Meat
        [3117] = 1,  -- Stinging Material
        [1001] = 2,  -- Minor Elixirs
        [3111] = 2,  -- Soft Powder
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        if (target_actor:IsAlive() == true) then
            -- Decrement any active negative base stats status effects when alive
            target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STAMINA,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                                 90000);
            AudioManager:PlaySound("data/sounds/potion_drink.wav");
            return true;
        end
        return false;
    end,

    FieldUse = function(target)
        if (target:IsAlive() == true) then
            -- increment active base stats status effects when alive.
            target:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_STAMINA,
                                           vt_global.GameGlobal.GLOBAL_INTENSITY_POS_MODERATE,
                                           60000);
            AudioManager:PlaySound("data/sounds/potion_drink.wav");
            return true;
        end
        return false;
    end
}

items[1006] = {
    name = vt_system.Translate("Poison Potion"),
    description = vt_system.Translate("Poisons an enemy for a small amount of time."),
    icon = "data/inventory/items/poison_potion.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_FOE,
    standard_price = 1400,
    use_warmup_time = 1600,
    cooldown_time = 2100,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_item_single.lua",
        [KALYA] = "data/battles/characters_animations/all_use_item_single.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_item_single.lua",
        [THANIS] = "data/battles/characters_animations/all_use_item_single.lua"
    },

    trade_conditions = {
        [0] = 350,   -- Drunes
        [3101] = 2,  -- Refined Slimy Material
        [3107] = 5,  -- Saurian Material
        [3102] = 5,  -- Insect Material
    },

    BattleUse = function(user, target)
        local target_actor = target:GetActor();
        if (target_actor:IsAlive() == true) then
            -- Decrement any active negative base stats status effects when alive
            target_actor:ApplyActiveStatusEffect(vt_global.GameGlobal.GLOBAL_STATUS_HP,
                                                 vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE,
                                                 60000);
            return true;
        end
        return false;
    end,

    -- Can't be used from menu
    --FieldUse = function(target)
        --return false;
    --end
}

--------------------------------------------------------------------------------
-- IDs 2,001 - 3,000 are reserved for elemental potions
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
-- IDs 3,001 - 4,000 are reserved for improvement items
--------------------------------------------------------------------------------
items[3001] = {
    name = vt_system.Translate("Copper Ore"),
    description = vt_system.Translate("A small amount of copper ore."),
    icon = "data/inventory/items/copper_ore.png",
    standard_price = 150
}

items[3002] = {
    name = vt_system.Translate("Iron Ore"),
    description = vt_system.Translate("A small amount of iron ore."),
    icon = "data/inventory/items/iron_ore.png",
    standard_price = 450
}

items[3003] = {
    name = vt_system.Translate("Titanium Ore"),
    description = vt_system.Translate("A small amount of titanium ore."),
    icon = "data/inventory/items/titanium_ore.png",
    standard_price = 650
}

items[3004] = {
    name = vt_system.Translate("Anthozium Ore"),
    description = vt_system.Translate("A small amount of anthozium ore."),
    icon = "data/inventory/items/anthozium_ore.png",
    standard_price = 850
}

items[3005] = {
    name = vt_system.Translate("Auridium Ore"),
    description = vt_system.Translate("A small amount of auridium ore."),
    icon = "data/inventory/items/auridium_ore.png",
    standard_price = 1050
}

items[3006] = {
    name = vt_system.Translate("Borium Ore"),
    description = vt_system.Translate("A small amount of borium ore."),
    icon = "data/inventory/items/borium_ore.png",
    standard_price = 1250
}

items[3007] = {
    name = vt_system.Translate("Evanium Ore"),
    description = vt_system.Translate("A small amount of evanium ore."),
    icon = "data/inventory/items/evanium_ore.png",
    standard_price = 1500
}

items[3008] = {
    name = vt_system.Translate("Phoenix Feather"),
    description = vt_system.Translate("A genuine and perfect feather from the great fire bird. It is known to bring life to inanimated items."),
    icon = "data/inventory/items/phoenix_feather.png",
    standard_price = 4500
}

-- Basic Material

items[3100] = {
    name = vt_system.Translate("Slimy Material"),
    description = vt_system.Translate("Material provided by slimy creatures."),
    icon = "data/inventory/items/slimy_material.png",
    standard_price = 4
}

items[3101] = {
    name = vt_system.Translate("Refined Slimy Material"),
    description = vt_system.Translate("Quality material provided by slimy creatures."),
    icon = "data/inventory/items/slimy_material_refined.png",
    standard_price = 40
}

items[3102] = {
    name = vt_system.Translate("Insect Material"),
    description = vt_system.Translate("Material provided by various insects."),
    icon = "data/inventory/items/insect_material.png",
    standard_price = 10
}

items[3103] = {
    name = vt_system.Translate("Refined Insect Material"),
    description = vt_system.Translate("Quality material provided by various insects."),
    icon = "data/inventory/items/insect_material_refined.png",
    standard_price = 100
}

items[3104] = {
    name = vt_system.Translate("Fur"),
    description = vt_system.Translate("Material provided by various creatures."),
    icon = "data/inventory/items/fur.png",
    standard_price = 20
}

items[3105] = {
    name = vt_system.Translate("Soft Fur"),
    description = vt_system.Translate("Material provided by rare animals."),
    icon = "data/inventory/items/soft_fur.png",
    standard_price = 200
}

items[3106] = {
    name = vt_system.Translate("Silver Fur"),
    description = vt_system.Translate("Highly sought-after material provided by very special creatures."),
    icon = "data/inventory/items/silver_fur.png",
    standard_price = 2000
}

items[3107] = {
    name = vt_system.Translate("Saurian Material"),
    description = vt_system.Translate("Material provided by various reptilians."),
    icon = "data/inventory/items/saurian_material.png",
    standard_price = 40
}

items[3108] = {
    name = vt_system.Translate("Refined Saurian Material"),
    description = vt_system.Translate("Quality material provided by various reptilians."),
    icon = "data/inventory/items/saurian_material_refined.png",
    standard_price = 400
}

items[3109] = {
    name = vt_system.Translate("Rodent Material"),
    description = vt_system.Translate("Material provided by small rodents."),
    icon = "data/inventory/items/rodent_material.png",
    standard_price = 30
}

items[3110] = {
    name = vt_system.Translate("Refined Rodent Material"),
    description = vt_system.Translate("Quality material provided by not-that-small rodents."),
    icon = "data/inventory/items/rodent_material_refined.png",
    standard_price = 300
}

items[3111] = {
    name = vt_system.Translate("Soft Powder"),
    description = vt_system.Translate("Material provided by various creatures."),
    icon = "data/inventory/items/soft_powder.png",
    standard_price = 50
}

items[3112] = {
    name = vt_system.Translate("Silk"),
    description = vt_system.Translate("Material provided by a few creatures."),
    icon = "data/inventory/items/silk.png",
    standard_price = 56
}

items[3113] = {
    name = vt_system.Translate("Red Meat"),
    description = vt_system.Translate("Material provided by warm-blooded creatures."),
    icon = "data/inventory/items/red_meat.png",
    standard_price = 80
}

items[3114] = {
    name = vt_system.Translate("Fish Meat"),
    description = vt_system.Translate("Material provided by various underwater creatures."),
    icon = "data/inventory/items/fish_meat.png",
    standard_price = 70
}

items[3115] = {
    name = vt_system.Translate("White Meat"),
    description = vt_system.Translate("Material provided by various creatures."),
    icon = "data/inventory/items/white_meat.png",
    standard_price = 80
}

items[3116] = {
    name = vt_system.Translate("Orb"),
    description = vt_system.Translate("Material provided by a few creatures."),
    icon = "data/inventory/items/orb.png",
    standard_price = 60
}

items[3117] = {
    name = vt_system.Translate("Stinging Material"),
    description = vt_system.Translate("Material provided by a few creatures."),
    icon = "data/inventory/items/stinging_material.png",
    standard_price = 90
}

items[3118] = {
    name = vt_system.Translate("Thick Fabric"),
    description = vt_system.Translate("Common fabric, but of good quality."),
    icon = "data/inventory/items/thick_fabric.png",
    standard_price = 20
}

items[3119] = {
    name = vt_system.Translate("Fine Wood"),
    description = vt_system.Translate("Common but durable wood. Always useful"),
    icon = "data/inventory/items/fine_wood.png",
    standard_price = 15
}

--------------------------------------------------------------------------------
-- IDs 4,001 - 5,000 are reserved for items with special effects
--------------------------------------------------------------------------------

-- Gets the average stamina value of the hero party.
function _GetAverageHeroesEvasionLevel()
    local id = 0;
    local stamina_sum = 0.0;

    local battle_instance = ModeManager:GetTop();
    local nb_heroes = battle_instance:GetNumberOfCharacters();
    while id < nb_heroes do
        local hero = battle_instance:GetCharacterActor(id);
        if (hero ~= nil and hero:CanFight() == true) then
            stamina_sum = stamina_sum + hero:GetStamina();
        end
    id = id + 1;
    end

    if (id == 0) then
        return 0.0;
    end
    return stamina_sum / id;
end

-- Gets the average stamina value of the enemy party.
function _GetAverageEnemiesStaminaLevel()
    local id = 0;
    local stamina_sum = 0.0;

    local battle_instance = ModeManager:GetTop();
    local nb_enemies = battle_instance:GetNumberOfEnemies();
    while id < nb_enemies do
        local enemy = battle_instance:GetEnemyActor(id);
        if (enemy ~= nil and enemy:CanFight() == true) then
            stamina_sum = stamina_sum + enemy:GetStamina();
        end
    id = id + 1;
    end

    if (id == 0) then
        return 0.0;
    end
    return stamina_sum / id;
end

items[4001] = {
    name = vt_system.Translate("Escape Smoke"),
    description = vt_system.Translate("A ninja potion that bursts out a damp mist when it is crashed on the ground. Used to hopefully escape from standard opponents."),
    icon = "data/inventory/items/escape_smoke.png",
    target_type = vt_global.GameGlobal.GLOBAL_TARGET_ALL_ALLIES,
    standard_price = 15,

    use_warmup_time = 1600,
    cooldown_time = 2100,

    animation_scripts = {
        [BRONANN] = "data/battles/characters_animations/all_use_escape_smoke.lua",
        [KALYA] = "data/battles/characters_animations/all_use_escape_smoke.lua",
        [SYLVE] = "data/battles/characters_animations/all_use_escape_smoke.lua",
        [THANIS] = "data/battles/characters_animations/all_use_escape_smoke.lua"
    },

    trade_conditions = {
        [0] = 5,    -- Drunes
        [3102] = 2,  -- Insect Material
    },

    BattleUse = function(user, target)
        local battle_instance = ModeManager:GetTop();
        -- If it's a boss battle, this can't work at all
        if (battle_instance:IsBossBattle() == true) then
            return true;
        end

        -- % chance to miss due to too high stamina from enemies, for instance
        -- Give a slight advantage to the party.
        local evade_diff = 60.0 + _GetAverageHeroesEvasionLevel() - _GetAverageEnemiesStaminaLevel();
        if (math.random(0, 100) > evade_diff) then
            local target_actor = target:GetActor();
            return true;
        end

        -- Quit the battle (A parent mode should always be there to take the relay)
        battle_instance:SetSceneMode(true);
        ModeManager:Pop(true, true);

        -- Remove the item from inventory, as the battle mode won't handle it properly
        -- if at least one succeed during the battle.
        GlobalManager:DecrementItemCount(4001, 1)
        return true;
    end,

    -- Can't be used from menu
    -- TODO: Add support for this with the "home" map feature.
    --FieldUse = function(target)
        --return false;
    --end
}

--------------------------------------------------------------------------------
-- IDs 70001-80000 are reserved for "simple" key items
--------------------------------------------------------------------------------
items[70001] = {
    name = vt_system.Translate("Pen"),
    description = vt_system.Translate("Georges's pen, presumably used to write poetry."),
    icon = "data/inventory/items/key_items/ink.png",
    standard_price = 0,
    key_item = true
}

items[70002] = {
    name = vt_system.Translate("Barley Meal"),
    description = vt_system.Translate("Basic yet good and cheap flour."),
    icon = "data/inventory/items/key_items/barley_meal_bag.png",
    standard_price = 0,
    key_item = true
}

-- old item: To remove later.
items[70003] = {
    name = vt_system.Translate("Wolfpain Necklace"),
    description = vt_system.Translate("A strange necklace with unknown powers."),
    icon = "data/inventory/items/key_items/necklace_wolfpain.png",
    standard_price = 0,
    key_item = true,
}
