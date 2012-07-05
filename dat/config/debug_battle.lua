function BootBattleTest()
	print("BootBattleTest");

--	GlobalManager:SetBattleSetting(hoa_global.GameGlobal.GLOBAL_BATTLE_WAIT);
	GlobalManager:SetBattleSetting(hoa_global.GameGlobal.GLOBAL_BATTLE_ACTIVE);

	GlobalManager:AddCharacter(1);
	GlobalManager:AddCharacter(2);
	GlobalManager:AddCharacter(4);
	GlobalManager:AddCharacter(8);
	GlobalManager:AddToInventory(1, 5);

	-- local claudius = GlobalManager:GetCharacter(1);
	-- claudius:AddSkill(10001);

	local battle = hoa_battle.BattleMode();
	battle:AddEnemy(6);
	battle:AddEnemy(7);
	battle:AddEnemy(8);
	battle:AddEnemy(9);
	battle:AddEnemy(10);
	battle:AddEnemy(11);
	battle:AddEnemy(12);
	battle:GetScriptSupervisor():AddScript("dat/battles/desert_cave_battle_anim.lua");
	battle:GetScriptSupervisor():AddScript("dat/battles/tutorial_battle_dialogs.lua");

	ModeManager:Push(battle, false, false);
end
