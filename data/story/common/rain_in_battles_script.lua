local ns = {}
setmetatable(ns, {__index = _G})
rain_in_battles_script = ns;
setfenv(1, ns);

local Battle = nil
local Script = nil

local ripples = nil

function Initialize(battle_instance)
    Battle = battle_instance;
    Script = Battle:GetScriptSupervisor();

    -- Make the rain starts if needed
    Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/rain.lua", 512.0, 768.0);
    ripples = Script:CreateAnimation("data/visuals/particle_effects/ripples.lua");
    ripples:SetDimensions(256.0, 153.6); -- 256.0 * 0.6
end

--function Restart()

--end

function Update()
    ripples:Update();
end

local white_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawBackground()
    if (ripples ~= nil) then
        VideoManager:Move(235.0, 340.0);
        ripples:Draw(white_color);
        VideoManager:Move(235.0, 596.0);
        ripples:Draw(white_color);
        VideoManager:Move(491.0, 340.0);
        ripples:Draw(white_color);
        VideoManager:Move(491.0, 596.0);
        ripples:Draw(white_color);
    end
end

--function DrawPostEffects()

--end
