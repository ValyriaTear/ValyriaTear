-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

-- This files is following a special format compared to other animation scripts.

local ANIM_SOUTH = vt_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = vt_map.MapMode.ANIM_NORTH;
local ANIM_WEST = vt_map.MapMode.ANIM_WEST;
local ANIM_EAST = vt_map.MapMode.ANIM_EAST;

sprite_animation = {

    -- The file to load the frames from
    image_filename = "img/sprites/map/enemies/skeleton_spritesheet.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 7,
    columns = 4,
    -- The actual frame size
    --frame_width = 77 / 1.5,
    --frame_height = 76 / 1.5,
    -- The frames duration in milliseconds
    frames = {
        [ANIM_SOUTH] = {
            [0] = { id = 1, duration = 75 },
            [1] = { id = 2, duration = 75 },
            [2] = { id = 3, duration = 75 },
            [3] = { id = 4, duration = 75 },
            [4] = { id = 5, duration = 75 },
            [5] = { id = 6, duration = 75 },
        },
        [ANIM_NORTH] = {
            [0] = { id = 15, duration = 75 },
            [1] = { id = 16, duration = 75 },
            [2] = { id = 17, duration = 75 },
            [3] = { id = 18, duration = 75 },
            [4] = { id = 19, duration = 75 },
            [5] = { id = 20, duration = 75 },
        },
        [ANIM_WEST] = {
            [0] = { id = 8, duration = 75 },
            [1] = { id = 9, duration = 75 },
            [2] = { id = 10, duration = 75 },
            [3] = { id = 11, duration = 75 },
            [4] = { id = 12, duration = 75 },
            [5] = { id = 13, duration = 75 },
        },
        [ANIM_EAST] = {
            [0] = { id = 22, duration = 75 },
            [1] = { id = 23, duration = 75 },
            [2] = { id = 24, duration = 75 },
            [3] = { id = 25, duration = 75 },
            [4] = { id = 26, duration = 75 },
            [5] = { id = 27, duration = 75 },
        }
    }
}
