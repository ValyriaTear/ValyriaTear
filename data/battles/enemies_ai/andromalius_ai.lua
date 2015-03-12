local ns = {}
setmetatable(ns, {__index = _G})
andromalius_ai = ns;
setfenv(1, ns);

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


-- Skills
-- 1014,  -- Shake - (atk + AGI -) - 0 SP
-- 10100, -- Fire - 7 SP
-- 10007, -- Magical poison - 40 SP
-- 21002, -- Dark Wish (Revives an ally) - 10 SP

-- The battle_actor parameter is the enemy thinking, useful to exclude itself from other opponents.
function DecideAction(battle_instance, battle_actor)
    local Battle = battle_instance;

    local nb_enemies = Battle:GetNumberOfEnemies();
    local nb_heroes = Battle:GetNumberOfCharacters();

    local index = 0
    local dead_enemies = false;
    while index < nb_enemies do
        local enemy = Battle:GetEnemyActor(index);
        if (enemy == nil) then
            break;
        end

        if (enemy:IsAlive() == false) then
            dead_enemies = true;

            -- Let's revive Dorver
            if (battle_actor:GetSkillPoints() >= 10) then
                battle_actor:SetAction(21002, enemy); -- Dark Wish to revive the enemy ally.
                return;
            end
        end

        index = index + 1;
    end

    if (math.random(0, 100) > 50 and battle_actor:GetSkillPoints() >= 40) then
        -- Magical Poison on the hero with most SP
        battle_actor:SetAction(10007, _GetHeroWithMostSP(Battle));
        return;
    elseif (battle_actor:GetSkillPoints() >= 7) then
        -- Fire on the hero with less HP
        battle_actor:SetAction(10100, _GetHeroWithLessHP(Battle));
        return;
    end

    -- Default attack - Shake
    battle_actor:SetAction(1014, _GetHeroWithLessHP(Battle));

end
