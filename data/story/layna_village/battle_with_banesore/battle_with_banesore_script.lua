local ns = {}
setmetatable(ns, {__index = _G})
battle_with_banesore_script = ns;
setfenv(1, ns);

local Battle = nil
local Script = nil
local DialogueManager = nil
local start_timer = nil

-- The overall time of active battle.
local battle_time = 0

local dialogue1_done = false;
local dialogue2_done = false;
local dialogue3_done = false;
local dialogue4_done = false;
local battle_exit_done = false;

local fire1 = nil
local fire1_1 = nil
local fire1_2 = nil

local soldier = nil
local lilly = nil
local herth_walking = nil; -- starting value

local lilly_charge_time = 0;
local lilly_reaction_time = 2000;

local herth_x_position = 0.0;
local bronann = nil

function Initialize(battle_instance)
    Battle = battle_instance;
    Script = Battle:GetScriptSupervisor();

    fire1 = Script:CreateAnimation("data/entities/map/objects/campfire1.lua");
    fire1:SetDimensions(128.0, 128.0);
    fire1_1 = Script:CreateAnimation("data/visuals/lights/torch_light_mask2.lua");
    fire1_1:SetDimensions(340.0, 340.0);
    fire1_2 = Script:CreateImage("data/visuals/lights/sun_flare_light.png");
    fire1_2:SetDimensions(154.0, 161.0);

    soldier = Script:CreateAnimation("data/story/layna_village/battle_with_banesore/battle_dark_soldier_idle_down.lua");
    soldier:SetDimensions(150.0, 150.0);
    lilly = Script:CreateAnimation("data/story/layna_village/battle_with_banesore/battle_lilly_idle_down.lua");
    lilly:SetDimensions(70.0, 140.0);

    local text = nil
    local dialogue = nil

    DialogueManager = Battle:GetDialogueSupervisor();

    -- Add all speakers for the dialogues to be added
    DialogueManager:AddSpeaker("Bronann", vt_system.Translate("Bronann"), "data/entities/portraits/bronann.png");
    DialogueManager:AddSpeaker("Banesore", vt_system.Translate("Banesore"), "data/entities/portraits/npcs/lord_banesore.png");
    DialogueManager:AddSpeaker("Herth", vt_system.Translate("Herth"), "data/entities/portraits/npcs/herth.png");
    DialogueManager:AddSpeaker("Lilly", vt_system.Translate("Lilly"), "");

    dialogue = vt_common.Dialogue.Create(DialogueManager, "Battle intro dialogue");
    text = vt_system.Translate("So, you think you can wound me?");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("You? A child with an old wooden stick?");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("...");
    dialogue:AddLine(text, "Bronann");

    dialogue = vt_common.Dialogue.Create(DialogueManager, "'Stronger than I thought' dialogue");
    text = vt_system.Translate("You're stronger than I thought. But you are still too weak to challenge me!");
    dialogue:AddLine(text, "Banesore");

    dialogue = vt_common.Dialogue.Create(DialogueManager, "Herth appears");
    text = vt_system.Translate("Argh! It hurts!");
    dialogue:AddLine(text, "Bronann");
    text = vt_system.Translate("Yes! I can feel it coming closer to the surface!");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("Let your fear grow, young boy!");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("Bronann, don't listen to him!");
    dialogue:AddLine(text, "Herth");

    dialogue = vt_common.Dialogue.Create(DialogueManager, "Battle ending dialogue");
    text = vt_system.Translate("I will... try.");
    dialogue:AddLine(text, "Bronann");
    text = vt_system.Translate("Hmph. Soldiers, leave that man to me!");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("So, it appears that you remember me, Banesore. Is that correct?");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("I do remember you, old man...");
    dialogue:AddLine(text, "Banesore");
    text = vt_system.Translate("Then, you already know that your zombified soldiers are no match for me.");
    dialogue:AddLine(text, "Herth");
    text = vt_system.Translate("Argh! My chest is burning!");
    dialogue:AddLine(text, "Bronann");

    -- Lilly helps Bronann
    dialogue = vt_common.Dialogue.Create(DialogueManager, "Lilly helps Bronann dialogue");
    text = vt_system.Translate("Hold on Bronann!");
    dialogue:AddLine(text, "Lilly");

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
            herth_walking = Script:CreateAnimation("data/story/layna_village/battle_with_banesore/herth_walking.lua");
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
        DialogueManager:StartDialogue("Lilly helps Bronann dialogue");
        Battle:SetSceneMode(true);
        -- Set the time depending on the game difficulty
        if (SystemManager:GetGameDifficulty() == 1) then
            lilly_charge_time = 5000; -- Easy
        else
            lilly_charge_time = 10000;
        end
        lilly_reaction_time = 2000;
        lilly_heals_bronann();
    end

    if (dialogue1_done == false) then
        DialogueManager:StartDialogue("Battle intro dialogue");
        Battle:SetSceneMode(true);
        dialogue1_done = true;
    end

    if (battle_time >= 30000 and dialogue2_done == false) then
        DialogueManager:StartDialogue("'Stronger than I thought' dialogue");
        Battle:SetSceneMode(true);
        dialogue2_done = true;
    end

    -- Make Herth appear and help Bronann to flee
    if (battle_time >= 60000 and dialogue3_done == false) then
        DialogueManager:StartDialogue("Herth appears");
        Battle:SetSceneMode(true);
        dialogue3_done = true;
        bronann:ChangeSpriteAnimation("poor");
    end

    -- Once Herth is in place, let's trigger the last dialogue
    if (dialogue3_done == true and herth_x_position >= 175.0) then
        if (dialogue4_done == false) then
            DialogueManager:StartDialogue("Battle ending dialogue");
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
    local hit_points = 60

    if (SystemManager:GetGameDifficulty() == 1) then
        hit_points = hit_points + vt_utils.RandomBoundedInteger(50, 80); -- Easy
    else
        hit_points = hit_points + vt_utils.RandomBoundedInteger(0, 15);
    end

    bronann:RegisterHealing(hit_points, true);
    AudioManager:PlaySound("data/sounds/heal_spell.wav");
    Battle:TriggerBattleParticleEffect("data/visuals/particle_effects/heal_particle.lua",
            bronann:GetXLocation(), bronann:GetYLocation() + 5);
end
