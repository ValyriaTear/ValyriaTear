-- This script executes for the very first battle that the player encounters in a new game.
-- Its purpose is to present a dialogue to the player at the start of the battle to provide
-- a brief explanation of the battle system.

local ns = {}
setmetatable(ns, {__index = _G})
tutorial_battle_dialogs = ns;
setfenv(1, ns);

local hand1_image = {};
local hand2_image = {};
-- hand coordinates
local hand1_origin_x = 0.0;
local hand1_origin_y = 0.0;
local hand2_origin_x = 0.0;
local hand2_origin_y = 0.0;
-- bouncing coordinates
local hand1_bouncing_x = 0.0;
local bouncing1_force = 0.0;
local hand2_bouncing_y = 0.0;
local bouncing2_force = 0.0;
-- whether the hand should be shown
local trigger_show_hand1 = false;
local trigger_show_hand2 = false;
-- whether the hand should be visible
local hand1_visible = false;
local hand2_visible = false;
-- Keeps track of the latest line.
local last_line = 0;

local Battle = {};
local Script = {};
local DialogueManager = {};
local stop_script = false;
local start_timer = {};
local main_dialogue = {};

local dialogue_started = false;

function Initialize(battle_instance)
    Battle = battle_instance;
    Script = Battle:GetScriptSupervisor();

    stop_script = false;

    if (GlobalManager:GetEventValue("story", "first_battle") ~= 1) then
        GlobalManager:SetEventValue("story", "first_battle", 1);
        stop_script = false;
    else
        stop_script = true;
        return;
    end

    -- Load the hand cursors
    hand1_image = Script:CreateImage("img/menus/hand.png");
    hand1_image:SetDimensions(47.0, 26.0);
    hand2_image = Script:CreateImage("img/menus/hand_down.png");
    hand2_image:SetDimensions(26.0, 47.0);

    DialogueManager = Battle:GetDialogueSupervisor();

    -- Add all speakers for the dialogues to be added
    DialogueManager:AddCustomSpeaker(1000, vt_system.Translate("Bronann"), "img/portraits/bronann.png");
    DialogueManager:AddCustomSpeaker(1002, vt_system.Translate("Kalya"), "img/portraits/kalya.png");

    local text;
    -- The dialogue constructed below offers the player instructions on how to do battle. It is displayed only once in the first few seconds
    -- of battle, before any action can be taken. The player is presented with several options that they can read to get more information on
    -- the battle system. One of the options that the player may select from will finish the dialogue, allow the battle to resume.
    main_dialogue = vt_battle.BattleDialogue(1);
        text = vt_system.Translate("Woah, I wouldn't have expected monsters so close to the village. Bronann, do you need a quick reminder about the basics of battle?");
        main_dialogue:AddLine(text, 1002);
        text = vt_system.Translate("...");
        main_dialogue:AddLine(text, 1000);
        text = vt_system.Translate("Ask about battle basics.");
        main_dialogue:AddOption(text, 2);
        text = vt_system.Translate("Ask nothing. I know how to fight.");
        main_dialogue:AddOption(text, 13);
        -- [Line 2] After selecting option: Ask about battle basics.
        text = vt_system.Translate("Er, I could use a refresher on the fundamentals of combat.");
        main_dialogue:AddLine(text, 1000);
        text = vt_system.Translate("Here you can find the Hit Points (HP) and Skill Points (SP) of the ally party. Don't let our HP reach 0 or we're doomed!");
        main_dialogue:AddLine(text, 1002);
        text = vt_system.Translate("In battles, our icons and those of our enemies will rise up on the Stamina Bar. The more Agility you have, the faster your icon will climb.");
        main_dialogue:AddLine(text, 1002);
        text = vt_system.Translate("Once an ally has reached the action level, you can select an action for that ally. The battle is then paused and you've got all the time you need to select one, so don't panic.");
        main_dialogue:AddLine(text, 1002);
        text = vt_system.Translate("There are two action types: The basic 'Weapon' to attack, and later you can also use 'Magic' skills. Last but not least, you'll also be able to use our 'Items'. Simply choose an action followed by a target to trigger it.");
        main_dialogue:AddLine(text, 1002);
        text = vt_system.Translate("The icon will still need to climb to the top of the Stamina Bar before the selected action is executed. This is called the 'preparation time'. Generally the more powerful the skill being executed, the longer it will take to prepare.");
        main_dialogue:AddLine(text, 1002);
        text = vt_system.Translate("By the way, you're not forced to wait for your icon to reach the action level before you can select an action. When these two little arrows here are highlighted, you can use your up and down keys to preselect an action for the corresponding ally.");
        main_dialogue:AddLine(text, 1002);
        text = vt_system.Translate("So long as the little arrows there are still visible, you can change the selected action at any time.");
        main_dialogue:AddLine(text, 1002);
        text = vt_system.Translate("Don't forget that you can press '") .. InputManager:GetHelpKeyName() .. vt_system.Translate("' if you need more details.");
        main_dialogue:AddLine(text, 1002);
        text = vt_system.Translate("Shall I repeat all that?");
        main_dialogue:AddLine(text, 1002);
        text = vt_system.Translate("...");
        main_dialogue:AddLine(text, 1000);
        text = vt_system.Translate("Yes, please!");
        main_dialogue:AddOption(text, 3);
        text = vt_system.Translate("No, it's alright!");
        main_dialogue:AddOption(text, 14);

        -- [Line 13] End
        text = vt_system.Translate("Don't worry Kalya, I have not forgotten my training.");
        main_dialogue:AddLine(text, 1000, 15);
        -- [Line 14] After selecting option: Ask nothing. (After a topic has already been asked).
        text = vt_system.Translate("Thanks Kalya, I'm prepared now.");
        main_dialogue:AddLine(text, 1000);
        -- [Line 25] Final line of dialogue
        text = vt_system.Translate("Good. Let us quickly dispatch this minor threat.");
        main_dialogue:AddLine(text, 1002);
    DialogueManager:AddDialogue(main_dialogue);

    -- Construct a timer so we can start the dialogue a couple seconds after the battle begins
    start_timer = vt_system.SystemTimer(100, 0);

    dialogue_started = false;
