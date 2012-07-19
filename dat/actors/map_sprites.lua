sprites = {}
enemies = {}

local NORMAL_SPEED = hoa_map.MapMode.NORMAL_SPEED;
local SLOW_SPEED = hoa_map.MapMode.SLOW_SPEED;
local VERY_SLOW_SPEED = hoa_map.MapMode.VERY_SLOW_SPEED;
local VERY_FAST_SPEED = hoa_map.MapMode.VERY_FAST_SPEED;

sprites["Bronann"] = {
	name = hoa_system.Translate("Bronann"),
	coll_half_width = 0.95,
	coll_height = 1.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = NORMAL_SPEED,

	standard_animations = "img/sprites/map/characters/bronann_walk_unarmed.png",
	running_animations = "img/sprites/map/characters/bronann_run_unarmed.png",
	face_portrait = "img/portraits/bronann.png"
}

sprites["Kalya"] = {
	name = hoa_system.Translate("Kalya"),
	coll_half_width = 0.95,
	coll_height = 1.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

	standard_animations = "img/sprites/map/characters/kalya_walk.png",
	running_animations = "img/sprites/map/characters/kalya_run.png",
	face_portrait = "img/portraits/kalya.png"
}

sprites["Thanis"] = {
	name = hoa_system.Translate("Thanis"),
	coll_half_width = 0.95,
	coll_height = 1.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

	standard_animations = "img/sprites/map/thanis_walk.png",
	face_portrait = "img/portraits/thanis.png"
}

sprites["Carson"] = {
	name = hoa_system.Translate("Carson"),
	coll_half_width = 0.95,
	coll_height = 1.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = VERY_SLOW_SPEED,

	standard_animations = "img/sprites/map/npcs/story/carson_walk.png",
	face_portrait = "img/portraits/npcs/carson.png"
}

sprites["Malta"] = {
	name = hoa_system.Translate("Malta"),
	coll_half_width = 0.95,
	coll_height = 1.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = VERY_SLOW_SPEED,

	standard_animations = "img/sprites/map/npcs/story/malta_walk.png",
	face_portrait = "img/portraits/npcs/malta.png"
}

sprites["Orlinn"] = {
	name = hoa_system.Translate("Orlinn"),
	coll_half_width = 0.95,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = NORMAL_SPEED,

	standard_animations = "img/sprites/map/npcs/npc_boy01_walk.png"
}


-- ----
-- NPCs
-- ----

sprites["Girl1"] = {
	name = hoa_system.Translate("Olivia"), -- default name
	coll_half_width = 0.95,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

	standard_animations = "img/sprites/map/npcs/npc_girl01_walk.png"
}

sprites["Old Woman1"] = {
	name = hoa_system.Translate("Brymir"), -- default name
	coll_half_width = 0.95,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

	standard_animations = "img/sprites/map/npcs/npc_old_woman01_walk.png"
}

sprites["Woman1"] = {
	name = hoa_system.Translate("Martha"), -- default name
	coll_half_width = 0.95,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

	standard_animations = "img/sprites/map/npcs/npc_woman01_walk.png"
}

sprites["Woman2"] = {
	name = hoa_system.Translate("Sophia"), -- default name
	coll_half_width = 1.0,
	coll_height = 2.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = NORMAL_SPEED,

	standard_animations = "img/sprites/map/npcs/npc_woman02_walk.png"
}

sprites["Woman3"] = {
	name = hoa_system.Translate("Lilly"), -- default name
	coll_half_width = 1.0,
	coll_height = 2.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = NORMAL_SPEED,

	standard_animations = "img/sprites/map/npcs/npc_woman03_walk.png"
}

sprites["Man1"] = {
	name = hoa_system.Translate("Georges"), -- default name
	coll_half_width = 0.95,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

	standard_animations = "img/sprites/map/npcs/npc_man01_walk.png"
}

sprites["Man2"] = {
	name = hoa_system.Translate("Xandar"), -- default name
	coll_half_width = 0.95,
	coll_height = 1.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

	standard_animations = "img/sprites/map/npcs/npc_man02_walk.png"
}

sprites["Soldier"] = {
	name = hoa_system.Translate("Soldier"),
	coll_half_width = 0.95,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

	standard_animations = "img/sprites/map/npc_soldier01_walk.png"
}

sprites["Lord"] = {
	name = hoa_system.Translate("Lord"),
	coll_half_width = 0.95,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

	standard_animations = "img/sprites/map/npc_soldier02_walk.png"
}


