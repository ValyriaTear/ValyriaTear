-- Describes all the skills and other improvements in the skill graph the characters can get.
-- Each entries are nodes that are linked to a list of other nodes, permiting to set paths between them.
-- Cross a path costs experience points, items, or both.
-- And also the character's start location.

skill_graph_start = {
    -- Character id; node id
    [BRONANN] = 0,
    [KALYA] = 0,
    [SYLVE] = 0,
    [THANIS] = 0,
}


-- FIXME: This is the first skill graph prototype
skill_graph = {
    -- node id
    [0] = { -- Grqph center for Bronann
        -- X location
        x_location = 0,
        -- Y location
        y_location = 0,
        -- icon filename,
        icon_file = "data/gui/menus/socket.lua",
        -- Experience points cost needed to reach this node
        experience_points_needed = 0,
        -- links with other nodes
        links = {
            1,4,
        },
    },
    -- node id
    [1] = {
        -- X location
        x_location = 50,
        -- Y location
        y_location = 30,
        -- icon filename,
        icon_file = "data/entities/status_effects/hp_icon.lua",
        -- Experience points cost needed to reach this node
        experience_points_needed = 20,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- +5 HP
            [6] = 5,
        },
        -- links with other nodes
        links = {
            2,
        },
    },
    -- node id
    [2] = {
        -- X location
        x_location = 100,
        -- Y location
        y_location = 50,
        -- icon filename,
        icon_file = "data/entities/status_effects/sp_icon.lua",
        -- Experience points cost needed to reach this node
        experience_points_needed = 10,
        -- stats modifier
        stats = {
            -- +1 SP
            [7] = 1,
        },
        -- links with other nodes
        links = {
            3,
        },
    },
    -- node id
    [3] = {
        -- X location
        x_location = 150,
        -- Y location
        y_location = 30,
        -- icon filename,
        icon_file = "data/gui/menus/star.lua",
        -- Experience points cost needed to reach this node
        experience_points_needed = 30,
        -- Skill earned
        skill_id_learned = 2,
        -- links with other nodes
        links = {
            --5,
        },
    },
    -- node id
    [4] = {
        -- X location
        x_location = -50,
        -- Y location
        y_location = 50,
        -- icon filename,
        icon_file = "data/entities/status_effects/stamina_icon.lua",
        -- Experience points cost needed to reach this node
        experience_points_needed = 20,
        -- stats modifier
        stats = {
            -- +1 Stamina
            [4] = 1
        },
        -- links with other nodes
        links = {
            5,
        },
    },
    -- node id
    [5] = {
        -- X location
        x_location = -90,
        -- Y location
        y_location = 70,
        -- icon filename,
        icon_file = "data/entities/status_effects/atk_icon.lua",
        -- Experience points cost needed to reach this node
        experience_points_needed = 20,
        -- stats modifier
        stats = {
            -- Phys. Atk +2
            [0] = 2,
        },
        -- links with other nodes
        links = {
            6,
        },
    },
    -- node id
    [6] = {
        -- X location
        x_location = -140,
        -- Y location
        y_location = 80,
        -- icon filename,
        icon_file = "data/entities/status_effects/def_icon.lua",
        -- Experience points cost needed to reach this node
        experience_points_needed = 20,
        -- stats modifier
        stats = {
            -- Def +2
            [2] = 2,
        },
        -- links with other nodes
        links = {
            7,
        },
    },
    -- node id
    [7] = {
        -- X location
        x_location = -190,
        -- Y location
        y_location = 70,
        -- icon filename,
        icon_file = "data/entities/status_effects/mdef_icon.lua",
        -- Experience points cost needed to reach this node
        experience_points_needed = 20,
        -- stats modifier
        stats = {
            -- M.def +1
            [3] = 1,
        },
        -- links with other nodes
        links = {
            8,
        },
    },
    -- node id
    [8] = {
        -- X location
        x_location = -230,
        -- Y location
        y_location = 40,
        -- icon filename,
        icon_file = "data/entities/status_effects/matk_icon.lua",
        -- Experience points cost needed to reach this node
        experience_points_needed = 20,
        -- stats modifier
        stats = {
            -- M.atk +1
            [1] = 1,
        },
        -- links with other nodes
        links = {
            8,
        },
    },

}
