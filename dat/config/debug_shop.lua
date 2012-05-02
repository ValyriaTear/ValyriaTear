-- Debug shop script
function BootShopTest()
	print("BootShopTest");
    -- The shop window is still crash prone

    -- Those calls make the game crash
	--GlobalManager:AddCharacter(BRONANN);

    -- The drunes displayed are flawed.
	GlobalManager:AddDrunes(1842);
    -- Those calls make the game crash
	--GlobalManager:AddToInventory(1, 5);
	--GlobalManager:AddToInventory(1001, 2);
	--GlobalManager:AddToInventory(10001, 1);

	local shop = hoa_shop.ShopMode();

    -- Adding only one item makes the game crash at item selection
	--shop:AddObject(1, 12);
	--shop:AddObject(1001, 5);
	--shop:AddObject(10001, 2);
	--shop:AddObject(10002, 4);
	--shop:AddObject(10003, 12);
	--shop:AddObject(20002, 1);
	--shop:AddObject(20003, 2);
	--shop:AddObject(30001, 3);
	--shop:AddObject(30002, 10);
	--shop:AddObject(30003, 11);
	--shop:AddObject(40001, 2);
	--shop:AddObject(40002, 1);
	--shop:AddObject(40003, 1);
	--shop:AddObject(40004, 1);
	--shop:AddObject(50001, 1);
	--shop:AddObject(50002, 1);

	ModeManager:Push(shop, false, false);

end
