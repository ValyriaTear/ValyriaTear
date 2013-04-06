function BootBattleTest()
    print("BootBattleTest");

    GlobalManager:AddCharacter(BRONANN);
    GlobalManager:AddCharacter(KALYA);
    GlobalManager:AddCharacter(SYLVE);
    GlobalManager:AddCharacter(THANIS);
    GlobalManager:AddToInventory(1, 5);
    GlobalManager:AddToInventory(1003, 1);


    -- local bronann = GlobalManager:GetCharacter(1);
    -- bronann:AddSkill(10001);

    local battle = vt_battle.BattleMode();
    battle:AddEnemy(6, 0, 0);
    battle:AddEnemy(7, 0, 0);
    battle:AddEnemy(8, 0, 0);
    battle:AddEnemy(9, 0, 0);
    battle:AddEnemy(60, 0, 0);
    battle:AddEnemy(61, 0, 0);
    battle:AddEnemy(62, 0, 0);
    battle:GetScriptSupervisor():AddScript("dat/battles/desert_cave_battle_anim.lua");

    -- Set the battle to wait, semi active or active
    --battle:SetBattleType(vt_battle.BattleMode.BATTLE_TYPE_WAIT);
    --battle:SetBattleType(vt_battle.BattleMode.BATTLE_TYPE_SEMI_ACTIVE);
    battle:SetBattleType(vt_battle.BattleMode.BATTLE_TYPE_ACTIVE);

    ModeManager:Push(battle, false, false);
end
