sprites = {}

local NORMAL_SPEED = vt_map.MapMode.NORMAL_SPEED;
local SLOW_SPEED = vt_map.MapMode.SLOW_SPEED;
local VERY_SLOW_SPEED = vt_map.MapMode.VERY_SLOW_SPEED;
local VERY_FAST_SPEED = vt_map.MapMode.VERY_FAST_SPEED;

local ENEMY_SPEED = vt_map.MapMode.ENEMY_SPEED;

sprites["Bronann"] = {
    name = vt_system.Translate("Bronann"),
    coll_half_width = 0.95 * 16,
    coll_height = 1.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = NORMAL_SPEED,
    face_portrait = "data/entities/portraits/bronann.png",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/characters/bronann_idle_unarmed.lua",
        walk = "data/entities/map/characters/bronann_walk_unarmed.lua",
        run = "data/entities/map/characters/bronann_run_unarmed.lua"
    },

    -- using standard (one direction only) animation files.
    custom_animations = {
        hero_stance = "data/entities/map/characters/bronann_hero_stance_unarmed.lua",
        searching = "data/entities/map/characters/bronann_searching_unarmed.lua",
        frightened = "data/entities/map/characters/bronann_frightened_unarmed.lua",
        frightened_fixed = "data/entities/map/characters/bronann_frightened_unarmed_fixed.lua",
        hurt = "data/entities/map/characters/bronann_hurt_unarmed.lua",
        sleeping = "data/entities/map/characters/bronann_dead.lua",
        kneeling = "data/entities/map/characters/bronann_kneeling.lua",
        kneeling_left = "data/entities/map/characters/bronann_kneeling_left.lua",
        jump_south = "data/entities/map/characters/bronann_jump_south.lua",
        attack_south = "data/entities/map/characters/bronann_attack_south.lua",
        laughing = "data/entities/map/characters/bronann_laughing_unarmed.lua"
    }
}

-- Overworld sprite
sprites["Mini_Bronann"] = {
    name = vt_system.Translate("Bronann"),
    coll_half_width = 0.47 * 16,
    coll_height = 0.5 * 16,
    img_half_width = 0.5 * 16,
    img_height = 2.0 * 16,
    movement_speed = NORMAL_SPEED,
    face_portrait = "data/entities/portraits/bronann.png",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/characters/bronann_idle_overworld_unarmed.lua",
        walk = "data/entities/map/characters/bronann_walk_overworld_unarmed.lua"
    },
}

sprites["Kalya"] = {
    name = vt_system.Translate("Kalya"),
    coll_half_width = 0.95 * 16,
    coll_height = 1.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = SLOW_SPEED,
    face_portrait = "data/entities/portraits/kalya.png",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/characters/kalya_idle.lua",
        walk = "data/entities/map/characters/kalya_walk.lua",
        run = "data/entities/map/characters/kalya_run.lua"
    },

    -- using standard (one direction only) animation files.
    custom_animations = {
        kneeling = "data/entities/map/characters/kalya_kneeling.lua",
        struggling = "data/entities/map/characters/kalya_struggling.lua",
        jump_south = "data/entities/map/characters/kalya_jump_south.lua",
        laughing = "data/entities/map/characters/kalya_laughing.lua",
        hurt = "data/entities/map/characters/kalya_hurt.lua",
        frightened_fixed = "data/entities/map/characters/kalya_frightened_fixed.lua",
    }
}

sprites["Thanis"] = {
    name = vt_system.Translate("Thanis"),
    coll_half_width = 0.95 * 16,
    coll_height = 1.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = SLOW_SPEED,
    face_portrait = "data/entities/portraits/thanis.png",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/characters/thanis_idle.lua",
        walk = "data/entities/map/characters/thanis_walk.lua",
        run = "data/entities/map/characters/thanis_run.lua"
    }

}

sprites["Carson"] = {
    name = vt_system.Translate("Carson"),
    coll_half_width = 0.95 * 16,
    coll_height = 1.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = SLOW_SPEED,
    face_portrait = "data/entities/portraits/npcs/carson.png",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/story/carson_idle.lua",
        walk = "data/entities/map/npcs/story/carson_walk.lua"
    }
}

