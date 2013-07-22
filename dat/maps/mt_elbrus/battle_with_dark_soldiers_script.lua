local ns = {}
setmetatable(ns, {__index = _G})
battle_with_dark_soldiers_script = ns;
setfenv(1, ns);

local Battle = {};
local Script = {};
local DialogueManager = {};
local start_timer = {};

local dialogue1_done = false;

local time_left_header_text = {};
local time_left_text = {};

local header_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
-- Makes the time glow when low
local pulse_green_blue = 0.4;
local pulse_up = false;

function Initialize(battle_instance)
    Battle = battle_instance;
    Script = Battle:GetScriptSupervisor();

    local text = {};
    local dialogue = {};

    DialogueManager = Battle:GetDialogueSupervisor();

    -- Add all speakers for the dialogues to be added
    -- TODO: all of these custom speaker calls should be replaced with calls to AddCharacterSpeaker() later
    DialogueManager:AddCustomSpeaker(1000, vt_system.Translate("Kalya"), "img/portraits/kalya.png");
    DialogueManager:AddCustomSpeaker(1001, vt_system.Translate("Soldier"), "img/portraits/npcs/dark_soldier.png");

    dialogue = vt_battle.BattleDialogue(1);
    text = vt_system.Translate("You're spotted!! My comrades shall be here in no time. Surrender now or you'll be punished!!");
    dialogue:AddLine(text, 1001);
    text = vt_system.Translate("Don't listen to him! We must get rid of him before reinforcement gets along or we're doomed!!");
    dialogue:AddLine(text, 1000);
    DialogueManager:AddDialogue(dialogue);

    -- Construct a timer so we can start the dialogue a couple seconds after the battle begins
    start_timer = vt_system.SystemTimer(100, 0);

    -- 2 minutes before spawning more soldiers...
    timer_before_soldiers = vt_system.SystemTimer(120000, 0);
    timer_before_soldiers:EnableManualUpdate();
    time_left_text = "2:00";
    time_left_header_text = vt_system.Translate("Time left:");

    battle_time = 0;
    dialogue1_done = false;
end

function Restart()
    battle_time = 0;
    dialogue1_done = false;

    start_timer:Reset();

    timer_before_soldiers:Reset();
    pulse_green_blue = 0.4;
    pulse_up = false;
    text_color:SetColor(1.0, 1.0, 1.0, 1.0);
end


function Update()

    start_timer:Update();

    -- get time expired
    local time_expired = SystemManager:GetUpdateTime();

    -- Wait until the initial battle sequence ends to begin running the dialogue start timer
    if ((start_timer:IsInitial() == true) and (Battle:GetState() ~= vt_battle.BattleMode.BATTLE_STATE_INITIAL)) then
        start_timer:Run();
    end

    -- If battle is won or lost, don't update the script
    if (Battle:GetState() == vt_battle.BattleMode.BATTLE_STATE_VICTORY
        or Battle:GetState() == vt_battle.BattleMode.BATTLE_STATE_DEFEAT
        or Battle:GetState() == vt_battle.BattleMode.BATTLE_STATE_EXITING) then
        return;
    end

    -- Wait for the battle to start
    if (start_timer:IsFinished() == false) then
        return;
    elseif (timer_before_soldiers:IsRunning() == false) then
        timer_before_soldiers:Run();
    end

    -- If the dialogue has not been seen yet, check if its time to start it
    if (DialogueManager:IsDialogueActive() == true) then
        return;
    end

    Battle:SetSceneMode(false);

    if (dialogue1_done == false) then
        DialogueManager:BeginDialogue(1);
        Battle:SetSceneMode(true);
        dialogue1_done = true;
    end

    -- Update the timer text
    _UpdateTimer();

end

function _UpdateTimer()
    -- The timer is manually updated
    timer_before_soldiers:Update();

    -- Compute the text
    local time_left = timer_before_soldiers:TimeLeft();
    local minutes_text = math.floor(time_left / 60000);
    local seconds_text = math.fmod(time_left, 60000);
    seconds_text = math.floor(seconds_text / 1000); -- Keep only the two first digits.

    if (seconds_text < 10) then
        time_left_text = minutes_text .. ":0" .. seconds_text;
    else
        time_left_text = minutes_text .. ":" .. seconds_text;
    end

    -- Update the color according to the time left
    if (minutes_text > 0) then
        return;
    end
    -- Becomes yellow between :59 and :31
    text_color:SetColor(1.0, 1.0, 0.4, 1.0);
    if (minutes_text > 0 or seconds_text > 30) then
        return;
    end

    -- Becomes red between :30 and :16
    text_color:SetColor(1.0, 0.4, 0.4, 1.0);
    if (minutes_text > 0 or seconds_text > 15) then
        return;
    end

    -- Red pulse until 0:00
    local time_expired = SystemManager:GetUpdateTime();

    if (pulse_up == true) then
        pulse_green_blue = pulse_green_blue + 0.0025 * time_expired;
    elseif (pulse_up == false) then
        pulse_green_blue = pulse_green_blue - 0.0025 * time_expired;
    end

    if (pulse_green_blue >= 0.4) then
        pulse_up = false;
    elseif (pulse_green_blue <= 0.0) then
        pulse_up = true;
    end

    text_color:SetColor(1.0, pulse_green_blue, pulse_green_blue, 1.0);

    if (minutes_text > 0 or seconds_text > 0) then
        return;
    end

    -- Reset timer in the end, but add two soldiers:
    Battle:AddEnemy(9);
    Battle:AddEnemy(9);
    timer_before_soldiers:Reset();
    timer_before_soldiers:Run();
end

function DrawPostEffects()
    if (dialogue1_done == true) then
        -- Draw timer
        VideoManager:DrawText(time_left_header_text, 80.0, 80.0, header_color);
        VideoManager:DrawText(time_left_text, 100.0, 100.0, text_color);
    end
end
