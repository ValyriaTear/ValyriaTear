-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "img/sprites/map/objects/campfire.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 1,
    columns = 5,
    -- set the image dimensions on the map (in pixels)
    frame_width = 64,
    frame_height = 64,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 170 },
        [1] = { id = 1, duration = 170 },
        [2] = { id = 2, duration = 170 },
        [3] = { id = 3, duration = 170 },
        [4] = { id = 4, duration = 170 }
    }
}
