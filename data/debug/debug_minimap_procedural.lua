function TestFunction()
    print("Procedural Minimap Test");

    local map_mode = vt_map.MapMode("data/story/ep1/layna_forest/layna_forest_entrance_map.lua", "data/debug/subscripts/layna_forest_minimap_procedural_script.lua");
    ModeManager:Push(map_mode, true, true);
end
