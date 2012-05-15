objects = {}

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
	coll_height = 1.9,
	img_half_width = 3.0,
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

objects["Chair2"] = {
	animation_filename = "img/sprites/map/objects/chair2.lua",
	coll_half_width = 0.95,
	coll_height = 0.9,
	img_half_width = 0.95,
	img_height = 2.9
}

objects["Clock1"] = {
	animation_filename = "img/sprites/map/objects/clock1.lua",
	coll_half_width = 0.5,
	coll_height = 0.9,
	img_half_width = 0.5,
	img_height = 1.0
}

objects["Table1"] = {
	animation_filename = "img/sprites/map/objects/table1.lua",
	coll_half_width = 2.95,
	coll_height = 4.8,
	img_half_width = 2.95,
	img_height = 5.0
}

objects["Table2"] = {
	animation_filename = "img/sprites/map/objects/table2.lua",
	coll_half_width = 2.95,
	coll_height = 4.8,
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


-- "Ground" lights
-- Those are special light effect that have to be applied before the sky layer
objects["Left Window Light"] = {
	animation_filename = "img/misc/left_window_light.lua",
	coll_half_width = 0.0,
	coll_height = 0.0,
	img_half_width = 1.56,
	img_height = 4.0
}


-- The helper function permitting to easily create a prepared map object
object = {}

function _CreateObject(Map, name, x, y)
	if (objects[name] == nil) then
		print("Error: No object named: "..name.." found!!");
		return nil;
	end

	if (Map == nil) then
		print("Error: Function called with invalid Map object");
		return nil;
	end

	object = hoa_map.PhysicalObject();
	object:SetObjectID(Map.object_supervisor:GenerateObjectID());
	object:SetContext(hoa_map.MapMode.CONTEXT_01);
	object:SetPosition(x, y);
	object:SetCollHalfWidth(objects[name].coll_half_width);
	object:SetCollHeight(objects[name].coll_height);
	object:SetImgHalfWidth(objects[name].img_half_width);
	object:SetImgHeight(objects[name].img_height);
	object:AddAnimation(objects[name].animation_filename);

	return object;
end
