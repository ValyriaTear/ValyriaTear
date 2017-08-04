-- Describes all the skills and other improvements in the skill graph the characters can get.
-- Each entries are nodes that are linked to a list of other nodes, permiting to set paths between them.
-- Cross a path costs experience points, items, or both.
-- And also the character's start location.

skill_graph_start = {
    -- Character id; node id
    [BRONANN] = 0,
    [KALYA] = 1,
    [SYLVE] = 2,
    [THANIS] = 3,
}


-- FIXME: This is a test skill graph
skill_graph = {
    -- node id
    [0] = {
        -- X location
        x_location = 300,
        -- Y location
        y_location = 100,
        -- icon filename,
        icon_file = "data/gui/menus/socket.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 0,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- No stats improvement
        },
        -- links with other nodes
        links = {
            1,
        },
    },
    -- node id
    [1] = {
        -- X location
        x_location = 350,
        -- Y location
        y_location = 120,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            2, 3,
        },
    },
    -- node id
    [2] = {
        -- X location
        x_location = 380,
        -- Y location
        y_location = 60,
        -- icon filename,
        icon_file = "data/gui/menus/key.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 2,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- Skill earned
        skill_id_learned = 10001,
        -- stats modifier
        stats = {
            [3] = 3,
            [4] = 2,
            [5] = 1,
        },
        -- links with other nodes
        links = {
            3,
        },
    },
    -- node id
    [3] = {
        -- X location
        x_location = 420,
        -- Y location
        y_location = 60,
        -- icon filename,
        icon_file = "data/gui/menus/key.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 2,
        -- items needed to reach this node
        items_needed = {
            -- item id; number
            [1] = 10,
        },
        -- Skill earned
        skill_id_learned = 10002,
        -- stats modifier
        stats = {
            [3] = 2,
            [4] = 5,
            [5] = 2,
            [6] = 5,
            [7] = 4,
        },
        -- links with other nodes
        links = {
            4,
        },
    },
    -- node id
    [4] = {
        -- X location
        x_location = 450,
        -- Y location
        y_location = 120,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            5,
        },
    },
    -- node id
    [5] = {
        -- X location
        x_location = 480,
        -- Y location
        y_location = 120,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            6,
        },
    },
    -- node id
    [6] = {
        -- X location
        x_location = 510,
        -- Y location
        y_location = 120,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            7, 14,
        },
    },
    -- node id
    [7] = {
        -- X location
        x_location = 540,
        -- Y location
        y_location = 120,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            8,
        },
    },
    -- node id
    [8] = {
        -- X location
        x_location = 580,
        -- Y location
        y_location = 120,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            9, 10,
        },
    },
    -- node id
    [9] = {
        -- X location
        x_location = 600,
        -- Y location
        y_location = 120,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            10,
        },
    },
    -- node id
    [10] = {
        -- X location
        x_location = 600,
        -- Y location
        y_location = 60,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            11,
        },
    },
    -- node id
    [11] = {
        -- X location
        x_location = 650,
        -- Y location
        y_location = 60,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            12,
        },
    },
    -- node id
    [12] = {
        -- X location
        x_location = 700,
        -- Y location
        y_location = 80,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            13,
        },
    },
    -- node id
    [13] = {
        -- X location
        x_location = 720,
        -- Y location
        y_location = 120,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            -- end of path
        },
    },
    -- node id
    [14] = {
        -- X location
        x_location = 510,
        -- Y location
        y_location = 140,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            15,
        },
    },
    -- node id
    [15] = {
        -- X location
        x_location = 520,
        -- Y location
        y_location = 250,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            16, 1,
        },
    },
    -- node id
    [16] = {
        -- X location
        x_location = 520,
        -- Y location
        y_location = 300,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            17,
        },
    },
    [17] = {
        -- X location
        x_location = 520,
        -- Y location
        y_location = 380,
        -- icon filename,
        icon_file = "data/gui/menus/star.png",
        -- Experience points cost needed to reach this node
        experience_points_needed = 1,
        -- items needed to reach this node
        items_needed = {
            -- no items
        },
        -- no new skill earned
        skill_id_learned = -1,
        -- stats modifier
        stats = {
            -- Phys. Atk +1
            [0] = 1,
            [1] = 2,
            [2] = 2,
        },
        -- links with other nodes
        links = {
            -- end of path
        },
    },
}
