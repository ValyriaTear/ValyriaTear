treasures = {}

treasures["Vase1"] = {
	closed_animation_filename = "img/sprites/map/objects/vase1.lua",
	opening_animation_filename = "", -- no opening sequence
	opened_animation_filename = "img/sprites/map/objects/vase1.lua",
	coll_half_width = 0.75,
	coll_height = 1.2,
	img_half_width = 0.75,
	img_height = 1.5
}

treasures["Wood_Chest1"] = {
	closed_animation_filename = "img/sprites/map/treasures/chest1_closed.lua",
	opening_animation_filename = "img/sprites/map/treasures/chest1_opening.lua",
	opened_animation_filename = "img/sprites/map/treasures/chest1_open.lua",
	coll_half_width = 0.6,
	coll_height = 1.0,
	img_half_width =1.0,
	img_height = 2.0
}

-- The helper function permitting to easily create a prepared map object
treasure = {}

function CreateTreasure(Map, save_name, name, x, y)
	if (treasures[name] == nil) then
		print("Error: No treasure named: "..name.." found!!");
		return nil;
	end

	if (Map == nil) then
		print("Error: Function called with invalid Map object");
		return nil;
	end
	treasure = hoa_map.TreasureObject(save_name,
		treasures[name].closed_animation_filename,
		treasures[name].opening_animation_filename,
		treasures[name].opened_animation_filename);
	treasure:SetObjectID(Map.object_supervisor:GenerateObjectID());
	treasure:SetContext(hoa_map.MapMode.CONTEXT_01);
	treasure:SetPosition(x, y);
	treasure:SetCollHalfWidth(treasures[name].coll_half_width);
	treasure:SetCollHeight(treasures[name].coll_height);
	treasure:SetImgHalfWidth(treasures[name].img_half_width);
	treasure:SetImgHeight(treasures[name].img_height);

	return treasure;
end