sprites["Malta"] = {
    name = vt_system.Translate("Malta"),
    coll_half_width = 0.95 * 16,
    coll_height = 1.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = VERY_SLOW_SPEED,
    face_portrait = "data/entities/portraits/npcs/malta.png",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/story/malta_idle.lua",
        walk = "data/entities/map/npcs/story/malta_walk.lua"
    }
}

sprites["Orlinn"] = {
    name = vt_system.Translate("Orlinn"),
    coll_half_width = 0.80 * 16.0,
    coll_height = 1.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 3.0 * 16,
    movement_speed = NORMAL_SPEED,
    face_portrait = "data/entities/portraits/npcs/orlinn.png",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/story/orlinn_idle.lua",
        walk = "data/entities/map/npcs/story/orlinn_walk.lua"
    },

    -- using standard (one direction only) animation files.
    custom_animations = {
        laughing = "data/entities/map/npcs/story/orlinn_laughing.lua",
        frightened_fixed = "data/entities/map/npcs/story/orlinn_frightened_fixed.lua",
    }
}

sprites["Herth"] = {
    name = vt_system.Translate("Herth"),
    coll_half_width = 0.95 * 16,
    coll_height = 1.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = SLOW_SPEED,
    face_portrait = "data/entities/portraits/npcs/herth.png",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/story/herth_idle.lua",
        walk = "data/entities/map/npcs/story/herth_walk.lua"
    }
}

-- ----
-- NPCs
-- ----

sprites["Girl1"] = {
    name = vt_system.Translate("Olivia"), -- default name
    coll_half_width = 0.95 * 16,
    coll_height = 1.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/npc_girl01_idle.lua",
        walk = "data/entities/map/npcs/npc_girl01_walk.lua"
    }
}

sprites["Old Woman1"] = {
    name = vt_system.Translate("Brymir"), -- default name
    coll_half_width = 0.95 * 16,
    coll_height = 1.3 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/npc_old_woman01_idle.lua",
        walk = "data/entities/map/npcs/npc_old_woman01_walk.lua"
    }
}

sprites["Woman1"] = {
    name = vt_system.Translate("Martha"), -- default name
    coll_half_width = 0.95 * 16,
    coll_height = 1.1 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/npc_woman01_idle.lua",
        walk = "data/entities/map/npcs/npc_woman01_walk.lua"
    }
}

sprites["Woman2"] = {
    name = vt_system.Translate("Sophia"), -- default name
    coll_half_width = 1.0 * 16,
    coll_height = 1.1 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = NORMAL_SPEED,
    face_portrait = "data/entities/portraits/npcs/woman02.png",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/npc_woman02_idle.lua",
        walk = "data/entities/map/npcs/npc_woman02_walk.lua"
    }
}

sprites["Woman3"] = {
    name = vt_system.Translate("Lilly"), -- default name
    coll_half_width = 1.0 * 16,
    coll_height = 1.3 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = NORMAL_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/npc_woman03_idle.lua",
        walk = "data/entities/map/npcs/npc_woman03_walk.lua"
    }
}

sprites["Man1"] = {
    name = vt_system.Translate("Georges"), -- default name
    coll_half_width = 0.95 * 16,
    coll_height = 1.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/npc_man01_idle.lua",
        walk = "data/entities/map/npcs/npc_man01_walk.lua"
    }
}

sprites["Shop Mushroom"] = {
    name = vt_system.Translate("Dandy Shroom"),
    coll_half_width = 0.9 * 16,
    coll_height = 1.9 * 16,
    img_half_width = 1.25 * 16,
    img_height = 2.6 * 16,
    movement_speed = ENEMY_SPEED,
    --face_portrait = "",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/friendly_mushroom_idle.lua",
        walk = "data/entities/map/npcs/friendly_mushroom_idle.lua"
    },
}

