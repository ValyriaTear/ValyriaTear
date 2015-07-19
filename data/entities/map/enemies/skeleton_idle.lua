-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

-- This files is following a special format compared to other animation scripts.

local ANIM_SOUTH = vt_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = vt_map.MapMode.ANIM_NORTH;
local ANIM_WEST = vt_map.MapMode.ANIM_WEST;
local ANIM_EAST = vt_map.MapMode.ANIM_EAST;


sprite_animation = {

    -- The file to load the frames from
    image_filename = "data/entities/map/enemies/skeleton_spritesheet.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 4,
    columns = 7,
    -- The actual frame size
    --frame_width = 77 / 1.5,
    --frame_height = 76 / 1.5,
    -- The frames duration in milliseconds
    frames = {
        [ANIM_SOUTH] = {
            [0] = { id = 0, duration = 0 } -- 0 means forever
        },
        [ANIM_NORTH] = {
            [0] = { id = 14, duration = 0 }
        },
        [ANIM_WEST] = {
            [0] = { id = 7, duration = 0 }
        },
        [ANIM_EAST] = {
            [0] = { id = 21, duration = 0 }
        }
    }
}
