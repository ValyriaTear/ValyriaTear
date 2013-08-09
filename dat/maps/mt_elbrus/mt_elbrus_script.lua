local ns = {}
setmetatable(ns, {__index = _G})
mt_elbrus_script = ns;
setfenv(1, ns);

local Battle = {};
local Script = {};

function Initialize(battle_instance)
    Battle = battle_instance;
    Script = Battle:GetScriptSupervisor();

    -- Make the rain starts if needed
    if (GlobalManager:GetEventValue("story", "mt_elbrus_weather_level") > 0) then
        Battle:TriggerBattleParticleEffect("dat/effects/particles/rain.lua", 512.0, 768.0);
    end

end

--function Restart()

--end


--function Update()

--end

--function DrawPostEffects()

--end
