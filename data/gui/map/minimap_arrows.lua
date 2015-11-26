-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "data/gui/map/minimap_arrows.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 4,
    columns = 4,
    -- set the image dimensions (in pixels)
    frame_width = 35.0,
    frame_height = 35.0,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 2, duration = 0 },
        [1] = { id = 6, duration = 0 },
        [2] = { id = 10, duration = 0 },
        [3] = { id = 14, duration = 0 }
    }
}
