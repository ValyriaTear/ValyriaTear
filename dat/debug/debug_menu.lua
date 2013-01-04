function BootMenuTest()
    print("BootMenuTest");

    GlobalManager:AddCharacter(BRONANN);
    GlobalManager:AddCharacter(KALYA);
    GlobalManager:AddCharacter(SYLVE);
    GlobalManager:AddCharacter(THANIS);
    GlobalManager:AddDrunes(1842);
    GlobalManager:AddToInventory(1, 5);
    GlobalManager:AddToInventory(1001, 2);
    GlobalManager:AddToInventory(10001, 10);
    GlobalManager:AddToInventory(10002, 10);
    GlobalManager:AddToInventory(10003, 10);
    GlobalManager:AddToInventory(30011, 10);
    GlobalManager:AddToInventory(30012, 10);
    GlobalManager:AddToInventory(30013, 10);

    -- Used to debug the quest log
    GlobalManager:AddQuestLog("get_barley");
    GlobalManager:AddQuestLog("find_pen");

    local menu = hoa_menu.MenuMode(hoa_utils.MakeUnicodeString("The Boot Menu"), "img/menus/locations/desert_cave.png");
    ModeManager:Push(menu, false, false);
end
