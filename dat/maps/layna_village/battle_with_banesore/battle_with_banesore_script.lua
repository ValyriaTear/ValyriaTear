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
local dialogue4_done = false;
local battle_exit_done = false;

local fire1_id = -1;
local fire1_1_id = -1;
local fire1_2_id = -1;

local soldier_id = -1;
local lilly_id = -1;
local herth_walking_id = -1;

local lilly_charge_time = 0;

local herth_x_position = 0.0;
local bronann = {};

function Initialize(battle_instance)
    Battle = battle_instance;
    Script = Battle:GetScriptSupervisor();

    fire1_id = Script:AddAnimation("img/sprites/map/objects/campfire.lua", 128.0, 128.0);
    fire1_1_id = Script:AddAnimation("img/misc/lights/torch_light_mask2.lua", 340.0, 340.0);
    fire1_2_id = Script:AddImage("img/misc/lights/sun_flare_light.png", 154.0, 161.0);

    soldier_id = Script:AddAnimation("dat/maps/layna_village/battle_with_banesore/battle_dark_soldier_idle_down.lua", 150.0, 150.0);
    lilly_id = Script:AddAnimation("dat/maps/layna_village/battle_with_banesore/battle_lilly_idle_down.lua", 70.0, 140.0);

    local text = {};
    local dialogue = {};

    DialogueManager = Battle:GetDialogueSupervisor();

    -- Add all speakers for the dialogues to be added
    -- TODO: all of these custom speaker calls should be replaced with calls to AddCharacterSpeaker() later
    DialogueManager:AddCustomSpeaker(1000, vt_system.Translate("Bronann"), "img/portraits/bronann.png");
    DialogueManager:AddCustomSpeaker(1001, vt_system.Translate("Banesore"), "img/portraits/npcs/lord_banesore.png");
    DialogueManager:AddCustomSpeaker(1002, vt_system.Translate("Herth"), "");
    DialogueManager:AddCustomSpeaker(1003, vt_system.Translate("Lilly"), "");

    dialogue = vt_battle.BattleDialogue(1);
    text = vt_system.Translate("So... Do you think you can hurt me?");
    dialogue:AddLine(text, 1001);
    text = vt_system.Translate("...");
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

    dialogue = vt_battle.BattleDialogue(4);
    text = vt_system.Translate("Do you remind me, Banesore?");
    dialogue:AddLine(text, 1002);
    text = vt_system.Translate("I do, old man...");
    dialogue:AddLine(text, 1001);
    text = vt_system.Translate("Then, you know your zombified men are no match to me...");
    dialogue:AddLine(text, 1002);
    text = vt_system.Translate("Argh! My chest is burning!!");
    dialogue:AddLine(text, 1000);
    DialogueManager:AddDialogue(dialogue);

    -- Lilly helps Bronann
    dialogue = vt_battle.BattleDialogue(5);
    text = vt_system.Translate("Hold on Bronann!");
    dialogue:AddLine(text, 1003);
    DialogueManager:AddDialogue(dialogue);

    -- Construct a timer so we can start the dialogue a couple seconds after the battle begins
    start_timer = vt_system.SystemTimer(100, 0);

    battle_time = 0;
    dialogue1_done = false;
    dialogue2_done = false;
    dialogue3_done = false;
    dialogue4_done = false;
    battle_exit_done = false;

    -- Add a charge time at the end of which lilly can help Bronann
    lilly_charge_time = 10000;

    -- Set Herth's starting x position
    herth_x_position = 0.0;
    
    -- Get Bronann actor
    bronann = Battle:GetCharacterActor(0);
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

    -- Update herth position until he is in place
    if (dialogue3_done == true and DialogueManager:IsDialogueActive() == false) then
        if (herth_walking_id == -1) then
            -- Load and start Herth animation only after the dialogue 3.
            herth_walking_id = Script:AddAnimation("dat/maps/layna_village/battle_with_banesore/herth_walking.lua", 70.0, 140.0);
        end
        if (herth_x_position <= 175.0) then
            herth_x_position = herth_x_position + time_expired * 0.7;
        end
    end

    -- If the dialogue has not been seen yet, check if its time to start it
    if (DialogueManager:IsDialogueActive() == true) then
        return;
    end

    -- Don't play a dialogue when bronann is acting
    if (bronann:GetState() ~= vt_battle.BattleMode.ACTOR_STATE_IDLE and bronann:GetState() ~= vt_battle.BattleMode.ACTOR_STATE_COMMAND) then
        return;
    end

    -- Stay in scene mode once the battle is over
    if (battle_exit_done == true) then
        return;
    end

    Battle:SetSceneMode(false);

    -- Bronann is hurt by the crystal
    if (dialogue4_done == true and battle_exit_done == false) then
        Battle:SetSceneMode(true);
        ModeManager:Pop(true, true);
        battle_exit_done = true;
        --Change Bronann animation
        bronann:ChangeSpriteAnimation("hurt");
    end

    -- Only update the expired dialogue time when the battle isn't in scene mode
    -- and the actors aren't paused...
    if (Battle:AreActorStatesPaused() == false) then
        battle_time = battle_time + time_expired;
        if (lilly_charge_time > 0) then
           lilly_charge_time = lilly_charge_time - time_expired;
        end
    end

    -- Lilly discretly helps Bronann:
    -- - When Bronann's HP are low and at least a few seconds have passed
    -- - And the dialogue with Herth isn't done.
    if (dialogue3_done == false and lilly_charge_time <= 0) then
        if (bronann:GetHitPoints() <= 25) then
            DialogueManager:BeginDialogue(5);
            Battle:SetSceneMode(true);
            lilly_charge_time = 20000;
            lilly_heals_bronann();
        end
    end

    if (dialogue1_done == false) then
        DialogueManager:BeginDialogue(1);
        Battle:SetSceneMode(true);
        dialogue1_done = true;
    end

    if (battle_time >= 30000 and dialogue2_done == false) then
        DialogueManager:BeginDialogue(2);
        Battle:SetSceneMode(true);
        dialogue2_done = true;
    end

    -- Make Herth appear and help Bronann to flee
    if (battle_time >= 60000 and dialogue3_done == false) then
        DialogueManager:BeginDialogue(3);
        Battle:SetSceneMode(true);
        dialogue3_done = true;
    end

    -- Once Herth is in place, let's trigger the last dialogue
    if (dialogue3_done == true and herth_x_position >= 175.0) then
        if (dialogue4_done == false) then
            DialogueManager:BeginDialogue(4);
            Battle:SetSceneMode(true);
            dialogue4_done = true;
        end
    end

