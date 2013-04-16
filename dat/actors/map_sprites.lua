sprites = {}

local NORMAL_SPEED = vt_map.MapMode.NORMAL_SPEED;
local SLOW_SPEED = vt_map.MapMode.SLOW_SPEED;
local VERY_SLOW_SPEED = vt_map.MapMode.VERY_SLOW_SPEED;
local VERY_FAST_SPEED = vt_map.MapMode.VERY_FAST_SPEED;

local ENEMY_SPEED = vt_map.MapMode.ENEMY_SPEED;

sprites["Bronann"] = {
    name = vt_system.Translate("Bronann"),
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
        frightened_fixed = "img/sprites/map/characters/bronann_frightened_unarmed_fixed.lua",
        hurt = "img/sprites/map/characters/bronann_hurt_unarmed.lua",
        sleeping = "img/sprites/map/characters/bronann_dead.lua",
        kneeling = "img/sprites/map/characters/bronann_kneeling.lua"
    }
}

sprites["Kalya"] = {
    name = vt_system.Translate("Kalya"),
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
    },

    -- using standard (one direction only) animation files.
    custom_animations = {
        kneeling = "img/sprites/map/characters/kalya_kneeling.lua",
        struggling = "img/sprites/map/characters/kalya_struggling.lua"
    }
}

sprites["Thanis"] = {
    name = vt_system.Translate("Thanis"),
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
    name = vt_system.Translate("Carson"),
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
    name = vt_system.Translate("Malta"),
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
    name = vt_system.Translate("Orlinn"),
    coll_half_width = 0.95,
    coll_height = 1.0,
    img_half_width = 1.0,
    img_height = 3.0,
    movement_speed = NORMAL_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/story/orlinn_idle.lua",
        walk = "img/sprites/map/npcs/story/orlinn_walk.lua"
    }
}

sprites["Herth"] = {
    name = vt_system.Translate("Herth"),
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
    name = vt_system.Translate("Olivia"), -- default name
    coll_half_width = 0.95,
    coll_height = 1.0,
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
    name = vt_system.Translate("Brymir"), -- default name
    coll_half_width = 0.95,
    coll_height = 1.3,
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
    name = vt_system.Translate("Martha"), -- default name
    coll_half_width = 0.95,
    coll_height = 1.1,
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
    name = vt_system.Translate("Sophia"), -- default name
    coll_half_width = 1.0,
    coll_height = 1.1,
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
    name = vt_system.Translate("Lilly"), -- default name
    coll_half_width = 1.0,
    coll_height = 1.3,
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
    name = vt_system.Translate("Georges"), -- default name
    coll_half_width = 0.95,
    coll_height = 1.0,
    img_half_width = 1.0,
    img_height = 4.0,
    movement_speed = SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/npc_man01_idle.lua",
        walk = "img/sprites/map/npcs/npc_man01_walk.lua"
    }
}

-- Used as a NPC to get portrait support
sprites["Crystal"] = {
    name = vt_system.Translate("Crystal"), -- default name
    coll_half_width = 0.95,
    coll_height = 1.9,
    img_half_width = 1.0,
    img_height = 4.0,
    movement_speed = SLOW_SPEED,
    face_portrait = "img/backdrops/boot/crystal.png",

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/crystal_idle.lua",
        walk = "img/sprites/map/npcs/crystal_idle.lua"
    }
}

sprites["Soldier"] = {
    name = vt_system.Translate("Soldier"),
    coll_half_width = 0.95,
    coll_height = 1.2,
    img_half_width = 1.0,
    img_height = 4.0,
    movement_speed = SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/npcs/npc_soldier01_idle.lua",
        walk = "img/sprites/map/npcs/npc_soldier01_walk.lua"
    }
}

-- ---------
-- Scening - Animals, ect ...
-- ---------
sprites["Chicken"] = {
    name = vt_system.Translate("Chicken"),
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
    name = vt_system.Translate("Butterfly"),
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
    name = vt_system.Translate("Squirrel"),
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
sprites["bat"] = {
    name = vt_system.Translate("Bat"),
    coll_half_width = 1.0,
    coll_height = 2.0,
    img_half_width = 1.0,
    img_height = 2.0,
    movement_speed = ENEMY_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/enemies/bat_idle.lua",
        walk = "img/sprites/map/enemies/bat_walk.lua"
    }
}

sprites["slime"] = {
    coll_half_width = 0.6,
    coll_height = 1.0,
    img_half_width = 0.6,
    img_height = 1.0,
    movement_speed = ENEMY_SPEED,
    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/enemies/slime_idle.lua",
        walk = "img/sprites/map/enemies/slime_walk.lua"
    }
}