-- Used as a NPC to get portrait support
sprites["Crystal"] = {
    name = vt_system.Translate("Crystal"), -- default name
    coll_half_width = 0.95 * 16,
    coll_height = 1.9 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = SLOW_SPEED,
    face_portrait = "data/boot_menu/ep1/crystal.png",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/crystal_idle.lua",
        walk = "data/entities/map/npcs/crystal_idle.lua"
    }
}

sprites["Soldier"] = {
    name = vt_system.Translate("Soldier"),
    coll_half_width = 0.95 * 16,
    coll_height = 1.2 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/npc_soldier01_idle.lua",
        walk = "data/entities/map/npcs/npc_soldier01_walk.lua"
    }
}

-- ---------
-- Scening - Animals, ect ...
-- ---------
sprites["Chicken"] = {
    name = vt_system.Translate("Chicken"),
    coll_half_width = 0.95 * 16,
    coll_height = 0.8 * 16,
    img_half_width = 1.0 * 16,
    img_height = 2.0 * 16,
    movement_speed = VERY_SLOW_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/scening/chicken_idle.lua",
        walk = "data/entities/map/scening/chicken_walk.lua"
    }
}

sprites["Butterfly"] = {
    name = vt_system.Translate("Butterfly"),
    coll_half_width = 0.65 * 16,
    coll_height = 0.7 * 16,
    img_half_width = 1.0 * 16,
    img_height = 2.0 * 16,
    movement_speed = NORMAL_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/scening/butterfly_idle.lua",
        walk = "data/entities/map/scening/butterfly_walk.lua"
    }
}

sprites["Squirrel"] = {
    name = vt_system.Translate("Squirrel"),
    coll_half_width = 0.65 * 16,
    coll_height = 0.8 * 16,
    img_half_width = 1.0 * 16,
    img_height = 2.0 * 16,
    movement_speed = NORMAL_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/scening/squirrel_idle.lua",
        walk = "data/entities/map/scening/squirrel_walk.lua"
    }
}

-- ---------
-- Enemies
-- ---------
sprites["ratto"] = {
    name = vt_system.Translate("Ratto"),
    coll_half_width = 0.6 * 16,
    coll_height = 1.6 * 16,
    img_half_width = 1.0 * 16,
    img_height = 2.0 * 16,
    movement_speed = ENEMY_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/rat_idle.lua",
        walk = "data/entities/map/enemies/rat_walk.lua"
    }
}

sprites["bat"] = {
    name = vt_system.Translate("Bat"),
    coll_half_width = 0.6 * 16,
    coll_height = 1.6 * 16,
    img_half_width = 1.0 * 16,
    img_height = 2.0 * 16,
    movement_speed = ENEMY_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/bat_idle.lua",
        walk = "data/entities/map/enemies/bat_walk.lua"
    }
}

sprites["slime"] = {
    coll_half_width = 0.6 * 16,
    coll_height = 1.0 * 16,
    img_half_width = 0.6 * 16,
    img_height = 1.0 * 16,
    movement_speed = ENEMY_SPEED,
    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/slime_idle.lua",
        walk = "data/entities/map/enemies/slime_walk.lua"
    }
}

sprites["spider"] = {
    coll_half_width = 0.9 * 16,
    coll_height = 1.8 * 16,
    img_half_width = 1.0 * 16,
    img_height = 2.0 * 16,
    movement_speed = ENEMY_SPEED,
    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/spider_idle.lua",
        walk = "data/entities/map/enemies/spider_walk.lua"
    }
}

sprites["big slime"] = {
    name = vt_system.Translate("Slime Mother"),
    coll_half_width = 1.5 * 16,
    coll_height = 3.0 * 16,
    img_half_width = 1.5 * 16,
    img_height = 3.0 * 16,
    movement_speed = ENEMY_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/big_slime_idle.lua",
        walk = "data/entities/map/enemies/big_slime_walk.lua"
    }
}

sprites["snake"] = {
    coll_half_width = 1.0 * 16,
    coll_height = 2.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = ENEMY_SPEED,
    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/snake_idle.lua",
        walk = "data/entities/map/enemies/snake_walk.lua"
    }
}


sprites["scorpion"] = {
    coll_half_width = 1.0 * 16,
    coll_height = 2.0 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = ENEMY_SPEED,
    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/scorpion_idle.lua",
        walk = "data/entities/map/enemies/scorpion_walk.lua"
    }
}

