-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "data/entities/battle/effects/sword_slash.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 2,
    columns = 2,
    -- set the image dimensions on battles (in pixels)
    --frame_width = 128.0,
    --frame_height = 128.0,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 3, duration = 50 },
        [1] = { id = 2, duration = 50 },
        [2] = { id = 1, duration = 50 },
        [3] = { id = 0, duration = 0 }, -- 0 means forever
    }
}
