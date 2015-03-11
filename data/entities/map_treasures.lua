treasures = {}

treasures["Jar1"] = {
    closed_animation_filename = "data/entities/map/objects/jar1.lua",
    opening_animation_filename = "data/entities/map/objects/jar1.lua",
    opened_animation_filename = "data/entities/map/objects/jar1.lua",
    coll_half_width = 0.5 * 16,
    coll_height = 1.2 * 16,
    img_half_width = 0.625 * 16,
    img_height = 1.68 * 16
}

treasures["Vase1"] = {
    closed_animation_filename = "data/entities/map/objects/vase1.lua",
    opening_animation_filename = "data/entities/map/objects/vase1.lua",
    opened_animation_filename = "data/entities/map/objects/vase1.lua",
    coll_half_width = 0.75 * 16,
    coll_height = 1.2 * 16,
    img_half_width = 0.75 * 16,
    img_height = 1.5 * 16
}

treasures["Vase2"] = {
    closed_animation_filename = "data/entities/map/objects/vase2.lua",
    opening_animation_filename = "data/entities/map/objects/vase2.lua",
    opened_animation_filename = "data/entities/map/objects/vase2.lua",
    coll_half_width = 0.75 * 16,
    coll_height = 1.3 * 16,
    img_half_width = 0.81 * 16,
    img_height = 2.5 * 16
}

treasures["Vase3"] = {
    closed_animation_filename = "data/entities/map/objects/vase3.lua",
    opening_animation_filename = "data/entities/map/objects/vase3.lua",
    opened_animation_filename = "data/entities/map/objects/vase3.lua",
    coll_half_width = 0.6 * 16,
    coll_height = 1.1 * 16,
    img_half_width = 0.68 * 16,
    img_height = 1.68 * 16
}

treasures["Vase4"] = {
    closed_animation_filename = "data/entities/map/objects/vase4.lua",
    opening_animation_filename = "data/entities/map/objects/vase4.lua",
    opened_animation_filename = "data/entities/map/objects/vase4.lua",
    coll_half_width = 0.5 * 16,
    coll_height = 1.1 * 16,
    img_half_width = 0.59 * 16,
    img_height = 2.0 * 16
}

treasures["Wood_Chest1"] = {
    closed_animation_filename = "data/entities/map/treasures/chest1_closed.lua",
    opening_animation_filename = "data/entities/map/treasures/chest1_opening.lua",
    opened_animation_filename = "data/entities/map/treasures/chest1_open.lua",
    coll_half_width = 0.6 * 16,
    coll_height = 1.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 2.0 * 16
}

treasures["Wood_Chest2"] = {
    closed_animation_filename = "data/entities/map/treasures/chest2_closed.lua",
    opening_animation_filename = "data/entities/map/treasures/chest2_open.lua",
    opened_animation_filename = "data/entities/map/treasures/chest2_open.lua",
    coll_half_width = 0.6 * 16,
    coll_height = 1.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 3.125 * 16
}

treasures["Wood_Chest3"] = {
    closed_animation_filename = "data/entities/map/treasures/chest3_closed.lua",
    opening_animation_filename = "data/entities/map/treasures/chest3_open.lua",
    opened_animation_filename = "data/entities/map/treasures/chest3_open.lua",
    coll_half_width = 0.6 * 16,
    coll_height = 1.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 3.125 * 16
}

treasures["Locker_Chest1"] = {
    closed_animation_filename = "data/entities/map/treasures/locker1_closed.lua",
    opening_animation_filename = "data/entities/map/treasures/locker1_opening.lua",
    opened_animation_filename = "data/entities/map/treasures/locker1_open.lua",
    coll_half_width = 0.93 * 16,
    coll_height = 1.0 * 16,
    img_half_width = 0.93 * 16,
    img_height = 1.81 * 16
}

-- The helper function permitting to easily create a prepared map object
function CreateTreasure(Map, save_name, name, x, y, layer)
    if (treasures[name] == nil) then
        print("Error: No treasure named: "..name.." found!!");
        return nil;
    end

    if (Map == nil) then
        print("Error: Function called with invalid Map object");
        return nil;
    end

    -- Note: Auto-registered to the object supervisor.
    local treasure = vt_map.TreasureObject.Create(save_name, layer,
               treasures[name].closed_animation_filename,
               treasures[name].opening_animation_filename,
               treasures[name].opened_animation_filename);

    treasure:SetPosition(x, y);
    treasure:SetCollPixelHalfWidth(treasures[name].coll_half_width);
    treasure:SetCollPixelHeight(treasures[name].coll_height);
    treasure:SetImgPixelHalfWidth(treasures[name].img_half_width);
    treasure:SetImgPixelHeight(treasures[name].img_height);

    return treasure;
end