sprites["Fenrir"] = {
    name = vt_system.Translate("Fenrir"),
    coll_half_width = 1.5 * 16,
    coll_height = 3.0 * 16,
    img_half_width = 1.5 * 16,
    img_height = 3.0 * 16,
    movement_speed = ENEMY_SPEED,

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/fenrir_idle.lua",
        walk = "data/entities/map/enemies/fenrir_walk.lua"
    }
}

sprites["Dark Soldier"] = {
    name = vt_system.Translate("Dark Soldier"),
    coll_half_width = 0.95 * 16,
    coll_height = 1.2 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = ENEMY_SPEED,
    face_portrait = "data/entities/portraits/npcs/dark_soldier.png",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/dark_soldier01_idle.lua",
        walk = "data/entities/map/enemies/dark_soldier01_walk.lua"
    },

    -- using standard (one direction only) animation files.
    custom_animations = {
        ko = "data/entities/map/enemies/dark_soldier_ko.lua",
    }
}

sprites["Lord"] = {
    name = vt_system.Translate("Lord"),
    coll_half_width = 0.95 * 16,
    coll_height = 1.2 * 16,
    img_half_width = 1.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = SLOW_SPEED,
    face_portrait = "data/entities/portraits/npcs/lord_banesore.png",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/lord_idle.lua",
        walk = "data/entities/map/enemies/lord_walk.lua"
    }
}

-- First actual Mt. Elbrus monster
sprites["Shroom"] = {
    name = vt_system.Translate("Shroom"),
    coll_half_width = 0.9 * 16,
    coll_height = 1.9 * 16,
    img_half_width = 1.25 * 16,
    img_height = 2.6 * 16,
    movement_speed = ENEMY_SPEED,
    --face_portrait = "",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/spiky_mushroom_idle.lua",
        walk = "data/entities/map/enemies/spiky_mushroom_walk.lua"
    },

    -- using standard (one direction only) animation files.
    custom_animations = {
        mushroom_ko = "data/entities/map/enemies/spiky_mushroom_dead.lua",
    }
}

sprites["Eyeball"] = {
    name = vt_system.Translate("Eyeball"),
    coll_half_width = 0.9 * 16,
    coll_height = 1.9 * 16,
    img_half_width = 1.25 * 16,
    img_height = 2.6 * 16,
    movement_speed = ENEMY_SPEED,
    --face_portrait = "",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/eyeball_idle.lua",
        walk = "data/entities/map/enemies/eyeball_walk.lua"
    },
}

sprites["Dark Soldier on horse"] = {
    name = vt_system.Translate("Dark Soldier"),
    coll_half_width = 3.0 * 16,
    coll_height = 3.2 * 16,
    img_half_width = 3.0 * 16,
    img_height = 6.0 * 16,
    movement_speed = ENEMY_SPEED,
    face_portrait = "data/entities/portraits/npcs/dark_soldier.png",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/npcs/soldier_on_horse_idle.lua",
        walk = "data/entities/map/npcs/soldier_on_horse_walk.lua"
    }
}

sprites["Beetle"] = {
    name = vt_system.Translate("Beetle"),
    coll_half_width = 0.6 * 16,
    coll_height = 1.2 * 16,
    img_half_width = 1.25 * 16,
    img_height = 2.6 * 16,
    movement_speed = ENEMY_SPEED,
    --face_portrait = "",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/beetle_idle.lua",
        walk = "data/entities/map/enemies/beetle_walk.lua"
    },
}

sprites["Skeleton"] = {
    name = vt_system.Translate("Skeleton"),
    coll_half_width = 0.6 * 16,
    coll_height = 1.2 * 16,
    img_half_width = 2.0 * 16,
    img_height = 4.0 * 16,
    movement_speed = ENEMY_SPEED,
    --face_portrait = "",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/skeleton_idle.lua",
        walk = "data/entities/map/enemies/skeleton_walk.lua"
    },
}

