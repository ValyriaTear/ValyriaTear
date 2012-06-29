function BootBattleTest()
	print("BootBattleTest");

--	GlobalManager:SetBattleSetting(hoa_global.GameGlobal.GLOBAL_BATTLE_WAIT);
	GlobalManager:SetBattleSetting(hoa_global.GameGlobal.GLOBAL_BATTLE_ACTIVE);

	GlobalManager:AddCharacter(BRONANN);
	GlobalManager:AddCharacter(KALYA);
	GlobalManager:AddCharacter(SYLVE);
	GlobalManager:AddCharacter(THANIS);
	GlobalManager:AddToInventory(1, 5);

	-- local bronann = GlobalManager:GetCharacter(1);
	-- bronann:AddSkill(10001);

	local battle = hoa_battle.BattleMode();
	battle:AddEnemy(6);
	battle:AddEnemy(7);
	battle:AddEnemy(8);
	battle:AddEnemy(9);
	battle:AddEnemy(10);
	battle:AddEnemy(11);
	battle:AddEnemy(12);
	battle:GetScriptSupervisor():AddScript("dat/battles/desert_cave_battle_anim.lua");
	--battle:GetScriptSupervisor():AddScript("dat/battles/tutorial_battle_dialogs.lua");

	ModeManager:Push(battle, false, false);
end
