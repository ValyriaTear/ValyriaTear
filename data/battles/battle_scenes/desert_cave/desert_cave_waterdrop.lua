-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "data/battles/battle_scenes/desert_cave/desert_cave_waterdrop.png",
    -- Whether the animation frames are blended one with the next.
    blended_animation = true,
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 1,
    columns = 6,
    -- set the image dimensions on battles (in pixels)
    frame_width = 37.0,
    frame_height = 137.0,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 4000 },
        [1] = { id = 1, duration = 3000 },
        [2] = { id = 2, duration = 150 },
        [3] = { id = 3, duration = 9 },
        [4] = { id = 4, duration = 75 },
        [5] = { id = 5, duration = 75 }
    }
}
