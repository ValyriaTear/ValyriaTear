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
    face_portrait = "img/portraits/bronann.png",

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/characters/bronann_idle_unarmed.lua",
        walk = "img/sprites/map/characters/bronann_walk_unarmed.lua",
        run = "img/sprites/map/characters/bronann_run_unarmed.lua"
    },

    -- using standard (one direction only) animation files.
    custom_animations = {
        hero_stance = "img/sprites/map/characters/bronann_hero_stance_unarmed.lua",
        searching = "img/sprites/map/characters/bronann_searching_unarmed.lua",
        frightened = "img/sprites/map/characters/bronann_frightened_unarmed.lua",
    }
}

sprites["Kalya"] = {
	name = hoa_system.Translate("Kalya"),
	coll_half_width = 0.95,
	coll_height = 1.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,
	face_portrait = "img/portraits/kalya.png",

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/characters/kalya_idle.lua",
        walk = "img/sprites/map/characters/kalya_walk.lua",
        run = "img/sprites/map/characters/kalya_run.lua"
    }
}

sprites["Thanis"] = {
	name = hoa_system.Translate("Thanis"),
	coll_half_width = 0.95,
	coll_height = 1.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,
    face_portrait = "img/portraits/thanis.png",

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/characters/thanis_idle.lua",
        walk = "img/sprites/map/characters/thanis_walk.lua",
        run = "img/sprites/map/characters/thanis_run.lua"
    }

}

sprites["Carson"] = {
	name = hoa_system.Translate("Carson"),
	coll_half_width = 0.95,
	coll_height = 1.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,
    face_portrait = "img/portraits/npcs/carson.png",

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/story/carson_idle.lua",
        walk = "img/sprites/map/npcs/story/carson_walk.lua"
    }
}

sprites["Malta"] = {
	name = hoa_system.Translate("Malta"),
	coll_half_width = 0.95,
	coll_height = 1.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = VERY_SLOW_SPEED,
    face_portrait = "img/portraits/npcs/malta.png",

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/story/malta_idle.lua",
        walk = "img/sprites/map/npcs/story/malta_walk.lua"
    }
}

sprites["Orlinn"] = {
	name = hoa_system.Translate("Orlinn"),
	coll_half_width = 0.95,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = NORMAL_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/story/orlinn_idle.lua",
        walk = "img/sprites/map/npcs/story/orlinn_walk.lua"
    }
}

sprites["Herth"] = {
	name = hoa_system.Translate("Herth"),
	coll_half_width = 0.95,
	coll_height = 1.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/story/herth_idle.lua",
        walk = "img/sprites/map/npcs/story/herth_walk.lua"
    }
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

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/npc_girl01_idle.lua",
        walk = "img/sprites/map/npcs/npc_girl01_walk.lua"
    }
}

sprites["Old Woman1"] = {
	name = hoa_system.Translate("Brymir"), -- default name
	coll_half_width = 0.95,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/npc_old_woman01_idle.lua",
        walk = "img/sprites/map/npcs/npc_old_woman01_walk.lua"
    }
}

sprites["Woman1"] = {
	name = hoa_system.Translate("Martha"), -- default name
	coll_half_width = 0.95,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/npc_woman01_idle.lua",
        walk = "img/sprites/map/npcs/npc_woman01_walk.lua"
    }
}

sprites["Woman2"] = {
	name = hoa_system.Translate("Sophia"), -- default name
	coll_half_width = 1.0,
	coll_height = 2.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = NORMAL_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/npc_woman02_idle.lua",
        walk = "img/sprites/map/npcs/npc_woman02_walk.lua"
    }
}

sprites["Woman3"] = {
	name = hoa_system.Translate("Lilly"), -- default name
	coll_half_width = 1.0,
	coll_height = 2.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = NORMAL_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/npc_woman03_idle.lua",
        walk = "img/sprites/map/npcs/npc_woman03_walk.lua"
    }
}

sprites["Man1"] = {
	name = hoa_system.Translate("Georges"), -- default name
	coll_half_width = 0.95,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/npc_man01_idle.lua",
        walk = "img/sprites/map/npcs/npc_man01_walk.lua"
    }
}

sprites["Soldier"] = {
	name = hoa_system.Translate("Soldier"),
	coll_half_width = 0.95,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/npc_soldier01_idle.lua",
        walk = "img/sprites/map/npcs/npc_soldier01_walk.lua"
    }
}

sprites["Lord"] = {
	name = hoa_system.Translate("Lord"),
	coll_half_width = 0.95,
	coll_height = 1.9,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/npc_soldier02_idle.lua",
        walk = "img/sprites/map/npcs/npc_soldier02_walk.lua"
    }
}

-- ---------
-- Scening - Animals, ect ...
-- ---------
sprites["Chicken"] = {
	name = hoa_system.Translate("Chicken"),
	coll_half_width = 0.95,
	coll_height = 0.8,
	img_half_width = 1.0,
	img_height = 2.0,
	movement_speed = VERY_SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/scening/chicken_idle.lua",
        walk = "img/sprites/map/scening/chicken_walk.lua"
    }
}

sprites["Butterfly"] = {
	name = hoa_system.Translate("Butterfly"),
	coll_half_width = 0.65,
	coll_height = 0.7,
	img_half_width = 1.0,
	img_height = 2.0,
	movement_speed = NORMAL_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/scening/butterfly_idle.lua",
        walk = "img/sprites/map/scening/butterfly_walk.lua"
    }
}

