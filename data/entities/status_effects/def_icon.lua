-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "data/entities/status_effects/status.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 16,
    columns = 9,
    -- set the image dimensions (in pixels)
    frame_width = 225.0 / 9,
    frame_height = 400.0 / 16,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 22, duration = 0 } -- forever
    }
}
