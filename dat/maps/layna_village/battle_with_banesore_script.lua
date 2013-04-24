local ns = {}
setmetatable(ns, {__index = _G})
battle_with_banesore_script = ns;
setfenv(1, ns);

local Battle = {};
local Script = {};
local DialogueManager = {};
local start_timer = {};

-- The overall time of active battle.
local battle_time = {};

local dialogue1_done = false;
local dialogue2_done = false;
local dialogue3_done = false;
local battle_exit_done = false;

local fire1_id = -1;

function Initialize(battle_instance)
    Battle = battle_instance;
    Script = Battle:GetScriptSupervisor();

    -- TODO: Add campfire, lilly and soldiers
    fire1_id = Script:AddAnimation("img/sprites/map/objects/campfire.lua", 128.0, 128.0);

    local text = {};
    local dialogue = {};

    DialogueManager = Battle:GetDialogueSupervisor();

    -- Add all speakers for the dialogues to be added
    -- TODO: all of these custom speaker calls should be replaced with calls to AddCharacterSpeaker() later
    DialogueManager:AddCustomSpeaker(1000, "Bronann", "img/portraits/bronann.png");
    DialogueManager:AddCustomSpeaker(1001, "Banesore", "img/portraits/npcs/lord_banesore.png");
    DialogueManager:AddCustomSpeaker(1002, "Herth", "");

    dialogue = vt_battle.BattleDialogue(1);
    text = vt_system.Translate("So... Do you think you can hurt me?");
    dialogue:AddLine(text, 1001);
    text = vt_system.Translate("At least, I can try!");
    dialogue:AddLine(text, 1000);
    DialogueManager:AddDialogue(dialogue);

    dialogue = vt_battle.BattleDialogue(2);
    text = vt_system.Translate("You're stronger than I thought but still ... So weak!");
    dialogue:AddLine(text, 1001);
    DialogueManager:AddDialogue(dialogue);

    dialogue = vt_battle.BattleDialogue(3);
    text = vt_system.Translate("Yes! I can feel it come closer to the surface!");
    dialogue:AddLine(text, 1001);
    text = vt_system.Translate("Let your fear grow, young boy!");
    dialogue:AddLine(text, 1001);
    text = vt_system.Translate("Argh!");
    dialogue:AddLine(text, 1000);
    text = vt_system.Translate("Don't listen to him, Bronann!");
    dialogue:AddLine(text, 1002);
    DialogueManager:AddDialogue(dialogue);

    -- Construct a timer so we can start the dialogue a couple seconds after the battle begins
    start_timer = vt_system.SystemTimer(100, 0);

    battle_time = 0;
    dialogue1_done = false;
    dialogue2_done = false;
    dialogue3_done = false;
    battle_exit_done = false;
end


function Update()

    start_timer:Update();

    -- get time expired
    local time_expired = SystemManager:GetUpdateTime();

    -- Wait until the initial battle sequence ends to begin running the dialogue start timer
    if ((start_timer:IsInitial() == true) and (Battle:GetState() ~= vt_battle.BattleMode.BATTLE_STATE_INITIAL)) then
        start_timer:Run();
    end

    -- Wait for the battle to start
    if (start_timer:IsFinished() == false) then
        return;
    end

    -- If the dialogue has not been seen yet, check if its time to start it
    if (DialogueManager:IsDialogueActive() == true) then
        return;
    end

    -- Only update the expired dialogue time when the battle isn't in scene mode.
    -- TODO:
    -- A BATTLE_SCENE state is needed to handle:
    --  - State pause for actors
    --  - Dialogues updates
    --  - Other script events
    if (Battle:AreActorStatesPaused() == false) then
       battle_time = battle_time + time_expired;
    end

    if (dialogue1_done == false) then
        DialogueManager:BeginDialogue(1);
        dialogue1_done = true;
    end

    if (battle_time >= 3000 and dialogue2_done == false) then
        DialogueManager:BeginDialogue(2);
        dialogue2_done = true;
    end

    if (battle_time >= 6000 and dialogue3_done == false) then
        DialogueManager:BeginDialogue(3);
        dialogue3_done = true;
    end

    if (battle_time >= 8000 and battle_exit_done == false) then
        ModeManager:Pop(true, true);
        battle_exit_done = true;
    end

end

function DrawBackground()
    --TODO
	-- Draw background main animations
	Script:DrawAnimation(fire1_id, 235.0, 310.0);

	--Script:DrawImage(rock_id, 800.0, 248.0, vt_video.Color(1.0, 1.0, 1.0, 1.0));
end

function DrawPostEffects()
    --TODO

end
