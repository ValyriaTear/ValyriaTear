-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

-- This files is following a special format compared to other animation scripts.

local ANIM_SOUTH = vt_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = vt_map.MapMode.ANIM_NORTH;
local ANIM_WEST = vt_map.MapMode.ANIM_WEST;
local ANIM_EAST = vt_map.MapMode.ANIM_EAST;


sprite_animation = {

    -- The file to load the frames from
    image_filename = "img/sprites/map/enemies/andromalius_spritesheet.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 5,
    columns = 8,
    -- The frames duration in milliseconds
    frames = {
        [ANIM_SOUTH] = {
            [0] = { id = 20, duration = 150 },
            [1] = { id = 21, duration = 150 },
            [2] = { id = 22, duration = 150 },
            [3] = { id = 23, duration = 150 }
        },
        [ANIM_NORTH] = {
            [0] = { id = 20, duration = 150 },
            [1] = { id = 21, duration = 150 },
            [2] = { id = 22, duration = 150 },
            [3] = { id = 23, duration = 150 }
        },
        [ANIM_WEST] = {
            [0] = { id = 32, duration = 150 },
            [1] = { id = 33, duration = 150 },
            [2] = { id = 34, duration = 150 },
            [3] = { id = 35, duration = 150 },
            [4] = { id = 36, duration = 150 },
            [5] = { id = 37, duration = 150 },
            [6] = { id = 38, duration = 150 },
            [7] = { id = 39, duration = 150 },
        },
        [ANIM_EAST] = {
            [0] = { id = 0, duration = 150 },
            [1] = { id = 1, duration = 150 },
            [2] = { id = 2, duration = 150 },
            [3] = { id = 3, duration = 150 },
            [4] = { id = 4, duration = 150 },
            [5] = { id = 5, duration = 150 },
            [6] = { id = 6, duration = 150 },
            [7] = { id = 7, duration = 150 },
        }
    }
}
