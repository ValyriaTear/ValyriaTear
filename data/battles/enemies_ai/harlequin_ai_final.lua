local ns = {}
setmetatable(ns, {__index = _G})
harlequin_ai_final = ns
setfenv(1, ns)

-- Useful common functions
-- -----------------------
function _GetHeroWithMostSP(battle_instance)
    local id = 0;
    local most_sp = -1;
    local id_most_sp = -1;
    local nb_heroes = battle_instance:GetNumberOfCharacters();
    while id < nb_heroes do
        local hero = battle_instance:GetCharacterActor(id);
        if (hero ~= nil and hero:CanFight() == true) then
            if (hero:GetSkillPoints() > most_sp) then
                most_sp = hero:GetSkillPoints();
                id_most_sp = id;
            end
        end
    id = id + 1;
    end

    -- Default choice if everything failed.
    if (id_most_sp == -1) then
        id_most_sp = 0;
    end

    return battle_instance:GetCharacterActor(id_most_sp);
end

function _GetHeroWithLessHP(battle_instance)
    local id = 0;
    local less_hp = 100000;
    local id_less_hp = -1;
    local nb_heroes = battle_instance:GetNumberOfCharacters();
    while id < nb_heroes do
        local hero = battle_instance:GetCharacterActor(id);
        if (hero ~= nil and hero:CanFight() == true) then
            if (hero:GetHitPoints() < less_hp) then
                less_hp = hero:GetHitPoints();
                id_less_hp = id;
            end
        end
    id = id + 1;
    end

    -- Default choice if everything failed.
    if (id_less_hp == -1) then
        id_less_hp = 0;
    end

    return battle_instance:GetCharacterActor(id_less_hp);
end

-- The true harlequin has got the following skills:
-- 21003, -- HP/MP Drain - 0 SP
-- 10004, -- Holy Veil - ATK+M.ATK on all - 64 SP
-- 10011, -- War God - DEF + DEF-M on all - 96 SP
-- 10007, -- Magical poison - 40 SP
-- 21002, -- Dark Wish (Revives an ally) - 10 SP
-- 21004, -- Shield up (Max out physical defence) - 0 SP
-- 21005, -- Shield down (physical defence is back to normal) - 0 SP

-- Instance local vars
local war_god_done = false
local shield_is_up = false

-- The battle_actor parameter is the enemy thinking, useful to exclude itself from other opponents.
function DecideAction(battle_instance, battle_actor)
    local Battle = battle_instance;

    local nb_enemies = Battle:GetNumberOfEnemies()
    local nb_heroes = Battle:GetNumberOfCharacters()

    local index = 0
    local dead_enemies = 0;
    local enemies_have_enough_atk = false;
    while index < nb_enemies do
        local enemy = Battle:GetEnemyActor(index);
        if (enemy == nil) then
            break;
        end

        -- test whether the enemies have got an active attack status effect.
        if (enemy:GetActiveStatusEffectIntensity(vt_global.GameGlobal.GLOBAL_STATUS_PHYS_ATK)
                >= vt_global.GameGlobal.GLOBAL_INTENSITY_POS_LESSER) then
            enemies_have_enough_atk = true;
        end

        if (enemy:IsAlive() == false) then
            dead_enemies = dead_enemies + 1;
        end

        index = index + 1;
    end

    if (dead_enemies >= 5 and shield_is_up == true) then
        -- All other enemies are dead, the shield is down
        battle_actor:SetAction(21005, enemy)
        shield_is_up = false
        return
    end
    if (dead_enemies == 0 and shield_is_up == false) then
        -- All other enemies are dead, the shield is down
        battle_actor:SetAction(21004, enemy)
        shield_is_up = true
        return
    end

    -- If at least 3 enemies are dead and Harlequin has got enough points, let's revive it
    if (dead_enemies >= 3 and battle_actor:GetSkillPoints() >= 10) then
        battle_actor:SetAction(21002, enemy); -- Dark Wish to revive the enemy ally.
        return;
    end

    if (war_god_done == false and battle_actor:GetSkillPoints() >= 96) then
        battle_actor:SetAction(10011); -- War God (def - on all heroes)
        war_god_done = true;
        return;
    elseif (battle_actor:GetSkillPoints() >= 64 and enemies_have_enough_atk == false) then
        battle_actor:SetAction(10004); -- Holy Veil (atk + on all enemies)
        return;
    elseif (battle_actor:GetSkillPoints() >= 40) then
        -- Magical Poison on the hero with most SP
        battle_actor:SetAction(10007, _GetHeroWithMostSP(Battle));
        return;
    end

    -- Default attack on the weakest hero (with lowest HP)
    battle_actor:SetAction(21003, _GetHeroWithLessHP(Battle));

end
