function BootBattleTest()
	print("BootBattleTest");

--	GlobalManager:SetBattleSetting(hoa_global.GameGlobal.GLOBAL_BATTLE_WAIT);
	GlobalManager:SetBattleSetting(hoa_global.GameGlobal.GLOBAL_BATTLE_ACTIVE);

	GlobalManager:AddCharacter(BRONANN);
	GlobalManager:AddCharacter(KALYA);
	GlobalManager:AddCharacter(SYLVE);
	GlobalManager:AddCharacter(THANIS);
	GlobalManager:AddToInventory(1, 5);
	GlobalManager:AddToInventory(1003, 1);
 

	-- local bronann = GlobalManager:GetCharacter(1);
	-- bronann:AddSkill(10001);

	local battle = hoa_battle.BattleMode();
	battle:AddEnemy(6, 0, 0);
	battle:AddEnemy(7, 0, 0);
	battle:AddEnemy(8, 0, 0);
	battle:AddEnemy(9, 0, 0);
	battle:AddEnemy(10, 0, 0);
	battle:AddEnemy(11, 0, 0);
	battle:AddEnemy(12, 0, 0);
	battle:GetScriptSupervisor():AddScript("dat/battles/desert_cave_battle_anim.lua");

	ModeManager:Push(battle, false, false);
end
