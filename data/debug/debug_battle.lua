function TestFunction()
    print("Battle Test");

    local character_handler = GlobalManager:GetCharacterHandler()
    character_handler:AddCharacter(BRONANN)
    character_handler:AddCharacter(KALYA)
    character_handler:AddCharacter(SYLVE)
    character_handler:AddCharacter(THANIS)

    local inventory_handler = GlobalManager:GetInventoryHandler()
    inventory_handler:AddToInventory(1, 10)
    inventory_handler:AddToInventory(1003, 10)

    local bronann = character_handler:GetCharacter(BRONANN)
    bronann:AddSkill(2) -- Forward Thrust
    bronann:AddSkill(10120) -- Wave
    bronann:AddSkill(10111) -- Earth Circle
    bronann:AddSkill(10121) -- Water Circle
    bronann:AddSkill(10131) -- Wind Circle
    bronann:SetMaxHitPoints(9999)
    bronann:SetHitPoints(9999)

    local thanis = character_handler:GetCharacter(THANIS)
    thanis:AddSkill(2) -- Forward Thrust

    local battle = vt_battle.BattleMode()
    battle:AddEnemy(1, 0, 0)
    battle:AddEnemy(2, 0, 0)
    --battle:AddEnemy(3, 0, 0) -- Fenrir
    battle:AddEnemy(4, 0, 0)
    battle:AddEnemy(5, 0, 0)
    --battle:AddEnemy(6, 0, 0)
    --battle:AddEnemy(7, 0, 0) -- Fenrir
    --battle:AddEnemy(8, 0, 0) -- Fenrir
    --battle:AddEnemy(9, 0, 0)
    battle:GetScriptSupervisor():AddScript("data/battles/battle_scenes/desert_cave_battle_anim.lua")
    GlobalManager:GetBattleMedia():SetBackgroundImage("data/battles/battle_scenes/desert_cave/desert_cave.png")
    --battle:GetScriptSupervisor():AddScript("data/story/layna_village/battle_with_banesore/battle_with_banesore_script.lua")
    battle:BoostHeroPartyInitiative()

    ModeManager:Push(battle, false, false)

    ScriptManager:DEBUG_DumpScriptsState()
end
