local ns = {}
setmetatable(ns, {__index = _G})
soft_lightnings_script = ns;
setfenv(1, ns);

local Battle = {};
local Script = {};

local lightning = {};
local total_time = 0;
local lightning_sound_triggered = false;
local lightning_alpha = 0.3;

function Initialize(battle_instance)
    Battle = battle_instance;
    Script = Battle:GetScriptSupervisor();

    lightning = Script:CreateImage("");
    lightning:SetDimensions(1024.0, 768.0);

    AudioManager:LoadSound("snd/thunder.wav", Map);

    total_time = 0;
    lightning_sound_triggered = false;

    lightning_alpha = 0.3;

end


function Update()
    local time_expired = SystemManager:GetUpdateTime();

    if (total_time > 25000) then
        total_time = 0;
        lightning_sound_triggered = false;
    end;

    total_time = total_time + time_expired;

    -- First the distant light of a lightning
    if (total_time > 3000 and total_time < 4500) then
        lightning_alpha = 0.4 - 0.4 * (total_time - 3000) / (4500 - 3000);
    else
        lightning_alpha = 0.0;
    end

    -- Then its sound comes
    if (lightning_sound_triggered == false and total_time > 9000) then
        AudioManager:PlaySound("snd/thunder.wav");
        lightning_sound_triggered = true
    end

end

local lightning_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawForeground()
    if (lightning_alpha <= 0.0) then
        return
    end

    lightning_color:SetAlpha(lightning_alpha);
    VideoManager:Move(0.0, 0.0);
    lightning:Draw(lightning_color);
end