end



function DrawBackground()
    Script:SetDrawFlag(vt_video.GameVideo.VIDEO_BLEND);

    Script:DrawAnimation(soldier_id, 130.0, 280.0);
    Script:DrawAnimation(lilly_id, 75.0, 250.0);

    Script:DrawAnimation(soldier_id, 300.0, 290.0);
    Script:DrawAnimation(soldier_id, 495.0, 330.0);

    Script:DrawAnimation(soldier_id, 795.0, 250.0);

    Script:DrawAnimation(fire1_id, 235.0, 340.0);
    Script:SetDrawFlag(vt_video.GameVideo.VIDEO_BLEND_ADD);
    Script:DrawAnimation(fire1_1_id, 115.0, 270.0, vt_video.Color(0.85, 0.32, 0.0, 0.7));
    Script:DrawImage(fire1_2_id, 220.0, 350.0, vt_video.Color(0.99, 1.0, 0.27, 0.5));
end

function DrawForeground()
    if (dialogue3_done == true) then
        Script:DrawAnimation(herth_walking_id, herth_x_position, 450.0);
    end
end

function lilly_heals_bronann()
    local hit_points = (20 * 3) +  vt_utils.RandomBoundedInteger(0, 15);
    bronann:RegisterHealing(hit_points, true);
    AudioManager:PlaySound("snd/heal_spell.wav");
    Battle:TriggerBattleParticleEffect("dat/effects/particles/heal_particle.lua",
            bronann:GetXLocation(), bronann:GetYLocation() + 5);
end