sprites["Dorver"] = {
    name = vt_system.Translate("Dorver"),
    coll_half_width = 3.20 * 16,
    coll_height = 2.0 * 16,
    img_half_width = 3.39 * 16,
    img_height = 5.145 * 16,
    movement_speed = 600000, -- Non-moving
    --face_portrait = "",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/dorver_idle.lua",
        walk = "data/entities/map/enemies/dorver_idle.lua"
    },
}

sprites["Andromalius"] = {
    name = vt_system.Translate("Andromalius"),
    coll_half_width = 1.781 * 16,
    coll_height = 3.0 * 16,
    img_half_width = 1.781 * 16,
    img_height = 5.5 * 16,
    movement_speed = ENEMY_SPEED,
    --face_portrait = "",

    -- using special animation files.
    standard_animations = {
        idle = "data/entities/map/enemies/andromalius_idle.lua",
        walk = "data/entities/map/enemies/andromalius_idle.lua"
    },

    -- using standard (one direction only) animation files.
    custom_animations = {
        open_mouth_right = "data/entities/map/enemies/andromalius_openmouth_right.lua",
        open_mouth_left = "data/entities/map/enemies/andromalius_openmouth_left.lua",
    }
}

function CreateSprite(Map, name, x, y, layer)
    if (sprites[name] == nil) then
        print("Error: No object named: "..name.." found!!");
        return nil;
    end

    if (Map == nil) then
        print("Error: Function called with invalid Map object");
        return nil;
    end

    -- Note: Auto-registered to the object supervisor.
    local sprite = vt_map.MapSprite.Create(layer);
    sprite:SetName(sprites[name].name);
    sprite:SetSpriteName(name); -- The catalog reference
    sprite:SetPosition(x, y);
    sprite:SetCollPixelHalfWidth(sprites[name].coll_half_width);
    sprite:SetCollPixelHeight(sprites[name].coll_height);
    sprite:SetImgPixelHalfWidth(sprites[name].img_half_width);
    sprite:SetImgPixelHeight(sprites[name].img_height);
    sprite:SetMovementSpeed(sprites[name].movement_speed);
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
        for animation_name,file in pairs(sprites[name].custom_animations) do
            sprite:LoadCustomAnimation(animation_name, file);
        end
    end

    return sprite;
end


-- Permit to setup a custom name
-- and reuse the npcs sprites more easily
function CreateNPCSprite(Map, name, npc_name, x, y, layer)
    if (sprites[name] == nil) then
        print("Error: No object named: "..name.." found!!");
        return nil;
    end

    if (Map == nil) then
        print("Error: Function called with invalid Map object");
        return nil;
    end

    -- Note: Auto-registered to the object supervisor.
    local sprite = vt_map.MapSprite.Create(layer);
    sprite:SetName(npc_name);
    sprite:SetSpriteName(name); -- The catalog reference
    sprite:SetPosition(x, y);
    sprite:SetCollPixelHalfWidth(sprites[name].coll_half_width);
    sprite:SetCollPixelHeight(sprites[name].coll_height);
    sprite:SetImgPixelHalfWidth(sprites[name].img_half_width);
    sprite:SetImgPixelHeight(sprites[name].img_height);
    sprite:SetMovementSpeed(sprites[name].movement_speed);

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
        for animation_name,file in pairs(sprites[name].custom_animations) do
            sprite:LoadCustomAnimation(animation_name, file);
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

    -- Note: Auto-registered to the object supervisor on the GROUND_LAYER.
    local enemy = vt_map.EnemySprite.Create();
    enemy:SetSpriteName(name); -- The catalog reference
    enemy:SetCollPixelHalfWidth(sprites[name].coll_half_width);
    enemy:SetCollPixelHeight(sprites[name].coll_height);
    enemy:SetImgPixelHalfWidth(sprites[name].img_half_width);
    enemy:SetImgPixelHeight(sprites[name].img_height);
    enemy:SetMovementSpeed(sprites[name].movement_speed);

    enemy:LoadStandingAnimations(sprites[name].standard_animations.idle);
    enemy:LoadWalkingAnimations(sprites[name].standard_animations.walk);

    return enemy;
end
