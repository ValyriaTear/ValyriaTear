function BootMenuTest()
	print("BootMenuTest");

	GlobalManager:AddCharacter(1);
	GlobalManager:AddCharacter(2);
	GlobalManager:AddCharacter(4);
	GlobalManager:AddCharacter(8);
	GlobalManager:AddDrunes(1842);
	GlobalManager:AddToInventory(1, 5);
	GlobalManager:AddToInventory(1001, 2);
	GlobalManager:AddToInventory(10001, 10);
	GlobalManager:AddToInventory(10002, 10);
	GlobalManager:AddToInventory(10003, 10);
	GlobalManager:AddToInventory(30001, 10);
	GlobalManager:AddToInventory(30002, 10);
	GlobalManager:AddToInventory(30003, 10);

	local menu = hoa_menu.MenuMode(hoa_utils.MakeUnicodeString("The Boot Menu"), "img/menus/locations/desert_cave.png");
	ModeManager:Push(menu, false, false);
end
