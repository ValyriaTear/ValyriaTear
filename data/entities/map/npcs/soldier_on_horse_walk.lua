-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

-- This files is following a special format compared to other animation scripts.

local ANIM_SOUTH = vt_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = vt_map.MapMode.ANIM_NORTH;
local ANIM_WEST = vt_map.MapMode.ANIM_WEST;
local ANIM_EAST = vt_map.MapMode.ANIM_EAST;

sprite_animation = {

    -- The file to load the frames from
    image_filename = "data/entities/map/npcs/soldier_on_horse.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 4,
    columns = 4,
    -- The frames duration in milliseconds
    frames = {
        [ANIM_SOUTH] = {
            [0] = { id = 0, duration = 110 },
            [1] = { id = 1, duration = 110 },
            [2] = { id = 2, duration = 110 },
            [3] = { id = 3, duration = 110 },
            [4] = { id = 4, duration = 110 },
            [5] = { id = 5, duration = 110 },
            [6] = { id = 6, duration = 110 },
            [7] = { id = 7, duration = 110 },
        },
        [ANIM_NORTH] = {
            [0] = { id = 8, duration = 110 },
            [1] = { id = 9, duration = 110 },
            [2] = { id = 10, duration = 110 },
            [3] = { id = 11, duration = 110 },
            [4] = { id = 12, duration = 110 },
            [5] = { id = 13, duration = 110 },
            [6] = { id = 14, duration = 110 },
            [7] = { id = 15, duration = 110 },
        },
        [ANIM_WEST] = {
            [0] = { id = 0, duration = 110 },
            [1] = { id = 1, duration = 110 },
            [2] = { id = 2, duration = 110 },
            [3] = { id = 3, duration = 110 },
            [4] = { id = 4, duration = 110 },
            [5] = { id = 5, duration = 110 },
            [6] = { id = 6, duration = 110 },
            [7] = { id = 7, duration = 110 },
        },
        [ANIM_EAST] = {
            [0] = { id = 8, duration = 110 },
            [1] = { id = 9, duration = 110 },
            [2] = { id = 10, duration = 110 },
            [3] = { id = 11, duration = 110 },
            [4] = { id = 12, duration = 110 },
            [5] = { id = 13, duration = 110 },
            [6] = { id = 14, duration = 110 },
            [7] = { id = 15, duration = 110 },
        }
    }
}
