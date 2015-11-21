function TestFunction()
    print("Audio Test");

    local map_mode = vt_map.MapMode("data/story/layna_forest/layna_forest_crystal_map.lua", "data/debug/subscripts/audio_test.lua");
    ModeManager:Push(map_mode, true, true);
end
