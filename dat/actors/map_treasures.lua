treasures = {}

treasures["Jar1"] = {
    closed_animation_filename = "img/sprites/map/objects/jar1.lua",
    opening_animation_filename = "img/sprites/map/objects/jar1.lua",
    opened_animation_filename = "img/sprites/map/objects/jar1.lua",
    coll_half_width = 0.5,
    coll_height = 1.2,
    img_half_width = 0.625,
    img_height = 1.68
}

treasures["Vase1"] = {
    closed_animation_filename = "img/sprites/map/objects/vase1.lua",
    opening_animation_filename = "img/sprites/map/objects/vase1.lua",
    opened_animation_filename = "img/sprites/map/objects/vase1.lua",
    coll_half_width = 0.75,
    coll_height = 1.2,
    img_half_width = 0.75,
    img_height = 1.5
}

treasures["Vase2"] = {
    closed_animation_filename = "img/sprites/map/objects/vase2.lua",
    opening_animation_filename = "img/sprites/map/objects/vase2.lua",
    opened_animation_filename = "img/sprites/map/objects/vase2.lua",
    coll_half_width = 0.75,
    coll_height = 1.3,
    img_half_width = 0.81,
    img_height = 2.5
}

treasures["Vase3"] = {
    closed_animation_filename = "img/sprites/map/objects/vase3.lua",
    opening_animation_filename = "img/sprites/map/objects/vase3.lua",
    opened_animation_filename = "img/sprites/map/objects/vase3.lua",
    coll_half_width = 0.6,
    coll_height = 1.1,
    img_half_width = 0.68,
    img_height = 1.68
}

treasures["Vase4"] = {
    closed_animation_filename = "img/sprites/map/objects/vase4.lua",
    opening_animation_filename = "img/sprites/map/objects/vase4.lua",
    opened_animation_filename = "img/sprites/map/objects/vase4.lua",
    coll_half_width = 0.5,
    coll_height = 1.1,
    img_half_width = 0.59,
    img_height = 2.0
}

treasures["Wood_Chest1"] = {
    closed_animation_filename = "img/sprites/map/treasures/chest1_closed.lua",
    opening_animation_filename = "img/sprites/map/treasures/chest1_opening.lua",
    opened_animation_filename = "img/sprites/map/treasures/chest1_open.lua",
    coll_half_width = 0.6,
    coll_height = 1.0,
    img_half_width = 1.0,
    img_height = 2.0
}

treasures["Wood_Chest2"] = {
    closed_animation_filename = "img/sprites/map/treasures/chest2_closed.lua",
    opening_animation_filename = "img/sprites/map/treasures/chest2_open.lua",
    opened_animation_filename = "img/sprites/map/treasures/chest2_open.lua",
    coll_half_width = 0.6,
    coll_height = 1.0,
    img_half_width = 1.0,
    img_height = 3.125
}

treasures["Wood_Chest3"] = {
    closed_animation_filename = "img/sprites/map/treasures/chest3_closed.lua",
    opening_animation_filename = "img/sprites/map/treasures/chest3_open.lua",
    opened_animation_filename = "img/sprites/map/treasures/chest3_open.lua",
    coll_half_width = 0.6,
    coll_height = 1.0,
    img_half_width = 1.0,
    img_height = 3.125
}

treasures["Locker_Chest1"] = {
    closed_animation_filename = "img/sprites/map/treasures/locker1_closed.lua",
    opening_animation_filename = "img/sprites/map/treasures/locker1_opening.lua",
    opened_animation_filename = "img/sprites/map/treasures/locker1_open.lua",
    coll_half_width = 0.93,
    coll_height = 1.0,
    img_half_width = 0.93,
    img_height = 1.81
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
    treasure:SetCollHalfWidth(treasures[name].coll_half_width);
    treasure:SetCollHeight(treasures[name].coll_height);
    treasure:SetImgHalfWidth(treasures[name].img_half_width);
    treasure:SetImgHeight(treasures[name].img_height);

    return treasure;
end
