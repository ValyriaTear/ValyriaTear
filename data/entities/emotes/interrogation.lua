-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "data/entities/emotes/dialogue_interrogation.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 3,
    columns = 2,
    -- set the image dimensions (in pixels)
    frame_width = 20.0,
    frame_height = 20.0,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 60 },
        [1] = { id = 1, duration = 60 },
        [2] = { id = 2, duration = 60 },
        [3] = { id = 3, duration = 60 },
        [4] = { id = 4, duration = 60 },
        [5] = { id = 5, duration = 500 } -- 0 means forever
    }
}