-- ---------
-- Enemies
-- ---------
enemies["slime"] = {
	coll_half_width = 1.0,
	coll_height = 2.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = NORMAL_SPEED,
	standard_animations = "img/sprites/map/enemies/slime_walk.png"
}


enemies["snake"] = {
	coll_half_width = 1.0,
	coll_height = 2.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = NORMAL_SPEED,
	standard_animations = "img/sprites/map/enemies/snake_walk.png"
}


enemies["scorpion"] = {
	coll_half_width = 1.0,
	coll_height = 2.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = NORMAL_SPEED,
	standard_animations = "img/sprites/map/enemies/scorpion_walk.png"
}


sprite = {}
enemy = {}

function _CreateSprite(Map, name, x, y)
	if (sprites[name] == nil) then
		print("Error: No object named: "..name.." found!!");
		return nil;
	end

	if (Map == nil) then
		print("Error: Function called with invalid Map object");
		return nil;
	end

	dir = (2 ^ math.random(0, 3));

	sprite = hoa_map.MapSprite();
	sprite:SetName(sprites[name].name);
	sprite:SetObjectID(Map.object_supervisor:GenerateObjectID());
	sprite:SetContext(hoa_map.MapMode.CONTEXT_01);
	sprite:SetPosition(x, y);
	sprite:SetCollHalfWidth(sprites[name].coll_half_width);
	sprite:SetCollHeight(sprites[name].coll_height);
	sprite:SetImgHalfWidth(sprites[name].img_half_width);
	sprite:SetImgHeight(sprites[name].img_height);
	sprite:SetMovementSpeed(sprites[name].movement_speed);
	sprite:SetDirection(dir);
	sprite:LoadStandardAnimations(sprites[name].standard_animations);
	if (sprites[name].running_animations) then
		sprite:LoadRunningAnimations(sprites[name].running_animations);
	end
	if (sprites[name].face_portrait) then
		sprite:LoadFacePortrait(sprites[name].face_portrait);
	end

		return sprite;
end

-- Permit to setup a custom name
-- and reuse the npcs sprites more easily
function _CreateNPCSprite(Map, name, npc_name, x, y)
	if (sprites[name] == nil) then
		print("Error: No object named: "..name.." found!!");
		return nil;
	end

	if (Map == nil) then
		print("Error: Function called with invalid Map object");
		return nil;
	end

	dir = (2 ^ math.random(0, 3));

	sprite = hoa_map.MapSprite();
	sprite:SetName(npc_name);
	sprite:SetObjectID(Map.object_supervisor:GenerateObjectID());
	sprite:SetContext(hoa_map.MapMode.CONTEXT_01);
	sprite:SetPosition(x, y);
	sprite:SetCollHalfWidth(sprites[name].coll_half_width);
	sprite:SetCollHeight(sprites[name].coll_height);
	sprite:SetImgHalfWidth(sprites[name].img_half_width);
	sprite:SetImgHeight(sprites[name].img_height);
	sprite:SetMovementSpeed(sprites[name].movement_speed);
	sprite:SetDirection(dir);
	sprite:LoadStandardAnimations(sprites[name].standard_animations);
	if (sprites[name].running_animations) then
		sprite:LoadRunningAnimations(sprites[name].running_animations);
	end
	if (sprites[name].face_portrait) then
		sprite:LoadFacePortrait(sprites[name].face_portrait);
	end

		return sprite;
end



function _CreateEnemySprite(Map, name)
	if (enemies[name] == nil) then
		print("Error: No object named: "..name.." found!!");
		return nil;
	end

	if (Map == nil) then
		print("Error: Function called with invalid Map object");
		return nil;
	end

	enemy = hoa_map.EnemySprite();
	enemy:SetObjectID(Map.object_supervisor:GenerateObjectID());
	enemy:SetContext(hoa_map.MapMode.CONTEXT_01);
	enemy:SetCollHalfWidth(enemies[name].coll_half_width);
	enemy:SetCollHeight(enemies[name].coll_height);
	enemy:SetImgHalfWidth(enemies[name].img_half_width);
	enemy:SetImgHeight(enemies[name].img_height);
	enemy:SetMovementSpeed(enemies[name].movement_speed);
	enemy:LoadStandardAnimations(enemies[name].standard_animations);

	return enemy;
end
