function BootMenuTest()
    print("BootMenuTest");

    GlobalManager:AddCharacter(BRONANN);
    GlobalManager:AddCharacter(KALYA);
    GlobalManager:AddCharacter(SYLVE);
    GlobalManager:AddCharacter(THANIS);

    -- Add a special to Kalya
    local kalya = GlobalManager:GetCharacter(2);
    kalya:AddSkill(20001);

    GlobalManager:AddDrunes(1842);
    GlobalManager:AddToInventory(1, 5);
    GlobalManager:AddToInventory(1001, 2);
    GlobalManager:AddToInventory(10001, 10);
    GlobalManager:AddToInventory(10002, 10);
    GlobalManager:AddToInventory(10003, 10);
    GlobalManager:AddToInventory(10004, 1);
    GlobalManager:AddToInventory(10999, 1);
    GlobalManager:AddToInventory(30011, 10);
    GlobalManager:AddToInventory(30012, 10);
    GlobalManager:AddToInventory(30013, 10);

    -- Used to debug the quest log
    GlobalManager:AddQuestLog("get_barley");
    GlobalManager:AddQuestLog("find_pen");
    GlobalManager:AddQuestLog("wants_to_go_into_the_forest");
    GlobalManager:AddQuestLog("bring_orlinn_back");

    -- Used to debug World Map
    GlobalManager:SetWorldMap("img/menus/worldmaps/test_map.png")
    GlobalManager:ShowWorldLocation("layna village")
    GlobalManager:ShowWorldLocation("layna forest")
    GlobalManager:SetCurrentLocationId("layna forest")

    GlobalManager:SetMap("dummy_map", "dummy_script",
                         "img/menus/locations/desert_cave.png",
                         hoa_utils.MakeUnicodeString("The Boot Menu"));

    local menu = hoa_menu.MenuMode();
    ModeManager:Push(menu, false, false);
end
