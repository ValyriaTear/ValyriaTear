function BootMenuTest()
    print("BootMenuTest");

    GlobalManager:AddCharacter(BRONANN);
    GlobalManager:AddCharacter(KALYA);
    GlobalManager:AddCharacter(SYLVE);
    GlobalManager:AddCharacter(THANIS);

    -- Bronann
    local bronann = GlobalManager:GetCharacter(BRONANN);

    -- specials
    --bronann:AddSkill(20001);

    -- Add magic skills
    bronann:AddSkill(10003);

    -- elemental skills
    bronann:AddSkill(10100);
    bronann:AddSkill(10101);
    bronann:AddSkill(10106);
    bronann:AddSkill(10119);
    bronann:AddSkill(10132);
    bronann:AddSkill(10140);
    bronann:AddSkill(10143);
    bronann:AddSkill(10114);

    -- Kalya
    local kalya = GlobalManager:GetCharacter(KALYA);
    -- specials
    kalya:AddSkill(20011);
    --kalya:AddSkill(20012);

    -- Add Magic skills for testing
    kalya:AddSkill(10001);
    kalya:AddSkill(10002);

    -- elemental skills
    kalya:AddSkill(10107);
    kalya:AddSkill(10110);
    kalya:AddSkill(10111);
    kalya:AddSkill(10112);
    kalya:AddSkill(10116);
    kalya:AddSkill(10125);
    kalya:AddSkill(10129);
    kalya:AddSkill(10141);
    kalya:AddSkill(10164);
    kalya:AddSkill(10113);
    kalya:AddSkill(10165);
    kalya:AddSkill(10133);
    kalya:AddSkill(10120);

    -- Sylve
    local sylve = GlobalManager:GetCharacter(SYLVE);
    -- Elementals
    sylve:AddSkill(10103);
    sylve:AddSkill(10121);
    sylve:AddSkill(10122);
    sylve:AddSkill(10123);
    sylve:AddSkill(10124);
    sylve:AddSkill(10131);
    sylve:AddSkill(10134);
    sylve:AddSkill(10162);
    sylve:AddSkill(10105);
    sylve:AddSkill(10115);
    sylve:AddSkill(10126);
    sylve:AddSkill(10136);


    -- Thanis
    local thanis = GlobalManager:GetCharacter(THANIS);

    -- elementals
    thanis:AddSkill(10100);
    thanis:AddSkill(10101);
    thanis:AddSkill(10102);
    thanis:AddSkill(10104);
    thanis:AddSkill(10109);
    thanis:AddSkill(10118);
    thanis:AddSkill(10150);
    thanis:AddSkill(10151);
    thanis:AddSkill(10152);
    thanis:AddSkill(10108);
    thanis:AddSkill(10163);


    -- Equipment
    GlobalManager:AddDrunes(1842);
    GlobalManager:AddToInventory(1, 5);
    GlobalManager:AddToInventory(1001, 2);
    GlobalManager:AddToInventory(3001, 1);
    GlobalManager:AddToInventory(3002, 1);
    GlobalManager:AddToInventory(3003, 1);
    GlobalManager:AddToInventory(3004, 1);
    GlobalManager:AddToInventory(3005, 1);
    GlobalManager:AddToInventory(3006, 1);
    GlobalManager:AddToInventory(3007, 1);
    GlobalManager:AddToInventory(3008, 1);
    GlobalManager:AddToInventory(10001, 10);
    GlobalManager:AddToInventory(10002, 10);
    GlobalManager:AddToInventory(10003, 10);
    GlobalManager:AddToInventory(10999, 1);
    GlobalManager:AddToInventory(30011, 10);
    GlobalManager:AddToInventory(30012, 10);
    GlobalManager:AddToInventory(30013, 10);

    -- Add the spirits
    GlobalManager:AddToInventory(60001, 1);
    GlobalManager:AddToInventory(60002, 1);
    GlobalManager:AddToInventory(60003, 1);
    GlobalManager:AddToInventory(60004, 1);
    GlobalManager:AddToInventory(60005, 1);
    GlobalManager:AddToInventory(60006, 1);
    GlobalManager:AddToInventory(60007, 1);

    -- Used to debug the quest log
    GlobalManager:AddQuestLog("get_barley");
    GlobalManager:AddQuestLog("find_pen");
    GlobalManager:AddQuestLog("wants_to_go_into_the_forest");
    GlobalManager:AddQuestLog("bring_orlinn_back");
    GlobalManager:AddQuestLog("flee_from_the_dark_soldiers");

    -- Used to debug World Map
    GlobalManager:SetWorldMap("img/menus/worldmaps/layna_region.png")
    GlobalManager:ShowWorldLocation("layna village")
    GlobalManager:ShowWorldLocation("layna forest")
    GlobalManager:ShowWorldLocation("mt elbrus")
    GlobalManager:SetCurrentLocationId("layna forest")

    GlobalManager:SetMap("dummy_map", "dummy_script",
                         "img/menus/locations/desert_cave.png",
                         vt_utils.MakeUnicodeString("The Boot Menu"));

    local menu = vt_menu.MenuMode();
    ModeManager:Push(menu, false, false);
end
