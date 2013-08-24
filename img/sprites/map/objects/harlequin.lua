-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "img/sprites/battle/enemies/harlequin.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 1,
    columns = 1,
    -- set the image dimensions on the map (in pixels)
    frame_width = 79.0,
    frame_height = 94.0,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 180 },
        [1] = { id = 0, duration = 150, y_offset = -1.0 },
        [2] = { id = 0, duration = 120, y_offset = -2.0 },
        [3] = { id = 0, duration = 120, y_offset = -3.0 },
        [4] = { id = 0, duration = 150, y_offset = -4.0 },
        [5] = { id = 0, duration = 180, y_offset = -5.0 },
        [6] = { id = 0, duration = 150, y_offset = -4.0 },
        [7] = { id = 0, duration = 120, y_offset = -3.0 },
        [8] = { id = 0, duration = 120, y_offset = -2.0 },
        [9] = { id = 0, duration = 150, y_offset = -1.0 },
    }
}
