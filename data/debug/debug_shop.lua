-- Debug shop script
function TestFunction()
    print("Shop Test");

    local character_handler = GlobalManager:GetCharacterHandler()
    character_handler:AddCharacter(BRONANN)
    character_handler:AddCharacter(KALYA)
    character_handler:AddCharacter(SYLVE)
    character_handler:AddCharacter(THANIS)

    -- Initial character money
    GlobalManager:AddDrunes(1842);

    -- Initial inventory
    local inventory_handler = GlobalManager:GetInventoryHandler()
    inventory_handler:AddToInventory(1, 5);
    inventory_handler:AddToInventory(1001, 2);
    inventory_handler:AddToInventory(10001, 1);
    inventory_handler:AddToInventory(20002, 1);
    inventory_handler:AddToInventory(30001, 1);
    inventory_handler:AddToInventory(40001, 1);
    inventory_handler:AddToInventory(50001, 1);

    local shop = vt_shop.ShopMode("Debug shop");
    shop:SetShopName(vt_system.UTranslate("Flora's Shop"));
    shop:SetGreetingText(vt_system.UTranslate("Don't forget my trade offers!"));

    -- AddObject(item_id, number)
    shop:AddItem(1, 12);
    shop:AddItem(1001, 5);
    shop:AddItem(1003, 1);
    shop:AddItem(10001, 2);
    shop:AddItem(10002, 4);
    shop:AddItem(10003, 12);
    shop:AddItem(10004, 1);
    shop:AddItem(10999, 1);
    shop:AddItem(11001, 5);
    shop:AddItem(20002, 1);
    shop:AddItem(20003, 2);
    shop:AddItem(30001, 3);
    shop:AddItem(30002, 10);
    shop:AddItem(30003, 11);
    shop:AddItem(40001, 2);
    shop:AddItem(40002, 1);
    shop:AddItem(40003, 1);
    shop:AddItem(40004, 1);
    shop:AddItem(50001, 1);
    shop:AddItem(50002, 1);
    shop:AddItem(3100, 10);

    -- AddTrade(item_id to trade for, number)
    -- the trade conditions are within the item declaration
    shop:AddTrade(2, 2);
    shop:AddTrade(3, 2);
    shop:AddTrade(10004, 1);
    shop:AddTrade(10999, 1);
    shop:AddTrade(1, 0);

    shop:GetScriptSupervisor():AddScript("data/story/layna_village/tutorial_shop_dialogs.lua");

    ModeManager:Push(shop, false, false);
end
