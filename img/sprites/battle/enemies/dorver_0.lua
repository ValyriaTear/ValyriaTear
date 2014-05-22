-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "img/sprites/battle/enemies/dorver.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 2,
    columns = 2,
    -- set the image dimensions on battles (in pixels)
    frame_width = 145.0 * 2,
    frame_height = 110.0 * 2,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 2, duration = 800, x_offset = 40, y_offset = 20 },
        -- FIXME: the offsets don't work when frames are reused... #311
        --[1] = { id = 3, duration = 200, x_offset = 40, y_offset = 20 }, 
        --[2] = { id = 0, duration = 200, x_offset = 40, y_offset = 20 },
        --[3] = { id = 1, duration = 200, x_offset = 40, y_offset = 20 },
        --[4] = { id = 0, duration = 200, x_offset = 40, y_offset = 20 },
        --[5] = { id = 3, duration = 200, x_offset = 40, y_offset = 20 },
    }
}
