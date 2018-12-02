local ns = {}
setmetatable(ns, {__index = _G})
overworld_battle_background_script = ns;
setfenv(1, ns);

-- Set the correct battle background according to the zone type
function Initialize(battle_instance)

    local background_type = GlobalManager:GetGameEvents():GetEventValue("overworld", "battle_background");
    if (background_type == 0) then return end

    local background_file = "";

    if (background_type == 2) then
        background_file = "data/battles/battle_scenes/forest_background.png";
        --battle_instance:GetScriptSupervisor():AddScript("data/battles/battle_scenes/forest_battle_anim.lua");
    elseif (background_type == 3) then
        background_file = "data/battles/battle_scenes/desert_background.png";
        --battle_instance:GetScriptSupervisor():AddScript("data/battles/battle_scenes/desert_battle_anim.lua");
    elseif (background_type == 4) then
        background_file = "data/battles/battle_scenes/snow_background.png";
        --battle_instance:GetScriptSupervisor():AddScript("data/battles/battle_scenes/snow_battle_anim.lua");
    else -- == 1 or else
        background_file = "data/battles/battle_scenes/plains_background.png";
    end

    GlobalManager:GetBattleMedia():SetBackgroundImage(background_file);
end
