function BootBattleTest()
    print("BootBattleTest");

    GlobalManager:AddCharacter(BRONANN);
    GlobalManager:AddCharacter(KALYA);
    GlobalManager:AddCharacter(SYLVE);
    GlobalManager:AddCharacter(THANIS);
    GlobalManager:AddToInventory(1, 10);
    GlobalManager:AddToInventory(1003, 10);


    local bronann = GlobalManager:GetCharacter(BRONANN);
    bronann:AddSkill(10120); -- Wave
    bronann:SetMaxHitPoints(9999);
    bronann:SetHitPoints(9999);

    local battle = vt_battle.BattleMode();
    battle:AddEnemy(1, 0, 0);
    battle:AddEnemy(2, 0, 0);
    --battle:AddEnemy(3, 0, 0); -- Fenrir
    battle:AddEnemy(4, 0, 0);
    battle:AddEnemy(5, 0, 0);
    battle:AddEnemy(6, 0, 0);
    --battle:AddEnemy(7, 0, 0); -- Fenrir
    --battle:AddEnemy(8, 0, 0); -- Fenrir
    battle:AddEnemy(9, 0, 0);
    battle:GetScriptSupervisor():AddScript("dat/battles/desert_cave_battle_anim.lua");

    -- Set the battle to wait, semi active or active
    --battle:SetBattleType(vt_battle.BattleMode.BATTLE_TYPE_WAIT);
    --battle:SetBattleType(vt_battle.BattleMode.BATTLE_TYPE_SEMI_ACTIVE);
    battle:SetBattleType(vt_battle.BattleMode.BATTLE_TYPE_ACTIVE);

    ModeManager:Push(battle, false, false);
end
