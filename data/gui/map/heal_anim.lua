-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {
    -- The file to load the frames from
    image_filename = "data/gui/map/heal.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 1,
    columns = 1,
    -- set the image dimensions (in pixels)
    frame_width = 36.0,
    frame_height = 34.0,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 150, x_offset = 0.0, y_offset = 0.0 },
        [1] = { id = 0, duration = 150, x_offset = 0.0, y_offset = 1.0 },
        [2] = { id = 0, duration = 150, x_offset = 0.0, y_offset = 2.0 },
        [3] = { id = 0, duration = 150, x_offset = 0.0, y_offset = 1.0 },
    }
}
