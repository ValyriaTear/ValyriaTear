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

local fire1 = {};
local fire1_1 = {};
local fire1_2 = {};

local soldier = {};
local lilly = {};
local herth_walking = nil; -- starting value

local lilly_charge_time = 0;
local lilly_reaction_time = 2000;

local herth_x_position = 0.0;
local bronann = {};

function Initialize(battle_instance)
    Battle = battle_instance;
    Script = Battle:GetScriptSupervisor();

    fire1 = Script:CreateAnimation("img/sprites/map/objects/campfire1.lua");
    fire1:SetDimensions(128.0, 128.0);
    fire1_1 = Script:CreateAnimation("img/misc/lights/torch_light_mask2.lua");
    fire1_1:SetDimensions(340.0, 340.0);
    fire1_2 = Script:CreateImage("img/misc/lights/sun_flare_light.png");
    fire1_2:SetDimensions(154.0, 161.0);

    soldier = Script:CreateAnimation("dat/maps/layna_village/battle_with_banesore/battle_dark_soldier_idle_down.lua");
    soldier:SetDimensions(150.0, 150.0);
    lilly = Script:CreateAnimation("dat/maps/layna_village/battle_with_banesore/battle_lilly_idle_down.lua");
    lilly:SetDimensions(70.0, 140.0);

    local text = {};
    local dialogue = {};

    DialogueManager = Battle:GetDialogueSupervisor();

    -- Add all speakers for the dialogues to be added
    DialogueManager:AddSpeaker("Bronann", vt_system.Translate("Bronann"), "img/portraits/bronann.png");
    DialogueManager:AddSpeaker("Banesore", vt_system.Translate("Banesore"), "img/portraits/npcs/lord_banesore.png");
    DialogueManager:AddSpeaker("Herth", vt_system.Translate("Herth"), "img/portraits/npcs/herth.png");
    DialogueManager:AddSpeaker("Lilly", vt_system.Translate("Lilly"), ""); -- 1003

    dialogue = vt_common.Dialogue("Battle intro dialogue");
    text = vt_system.Translate("So... You think you can hurt me?");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("You? A child with an old wooden stick?");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("...");
    dialogue:AddLine(text, "Bronann");
    DialogueManager:AddDialogue(dialogue);

    dialogue = vt_common.Dialogue("'Stronger than I thought' dialogue");
    text = vt_system.Translate("You're stronger than I thought... but still ... You are too weak to challenge me!");
    dialogue:AddLine(text, "Banesore");
    DialogueManager:AddDialogue(dialogue);

    dialogue = vt_common.Dialogue("Herth appears");
    text = vt_system.Translate("Argh! It hurts!");
    dialogue:AddLine(text, "Bronann");
    text = vt_system.Translate("Yes! I can feel it coming closer to the surface!");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("Let your fear grow, young boy!");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("Bronann, don't listen to him!");
    dialogue:AddLine(text, "Herth");
    DialogueManager:AddDialogue(dialogue);

    dialogue = vt_common.Dialogue("Battle ending dialogue");
    text = vt_system.Translate("I will... try.");
    dialogue:AddLine(text, "Bronann");
    text = vt_system.Translate("Hmph. Soldiers, leave that man to me!");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("So... It appears that you remember me, Banesore. Is that correct...?");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("I do remember you, old man...");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("Then, you already know that your zombified soldiers are of no match to me...");
    dialogue:AddLine(text, "Herth");
    text = vt_system.Translate("Argh! My chest is burning!!");
    dialogue:AddLine(text, "Bronann");
    DialogueManager:AddDialogue(dialogue);

    -- Lilly helps Bronann
    dialogue = vt_common.Dialogue("Lilly helps Bronann dialogue");
    text = vt_system.Translate("Hold on Bronann!");
    dialogue:AddLine(text, "Lilly");
    DialogueManager:AddDialogue(dialogue);

    -- Construct a timer so we can start the dialogue a couple seconds after the battle begins
    start_timer = vt_system.SystemTimer(100, 0);

    battle_time = 0;
    dialogue1_done = false;
    dialogue2_done = false;
    dialogue3_done = false;
    dialogue4_done = false;
    battle_exit_done = false;

    -- Add a charge time at the end of which Lilly can help Bronann
    lilly_charge_time = 10000;
    -- Time Lilly waits before actually healing Bronann
    -- (to avoid triggering the Heal in the middle of Banesore's attack)
    lilly_reaction_time = 2000;

    -- Set Herth's starting x position
    herth_x_position = 0.0;

    -- Get Bronann actor
    bronann = Battle:GetCharacterActor(0);
end

function Restart()
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

    start_timer:Reset();
end


function Update()

    start_timer:Update();

    -- get time expired
    local time_expired = SystemManager:GetUpdateTime();

    -- Update the animations
    if (herth_walking ~= nil) then
        herth_walking:Update(time_expired);
    end
    lilly:Update(time_expired);
    soldier:Update(time_expired);
    fire1:Update(time_expired);
    fire1_1:Update(time_expired);

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
        if (herth_walking == nil) then
            -- Load and start Herth animation only after the dialogue 3.
            herth_walking = Script:CreateAnimation("dat/maps/layna_village/battle_with_banesore/herth_walking.lua");
            herth_walking:SetDimensions(70.0, 140.0);
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
        bronann:ChangeSpriteAnimation("poor");
    end

    -- Only update the expired dialogue time when the battle isn't in scene mode
    -- and the actors aren't paused...
    if (Battle:AreActorStatesPaused() == false) then
        battle_time = battle_time + time_expired;

        if (lilly_charge_time > 0) then
           lilly_charge_time = lilly_charge_time - time_expired;
        elseif (lilly_reaction_time > 0 and bronann:GetHitPoints() <= 50) then
            lilly_reaction_time = lilly_reaction_time - time_expired;
        end
    end

    -- Lilly discretly helps Bronann:
    -- - When Bronann's HP are low and at least a few seconds have passed
    -- - And the dialogue with Herth isn't done.
    if (dialogue3_done == false and lilly_reaction_time <= 0) then
        DialogueManager:BeginDialogue("Lilly helps Bronann dialogue");
        Battle:SetSceneMode(true);
        lilly_charge_time = 10000;
        lilly_reaction_time = 2000;
        lilly_heals_bronann();
    end

    if (dialogue1_done == false) then
        DialogueManager:BeginDialogue("Battle intro dialogue");
        Battle:SetSceneMode(true);
        dialogue1_done = true;
    end

    if (battle_time >= 30000 and dialogue2_done == false) then
        DialogueManager:BeginDialogue("'Stronger than I thought' dialogue");
        Battle:SetSceneMode(true);
        dialogue2_done = true;
    end

    -- Make Herth appear and help Bronann to flee
    if (battle_time >= 60000 and dialogue3_done == false) then
        DialogueManager:BeginDialogue("Herth appears");
        Battle:SetSceneMode(true);
        dialogue3_done = true;
        bronann:ChangeSpriteAnimation("poor");
    end

    -- Once Herth is in place, let's trigger the last dialogue
    if (dialogue3_done == true and herth_x_position >= 175.0) then
        if (dialogue4_done == false) then
            DialogueManager:BeginDialogue("Battle ending dialogue");
            Battle:SetSceneMode(true);
            dialogue4_done = true;
        end
    end

end

local light_mask1_color = vt_video.Color(0.85, 0.32, 0.0, 0.7);
local light_mask2_color = vt_video.Color(0.99, 1.0, 0.27, 0.5);
local white_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawBackground()
    Script:SetDrawFlag(vt_video.GameVideo.VIDEO_BLEND);

    VideoManager:Move(130.0, 280.0);
    soldier:Draw(white_color);
    VideoManager:Move(75.0, 250.0);
    lilly:Draw(white_color);

    VideoManager:Move(300.0, 290.0);
    soldier:Draw(white_color);
    VideoManager:Move(495.0, 330.0);
    soldier:Draw(white_color);

    VideoManager:Move(795.0, 250.0);
    soldier:Draw(white_color);

    VideoManager:Move(235.0, 340.0);
    fire1:Draw(white_color);

    Script:SetDrawFlag(vt_video.GameVideo.VIDEO_BLEND_ADD);
    VideoManager:Move(115.0, 270.0);
    fire1_1:Draw(light_mask1_color);
    VideoManager:Move(220.0, 350.0);
    fire1_2:Draw(light_mask2_color);
end

function DrawForeground()
    if (dialogue3_done == true and herth_walking ~= nil) then
        VideoManager:Move(herth_x_position, 450.0);
        herth_walking:Draw(white_color);
    end
end

function lilly_heals_bronann()
    local hit_points = (20 * 3) +  vt_utils.RandomBoundedInteger(0, 15);
    bronann:RegisterHealing(hit_points, true);
    AudioManager:PlaySound("snd/heal_spell.wav");
    Battle:TriggerBattleParticleEffect("dat/effects/particles/heal_particle.lua",
            bronann:GetXLocation(), bronann:GetYLocation() + 5);
end