sprites["spider"] = {
    coll_half_width = 0.9,
    coll_height = 1.8,
    img_half_width = 1.0,
    img_height = 2.0,
    movement_speed = ENEMY_SPEED,
    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/enemies/spider_idle.lua",
        walk = "img/sprites/map/enemies/spider_walk.lua"
    }
}

sprites["big slime"] = {
    name = vt_system.Translate("Slime Mother"),
    coll_half_width = 1.5,
    coll_height = 3.0,
    img_half_width = 1.5,
    img_height = 3.0,
    movement_speed = ENEMY_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/enemies/big_slime_idle.lua",
        walk = "img/sprites/map/enemies/big_slime_walk.lua"
    }
}

sprites["snake"] = {
    coll_half_width = 1.0,
    coll_height = 2.0,
    img_half_width = 1.0,
    img_height = 4.0,
    movement_speed = ENEMY_SPEED,
    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/enemies/snake_idle.lua",
        walk = "img/sprites/map/enemies/snake_walk.lua"
    }
}


sprites["scorpion"] = {
    coll_half_width = 1.0,
    coll_height = 2.0,
    img_half_width = 1.0,
    img_height = 4.0,
    movement_speed = ENEMY_SPEED,
    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/enemies/scorpion_idle.lua",
        walk = "img/sprites/map/enemies/scorpion_walk.lua"
    }
}

sprites["Fenrir"] = {
    name = vt_system.Translate("Fenrir"),
    coll_half_width = 1.5,
    coll_height = 3.0,
    img_half_width = 1.5,
    img_height = 3.0,
    movement_speed = ENEMY_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/enemies/fenrir_idle.lua",
        walk = "img/sprites/map/enemies/fenrir_walk.lua"
    }
}

sprites["Dark Soldier"] = {
    name = vt_system.Translate("Dark Soldier"),
    coll_half_width = 0.95,
    coll_height = 1.2,
    img_half_width = 1.0,
    img_height = 4.0,
    movement_speed = SLOW_SPEED,
    face_portrait = "img/portraits/npcs/dark_soldier.png",

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/enemies/dark_soldier01_idle.lua",
        walk = "img/sprites/map/enemies/dark_soldier01_walk.lua"
    }
}

sprites["Lord"] = {
    name = vt_system.Translate("Lord"),
    coll_half_width = 0.95,
    coll_height = 1.2,
    img_half_width = 1.0,
    img_height = 4.0,
    movement_speed = SLOW_SPEED,
    face_portrait = "img/portraits/npcs/lord_banesore.png",

    -- using special animation files.
    standard_animations = {
        idle = "img/sprites/map/enemies/lord_idle.lua",
        walk = "img/sprites/map/enemies/lord_walk.lua"
    }
}



function CreateSprite(Map, name, x, y)
    if (sprites[name] == nil) then
        print("Error: No object named: "..name.." found!!");
        return nil;
    end

    if (Map == nil) then
        print("Error: Function called with invalid Map object");
        return nil;
    end

    dir = (2 ^ math.random(0, 3));

    local sprite = {}
    sprite = vt_map.MapSprite();
    sprite:SetName(sprites[name].name);
    sprite:SetSpriteName(name); -- The catalog reference
    sprite:SetObjectID(Map.object_supervisor:GenerateObjectID());
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


-- Permit to setup a custom name
-- and reuse the npcs sprites more easily
function CreateNPCSprite(Map, name, npc_name, x, y)
    if (sprites[name] == nil) then
        print("Error: No object named: "..name.." found!!");
        return nil;
    end

    if (Map == nil) then
        print("Error: Function called with invalid Map object");
        return nil;
    end

    dir = (2 ^ math.random(0, 3));
    local sprite = {};
    sprite = vt_map.MapSprite();
    sprite:SetName(npc_name);
    sprite:SetSpriteName(name); -- The catalog reference
    sprite:SetObjectID(Map.object_supervisor:GenerateObjectID());
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



function CreateEnemySprite(Map, name)
    if (sprites[name] == nil) then
        print("Error: No object named: "..name.." found!!");
        return nil;
    end

    if (Map == nil) then
        print("Error: Function called with invalid Map object");
        return nil;
    end

    local enemy = {}
    enemy = vt_map.EnemySprite();
    enemy:SetSpriteName(name); -- The catalog reference
    enemy:SetObjectID(Map.object_supervisor:GenerateObjectID());
    enemy:SetCollHalfWidth(sprites[name].coll_half_width);
    enemy:SetCollHeight(sprites[name].coll_height);
    enemy:SetImgHalfWidth(sprites[name].img_half_width);
    enemy:SetImgHeight(sprites[name].img_height);
    enemy:SetMovementSpeed(sprites[name].movement_speed);

    enemy:LoadStandingAnimations(sprites[name].standard_animations.idle);
    enemy:LoadWalkingAnimations(sprites[name].standard_animations.walk);

    return enemy;
end
