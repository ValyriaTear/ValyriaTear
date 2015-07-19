local ns = {}
setmetatable(ns, {__index = _G})
battles_in_trap_map_script = ns;
setfenv(1, ns);

local Battle = nil
local Script = nil

function Initialize(battle_instance)
    Battle = battle_instance;
    Script = Battle:GetScriptSupervisor();
end

function Reset()
    Battle:GetEffectSupervisor():ShakeScreen(0.4, 0, vt_mode_manager.EffectSupervisor.SHAKE_FALLOFF_NONE); -- 0 means infinite time.
end

function Restart()
    Reset();
end

function Update()
    -- nothing
end
