objects = {}

objects["Barrel1"] = {
	animation_filename = "img/sprites/map/objects/barrel1.lua",
	coll_half_width = 0.78,
	coll_height = 1.6,
	img_half_width = 0.78,
	img_height = 2.125
}

objects["Bed1"] = {
	animation_filename = "img/sprites/map/objects/bed1.lua",
	coll_half_width = 1.75,
	coll_height = 5.50,
	img_half_width = 1.75,
	img_height = 5.68
}

objects["Bench1"] = {
	animation_filename = "img/sprites/map/objects/bench1.lua",
	coll_half_width = 3.0,
	coll_height = 1.6,
	img_half_width = 3.0,
	img_height = 2.0
}

objects["Bench2"] = {
	animation_filename = "img/sprites/map/objects/bench2.lua",
	coll_half_width = 1.0,
	coll_height = 1.6,
	img_half_width = 1.0,
	img_height = 2.0
}

objects["Box1"] = {
	animation_filename = "img/sprites/map/objects/box1.lua",
	coll_half_width = 1.0,
	coll_height = 2.27,
	img_half_width = 1.0,
	img_height = 2.37
}

objects["Chair1"] = {
	animation_filename = "img/sprites/map/objects/chair1.lua",
	coll_half_width = 0.95,
	coll_height = 0.9,
	img_half_width = 0.95,
	img_height = 2.9
}

objects["Chair1_inverted"] = {
	animation_filename = "img/sprites/map/objects/chair1_inverted.lua",
	coll_half_width = 0.95,
	coll_height = 0.9,
	img_half_width = 0.95,
	img_height = 2.9
}

objects["Chair2"] = {
	animation_filename = "img/sprites/map/objects/chair2.lua",
	coll_half_width = 0.95,
	coll_height = 0.9,
	img_half_width = 0.95,
	img_height = 2.9
}

objects["Clock1"] = {
	animation_filename = "img/sprites/map/objects/clock1.lua",
	coll_half_width = 1.0,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 2.0
}

objects["Flower Pot1"] = {
	animation_filename = "img/sprites/map/objects/flower_pot1.lua",
	coll_half_width = 0.59,
	coll_height = 1.2,
	img_half_width = 0.59,
	img_height = 1.68
}

objects["Flower Pot2"] = {
	animation_filename = "img/sprites/map/objects/flower_pot2.lua",
	coll_half_width = 0.53,
	coll_height = 1.2,
	img_half_width = 0.53,
	img_height = 1.75
}

objects["Table1"] = {
	animation_filename = "img/sprites/map/objects/table1.lua",
	coll_half_width = 2.95,
	coll_height = 3.9,
	img_half_width = 2.95,
	img_height = 5.0
}

objects["Table2"] = {
	animation_filename = "img/sprites/map/objects/table2.lua",
	coll_half_width = 2.95,
	coll_height = 3.9,
	img_half_width = 2.95,
	img_height = 5.0
}

objects["Big Wooden Table"] = {
	animation_filename = "img/sprites/map/objects/wooden_table_big.lua",
	coll_half_width = 3.0,
	coll_height = 4.77,
	img_half_width = 3.0,
	img_height = 4.87
}

objects["Small Wooden Table"] = {
	animation_filename = "img/sprites/map/objects/wooden_table_small.lua",
	coll_half_width = 2.0,
	coll_height = 2.7,
	img_half_width = 2.0,
	img_height = 4.0
}

objects["Vase1"] = {
	animation_filename = "img/sprites/map/objects/vase1.lua",
	coll_half_width = 0.75,
	coll_height = 1.2,
	img_half_width = 0.75,
	img_height = 1.5
}


-- "Ground" lights
-- Those are special light effect that have to be applied before the sky layer
objects["Left Window Light"] = {
	animation_filename = "img/misc/lights/left_window_light.lua",
	coll_half_width = 0.0,
	coll_height = 0.0,
	img_half_width = 1.56,
	img_height = 4.5
}

objects["Right Window Light"] = {
	animation_filename = "img/misc/lights/right_window_light.lua",
	coll_half_width = 0.0,
	coll_height = 0.0,
	img_half_width = 1.56,
	img_height = 4.5
}

objects["Left Window Light 2"] = {
	animation_filename = "img/misc/lights/left_window_light.lua",
	coll_half_width = 0.0,
	coll_height = 0.0,
	img_half_width = 2.56,
	img_height = 6.5
}

objects["Right Window Light 2"] = {
	animation_filename = "img/misc/lights/right_window_light.lua",
	coll_half_width = 0.0,
	coll_height = 0.0,
	img_half_width = 2.56,
	img_height = 6.5
}

-- The helper function permitting to easily create a prepared map object
object = {}

function _CreateObject(Map, name, x, y, x_off, y_off)
	if (objects[name] == nil) then
		print("Error: No object named: "..name.." found!!");
		return nil;
	end

	if (Map == nil) then
		print("Error: Function called with invalid Map object");
		return nil;
    end

	if (x_off == nil) then
		x_off = 0.5;
	end
	if (y_off == nil) then
		y_off = 0.5;
	end

	object = hoa_map.PhysicalObject();
	object:SetObjectID(Map.object_supervisor:GenerateObjectID());
	object:SetContext(hoa_map.MapMode.CONTEXT_01);
	object:SetXPosition(x, x_off);
	object:SetYPosition(y, y_off);
	object:SetCollHalfWidth(objects[name].coll_half_width);
	object:SetCollHeight(objects[name].coll_height);
	object:SetImgHalfWidth(objects[name].img_half_width);
	object:SetImgHeight(objects[name].img_height);
	object:AddAnimation(objects[name].animation_filename);

	return object;
end
