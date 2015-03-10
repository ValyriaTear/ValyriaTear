local ns = {}
setmetatable(ns, {__index = _G})
dorver_ai = ns;
setfenv(1, ns);

-- Dorver, a big beast all in strength, but weakening its defence on purpose:
-- Skills: (100 SP)
-- 1008, -- Attack (0 SP)
-- 1009, -- Attack all (1 SP)
-- 1013, -- Frenzy (atk & agility +, but def -) (10 SP)


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

-- The battle_actor parameter is the enemy thinking, useful to exclude itself from other actors.
function DecideAction(battle_instance, battle_actor)

    -- Get actors
    local Dorver = battle_actor;
    local Target = _GetHeroWithLessHP(battle_instance)

    -- Test the current Shield spell strength
    if (Dorver:GetHitPoints() <= (Dorver:GetMaxHitPoints() / 2.0)
            and Dorver:GetSkillPoints() >= 10 
            and Dorver:GetActiveStatusEffectIntensity(vt_global.GameGlobal.GLOBAL_STATUS_AGILITY)
            < vt_global.GameGlobal.GLOBAL_INTENSITY_POS_GREATER) then

        -- Improve status
        Dorver:SetAction(1013, Dorver);
    elseif (Dorver:GetSkillPoints() >= 1) then
        -- Triggers an attack on all
        Dorver:SetAction(1009, Target);
    else
        -- Triggers a default attack
        Dorver:SetAction(1008, Target);
    end

end
