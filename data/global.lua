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
GlobalManager:SetMaxExperienceLevel(20);

function NewGame()
    -- Prepare the new game data
    GlobalManager:GetCharacterHandler():AddCharacter(BRONANN);
    GlobalManager:SetDrunes(0);

    -- Set the starting world map data
    local worldmap = GlobalManager:GetWorldMapData()
    worldmap:SetWorldMapImage("data/story/common/worldmaps/layna_region.png")
    worldmap:SetWorldLocationVisible("layna village", true)
    worldmap:SetCurrentLocationId("layna village")

    -- Don't permit autosave at that moment.
    local MM = vt_map.MapMode("data/story/ep1/layna_forest/layna_forest_crystal_map.lua", "data/story/ep1/introduction.lua",
                              0, false);
    ModeManager:Pop(false, false);
    ModeManager:Push(MM, false, true);
end
