-- setfenv redeclaration for lua 5.2
setfenv = setfenv or function(f, t)
    f = (type(f) == 'function' and f or debug.getinfo(f + 1, 'f').func)
    local name
    local up = 0
    repeat
        up = up + 1
        name = debug.getupvalue(f, up)
    until name == '_ENV' or name == nil
    if name then
        debug.upvaluejoin(f, up, function() return name end, 1) -- use unique upvalue
        debug.setupvalue(f, up, t)
    end
end

-- Character IDs. Each ID can have only a single bit active as IDs are used in bitmask operations.
BRONANN  = 1;
KALYA    = 2;
SYLVE    = 4;
THANIS   = 8;

-- level cap
-- Set to lvl 15 as there is only one dungeon.
GlobalManager:SetMaxExperienceLevel(15);

function NewGame()
    -- Prepare the new game data
    GlobalManager:AddCharacter(BRONANN);
    GlobalManager:SetDrunes(0);

    -- Set the starting world map data
    GlobalManager:SetWorldMap("img/menus/worldmaps/layna_region.png")
    GlobalManager:ShowWorldLocation("layna village")
    GlobalManager:SetCurrentLocationId("layna village")

    local MM = vt_map.MapMode("dat/maps/layna_forest/layna_forest_crystal_map.lua", "dat/maps/introduction.lua");
    ModeManager:Pop(false, false);
    ModeManager:Push(MM, false, true);
end
