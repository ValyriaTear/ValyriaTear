-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {
    -- The file to load the frames from
    image_filename = "img/sprites/map/treasures/chest2.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 1,
    columns = 2,
    -- set the image dimensions on the map (in pixels)
    frame_width = 32,
    frame_height = 50,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 1, duration = 0 } -- 0 means forever
    }
}
