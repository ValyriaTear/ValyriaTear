-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "data/entities/map/escape_point/escape_point_active.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 2,
    columns = 5,
    -- set the image dimensions (in pixels)
    frame_width = 64.0,
    frame_height = 54.0,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 2000 },
        [1] = { id = 1, duration = 100 },
        [2] = { id = 2, duration = 100 },
        [3] = { id = 3, duration = 100 },
        [4] = { id = 4, duration = 100 },
        [5] = { id = 5, duration = 100 },
        [6] = { id = 6, duration = 100 },
        [7] = { id = 7, duration = 100 },
        [8] = { id = 8, duration = 100 },
        [9] = { id = 9, duration = 2000 }
    }
}