end


function Update()
    if (stop_script == true) then
        return;
    end

    start_timer:Update();

    -- Wait until the initial battle sequence ends to begin running the dialogue start timer
    if ((start_timer:IsInitial() == true) and (Battle:GetState() ~= vt_battle.BattleMode.BATTLE_STATE_INITIAL)) then
        start_timer:Run();
    end

    -- If the dialogue is done, end the scene
    if ((dialogue_started == true) and (DialogueManager:IsDialogueActive()) == false) then
        Battle:SetSceneMode(false);
        stop_script = true;
    end

    -- If the dialogue has not been seen yet, check if its time to start it
    if ((dialogue_started == false) and (start_timer:IsFinished() == true) and (DialogueManager:IsDialogueActive() == false)) then
        DialogueManager:BeginDialogue(1);
        Battle:SetSceneMode(true);
        dialogue_started = true;
    end

    -- Set up whether the hand should be shown and where
    if (DialogueManager:GetLineCounter() < 3) then
        hand1_visible = false;
        hand2_visible = false;
    elseif (last_line ~= 3 and DialogueManager:GetLineCounter() == 3) then
        hand1_visible = false;
        hand2_visible = true;
        hand2_origin_x = 350.0;
        hand2_origin_y = 598.0;
        trigger_show_hand2 = false;
        last_line = 3;
    elseif (last_line ~= 4 and DialogueManager:GetLineCounter() == 4) then
        hand1_visible = true;
        hand2_visible = false;
        hand1_origin_x = 850.0;
        hand1_origin_y = 468.0;
        trigger_show_hand1 = false;
        last_line = 4;
    elseif (last_line ~= 5 and DialogueManager:GetLineCounter() == 5) then
        hand1_visible = true;
        hand2_visible = false;
        hand1_origin_x = 850.0;
        hand1_origin_y = 278.0;
        trigger_show_hand1 = false;
        last_line = 5;
    elseif (last_line ~= 6 and DialogueManager:GetLineCounter() == 6) then
        hand1_visible = false;
        hand2_visible = true;
        hand2_origin_x = 650.0;
        hand2_origin_y = 598.0;
        trigger_show_hand2 = false;
        last_line = 6;
    elseif (last_line ~= 7 and DialogueManager:GetLineCounter() == 7) then
        hand1_visible = true;
        hand2_visible = false;
        hand1_origin_x = 850.0;
        hand1_origin_y = 168.0;
        trigger_show_hand1 = false;
        last_line = 7;
    elseif (last_line ~= 8 and DialogueManager:GetLineCounter() == 8) then
        hand1_visible = false;
        hand2_visible = true;
        hand2_origin_x = 550.0;
        hand2_origin_y = 598.0;
        trigger_show_hand2 = false;
        last_line = 8;
    elseif (last_line ~= 10 and DialogueManager:GetLineCounter() == 10) then
        hand1_visible = false;
        hand2_visible = false;
        last_line = 10;
    elseif (DialogueManager:GetLineCounter() >= 13) then
        hand1_visible = false;
        hand2_visible = false;
        last_line = 15;
    end

    -- get time expired
    local time_expired = SystemManager:GetUpdateTime();

    -- Left to right hand handling
    -- The hand has reached the origin point, let's make it bounce
    if (hand1_bouncing_x >= hand1_origin_x) then
        bouncing1_force = 5.0;
    end

    -- Apply pseudo gravity
    bouncing1_force = bouncing1_force - (time_expired * 0.01);

    hand1_bouncing_x = hand1_bouncing_x - bouncing1_force;

    -- Update the hand shown.
    if (hand1_visible == true) then
        if (trigger_show_hand1 == false) then
            trigger_show_hand1 = true;
            hand1_bouncing_x = hand1_origin_x;
        end
    end

    -- Top to bottom hand handling
    -- The hand has reached the origin point, let's make it bounce
    if (hand2_bouncing_y >= hand2_origin_y) then
        bouncing2_force = 5.0;
    end

    -- Apply pseudo gravity
    bouncing2_force = bouncing2_force - (time_expired * 0.01);

    hand2_bouncing_y = hand2_bouncing_y - bouncing2_force;

    -- Update the hand shown.
    if (hand2_visible == true) then
        if (trigger_show_hand2 == false) then
            trigger_show_hand2 = true;
            hand2_bouncing_y = hand2_origin_y;
        end
    end
end

local white_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawPostEffects()

    -- Draw the hands
    if (hand1_visible == true) then
        VideoManager:Move(hand1_bouncing_x, hand1_origin_y);
        hand1_image:Draw(white_color);
    end
    if (hand2_visible == true) then
        VideoManager:Move(hand2_origin_x, hand2_bouncing_y);
        hand2_image:Draw(white_color);
    end
end