sprites["Squirrel"] = {
	name = hoa_system.Translate("Squirrel"),
	coll_half_width = 0.65,
	coll_height = 0.8,
	img_half_width = 1.0,
	img_height = 2.0,
	movement_speed = NORMAL_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/scening/squirrel_idle.lua",
        walk = "img/sprites/map/scening/squirrel_walk.lua"
    }
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
    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/enemies/slime_idle.lua",
        walk = "img/sprites/map/enemies/slime_walk.lua"
    }
}


enemies["snake"] = {
	coll_half_width = 1.0,
	coll_height = 2.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = NORMAL_SPEED,
    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/enemies/snake_idle.lua",
        walk = "img/sprites/map/enemies/snake_walk.lua"
    }
}


enemies["scorpion"] = {
	coll_half_width = 1.0,
	coll_height = 2.0,
	img_half_width = 1.0,
	img_height = 4.0,
	movement_speed = NORMAL_SPEED,
    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/enemies/scorpion_idle.lua",
        walk = "img/sprites/map/enemies/scorpion_walk.lua"
    }
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
	sprite:SetSpriteName(name); -- The catalog reference
	sprite:SetObjectID(Map.object_supervisor:GenerateObjectID());
	sprite:SetContext(hoa_map.MapMode.CONTEXT_01);
	sprite:SetPosition(x, y);
	sprite:SetCollHalfWidth(sprites[name].coll_half_width);
	sprite:SetCollHeight(sprites[name].coll_height);
	sprite:SetImgHalfWidth(sprites[name].img_half_width);
	sprite:SetImgHeight(sprites[name].img_height);
	sprite:SetMovementSpeed(sprites[name].movement_speed);
	sprite:SetDirection(dir);
	if (sprites[name].face_portrait) then
		sprite:LoadFacePortrait(sprites[name].face_portrait);
	end

	sprite:LoadStandingAnimations(sprites[name].standard_animations.idle);
	sprite:LoadWalkingAnimations(sprites[name].standard_animations.walk);
	if (sprites[name].standard_animations.run ~= nil) then
        sprite:LoadRunningAnimations(sprites[name].standard_animations.run);
	end
	-- Load custom animations
	if (sprites[name].custom_animations ~= nil) then
		for name,file in pairs(sprites[name].custom_animations) do
			sprite:LoadCustomAnimation(name, file);
		end
	end

    return sprite;
end

-- Used to reload (or change) the graphic animations of an existing sprite.
function ReloadSprite(sprite_ref, name)
	if (sprites[name] == nil) then
		print("Error: No object named: "..name.." found!!");
		return;
	end

	if (sprite_ref == nil) then
		print("Error: Function called with invalid sprite object");
		return;
	end

	sprite_ref:SetName(sprites[name].name);
	sprite_ref:SetSpriteName(name); -- The catalog reference
	sprite_ref:SetCollHalfWidth(sprites[name].coll_half_width);
	sprite_ref:SetCollHeight(sprites[name].coll_height);
	sprite_ref:SetImgHalfWidth(sprites[name].img_half_width);
	sprite_ref:SetImgHeight(sprites[name].img_height);
	if (sprites[name].face_portrait) then
		sprite_ref:LoadFacePortrait(sprites[name].face_portrait);
	end

	sprite_ref:ClearAnimations();
	sprite_ref:LoadStandingAnimations(sprites[name].standard_animations.idle);
	sprite_ref:LoadWalkingAnimations(sprites[name].standard_animations.walk);
	if (sprites[name].standard_animations.run ~= nil) then
		sprite_ref:LoadRunningAnimations(sprites[name].standard_animations.run);
	end
	-- Load custom animations
	if (sprites[name].custom_animations ~= nil) then
		for name,file in pairs(sprites[name].custom_animations) do
			sprite_ref:LoadCustomAnimation(name, file);
		end
	end
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
	sprite:SetSpriteName(name); -- The catalog reference
	sprite:SetObjectID(Map.object_supervisor:GenerateObjectID());
	sprite:SetContext(hoa_map.MapMode.CONTEXT_01);
	sprite:SetPosition(x, y);
	sprite:SetCollHalfWidth(sprites[name].coll_half_width);
	sprite:SetCollHeight(sprites[name].coll_height);
	sprite:SetImgHalfWidth(sprites[name].img_half_width);
	sprite:SetImgHeight(sprites[name].img_height);
	sprite:SetMovementSpeed(sprites[name].movement_speed);
	sprite:SetDirection(dir);
	if (sprites[name].face_portrait) then
		sprite:LoadFacePortrait(sprites[name].face_portrait);
	end

	sprite:LoadStandingAnimations(sprites[name].standard_animations.idle);
	sprite:LoadWalkingAnimations(sprites[name].standard_animations.walk);
	if (sprites[name].standard_animations.run ~= nil) then
        sprite:LoadRunningAnimations(sprites[name].standard_animations.run);
	end
	-- Load custom animations
	if (sprites[name].custom_animations ~= nil) then
		for name,file in pairs(sprites[name].custom_animations) do
			sprite:LoadCustomAnimation(name, file);
		end
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
	enemy:SetSpriteName(name); -- The catalog reference
	enemy:SetObjectID(Map.object_supervisor:GenerateObjectID());
	enemy:SetContext(hoa_map.MapMode.CONTEXT_01);
	enemy:SetCollHalfWidth(enemies[name].coll_half_width);
	enemy:SetCollHeight(enemies[name].coll_height);
	enemy:SetImgHalfWidth(enemies[name].img_half_width);
	enemy:SetImgHeight(enemies[name].img_height);
	enemy:SetMovementSpeed(enemies[name].movement_speed);

	enemy:LoadStandingAnimations(enemies[name].standard_animations.idle);
	enemy:LoadWalkingAnimations(enemies[name].standard_animations.walk);

	return enemy;
end
