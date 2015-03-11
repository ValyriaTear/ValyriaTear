local ns = {}
setmetatable(ns, {__index = _G})
tutorial_shop_dialogs = ns;
setfenv(1, ns);

local hand1_image = nil;
local hand2_image = nil;
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

local Shop = nil;
local Script = nil;
local DialogueManager = nil;
local stop_script = false;
local start_timer = nil;

local dialogue_started = false;

function Initialize(shop_instance)
    Shop = shop_instance;
    Script = Shop:GetScriptSupervisor();

    stop_script = false;

    if (GlobalManager:GetEventValue("story", "first_shop") ~= 1) then
        GlobalManager:SetEventValue("story", "first_shop", 1);
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

    DialogueManager = Shop:GetDialogueSupervisor();

    -- Add all speakers for the dialogues to be added
    DialogueManager:AddSpeaker("Bronann", vt_system.Translate("Bronann"), "data/entities/portraits/bronann.png");
    DialogueManager:AddSpeaker("Flora", vt_system.Translate("Flora"), "");

    -- The dialogue constructed below offers the player instructions on how to do battle. It is displayed only once in the first few seconds
    -- of battle, before any action can be taken. The player is presented with several options that they can read to get more information on
    -- the battle system. One of the options that the player may select from will finish the dialogue, allow the battle to resume.
    local main_dialogue = vt_common.Dialogue.Create(DialogueManager, "Shop tutorial first dialogue");
    local text = vt_system.Translate("It seems it is your first time around, right? Need help?");
    main_dialogue:AddLine(text, "Flora");
    text = vt_system.Translate("...");
    main_dialogue:AddLine(text, "Bronann");
    text = vt_system.Translate("I wouldn't mind some help...");
    main_dialogue:AddOption(text, 2);
    text = vt_system.Translate("I should be fine, thanks.");
    main_dialogue:AddOption(text, 12);
    -- [Line 2] After selecting option: Tell about shop basics.
    text = vt_system.Translate("I wouldn't mind some help...");
    main_dialogue:AddLine(text, "Bronann");
    text = vt_system.Translate("Welcome to my shop, then! \nFirst of all you can choose to either 'Buy' or 'Sell' items from the top menu right here... \n'Trade' is for special offers that don't only cost money.");
    main_dialogue:AddLine(text, "Flora");
    text = vt_system.Translate("Don't worry, I'll make good prices for you as you can testify in the buy ranking right there!");
    main_dialogue:AddLine(text, "Flora");
    text = vt_system.Translate("For now, let's see what I have in store and select the 'Buy' option...");
    main_dialogue:AddLine(text, "Flora");
    -- Set State buy
    text = vt_system.Translate("Here you can see the list of available wares, how many items I've got and how many you already have for each.");
    main_dialogue:AddLine(text, "Flora");
    text = vt_system.VTranslate("Don't forget that certain items are limited or unique! \nWhen reviewing items, you can also cycle through the proposed item categories using the menu key (%s).", InputManager:GetMenuKeyName());
    main_dialogue:AddLine(text, "Flora");
    text = vt_system.Translate("Let's have a look at the potions...");
    main_dialogue:AddLine(text, "Flora");
    -- Set State buy potions
    text = vt_system.Translate("Once you're here, you'll be able to add or remove how many items you want by pressing left or right. Of course, only if you have money, Sweeties...");
    main_dialogue:AddLine(text, "Flora");
    text = vt_system.Translate("Shall I repeat?");
    main_dialogue:AddLine(text, "Flora");
    text = vt_system.Translate("...");
    main_dialogue:AddLine(text, "Bronann");
    text = vt_system.Translate("Yes, please!");
    main_dialogue:AddOption(text, 3);
    text = vt_system.Translate("No, it's alright!");
    main_dialogue:AddOption(text, 12);
    -- Line 12: End of dialogue
    text = vt_system.Translate("Alright then, take a look around...");
    main_dialogue:AddLine(text, "Flora");

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
    if (start_timer:IsInitial() == true) then
        start_timer:Run();
    end

    -- If the dialogue is done, end the scene
    if ((dialogue_started == true) and (DialogueManager:IsDialogueActive()) == false) then
        Shop:SetInputEnabled(true);
        stop_script = true;

        -- Reset menu
        Shop:ChangeState(vt_shop.ShopMode.SHOP_STATE_ROOT)
    end

    -- If the dialogue has not been seen yet, check if its time to start it
    if ((dialogue_started == false) and (start_timer:IsFinished() == true) and (DialogueManager:IsDialogueActive() == false)) then
        DialogueManager:SetDialoguePosition(512.0, 760.0)
        DialogueManager:StartDialogue("Shop tutorial first dialogue");
        Shop:SetInputEnabled(false);
        dialogue_started = true;
    end

    -- Set up whether the hand should be shown and where
    if (DialogueManager:GetLineCounter() < 3) then
        hand1_visible = false;
        hand2_visible = false;
    elseif (last_line ~= 3 and DialogueManager:GetLineCounter() == 3) then
        -- Buy sell menu
        Shop:ChangeState(vt_shop.ShopMode.SHOP_STATE_ROOT)

        hand1_visible = true;
        hand2_visible = false;
        hand1_origin_x = 170.0;
        hand1_origin_y = 100.0;
        trigger_show_hand1 = false;
        last_line = 3;
    elseif (last_line ~= 4 and DialogueManager:GetLineCounter() == 4) then
        -- showing stars
        hand1_visible = true;
        hand2_visible = false;
        hand1_origin_x = 220.0;
        hand1_origin_y = 300.0;
        trigger_show_hand1 = false;
        last_line = 4;
    elseif (last_line ~= 5 and DialogueManager:GetLineCounter() == 5) then
        -- Show the buy option.
        hand1_visible = false;
        hand2_visible = true;
        hand2_origin_x = 300.0;
        hand2_origin_y = 50.0;
        trigger_show_hand2 = false;
        last_line = 5;
    elseif (last_line ~= 6 and DialogueManager:GetLineCounter() == 6) then
        -- Show the wares list
        Shop:ChangeState(vt_shop.ShopMode.SHOP_STATE_BUY)

        hand1_visible = false;
        hand2_visible = true;
        hand2_origin_x = 540.0;
        hand2_origin_y = 200.0;
        trigger_show_hand2 = false;
        last_line = 6;
    elseif (last_line ~= 7 and DialogueManager:GetLineCounter() == 7) then
        -- Show limits
        hand1_visible = true;
        hand2_visible = false;
        hand1_origin_x = 580.0;
        hand1_origin_y = 368.0;
        trigger_show_hand1 = false;
        last_line = 7;
    elseif (last_line ~= 8 and DialogueManager:GetLineCounter() == 8) then
        -- Show potions
        hand1_visible = true;
        hand2_visible = false;
        hand1_origin_x = 250.0;
        hand1_origin_y = 250.0;
        trigger_show_hand1 = false;
        last_line = 8;
    elseif (last_line ~= 9 and DialogueManager:GetLineCounter() == 9) then
        -- Show the potions info and buy amount
        Shop:ChangeViewMode(vt_shop.ShopMode.SHOP_VIEW_MODE_INFO);
        -- Show where the buy amount is.
        hand1_visible = true;
        hand2_visible = false;
        hand1_origin_x = 580.0;
        hand1_origin_y = 210.0;
        trigger_show_hand1 = false;
        last_line = 9
    elseif (DialogueManager:GetLineCounter() >= 12) then
        hand1_visible = false;
        hand2_visible = false;
        last_line = 12;
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
