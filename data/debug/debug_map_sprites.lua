function TestFunction()
    print("Sprite Test");

    local map_mode = vt_map.MapMode("data/story/layna_forest/layna_forest_crystal_map.lua", "data/debug/subscripts/sprite_test.lua");
    ModeManager:Push(map_mode, true, true);
end
