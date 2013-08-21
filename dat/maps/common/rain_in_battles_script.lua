local ns = {}
setmetatable(ns, {__index = _G})
rain_in_battles_script = ns;
setfenv(1, ns);

local Battle = {};
local Script = {};

local ripples_id = -1;

function Initialize(battle_instance)
    Battle = battle_instance;
    Script = Battle:GetScriptSupervisor();

    -- Make the rain starts if needed
    Battle:TriggerBattleParticleEffect("dat/effects/particles/rain.lua", 512.0, 768.0);
    ripples_id = Script:AddAnimation("img/effects/ripples.lua", 256.0, 153.6); -- 256.0 * 0.6
end

--function Restart()

--end


--function Update()

--end

function DrawBackground()
    if (ripples_id > -1) then
        Script:DrawAnimation(ripples_id, 235.0, 340.0);
        Script:DrawAnimation(ripples_id, 235.0, 596.0);
        Script:DrawAnimation(ripples_id, 491.0, 340.0);
        Script:DrawAnimation(ripples_id, 491.0, 596.0);
    end

end

--function DrawPostEffects()

--end
