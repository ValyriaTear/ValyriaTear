-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "data/entities/map/objects/oil_lamp.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 1,
    columns = 1,
    -- set the image dimensions on the map (in pixels)
    frame_width = 20,
    frame_height = 29,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 0 } -- 0 means infinite
    }
}
