-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "data/entities/battle/effects/wolf_claws.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 2,
    columns = 2,
    -- set the image dimensions on battles (in pixels)
    frame_width = 108.0,
    frame_height = 108.0,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 40 },
        [1] = { id = 1, duration = 40 },
        [2] = { id = 2, duration = 40 },
        [3] = { id = 3, duration = 0 }, -- 0 means forever
    }
}
