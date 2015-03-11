-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "img/sprites/map/objects/flame_pot1.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 1,
    columns = 5,
    -- set the image dimensions on the map (in pixels)
    frame_width = 32,
    frame_height = 32,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 150 },
        [1] = { id = 1, duration = 150 },
        [2] = { id = 2, duration = 150 },
        [3] = { id = 3, duration = 150 },
        [4] = { id = 1, duration = 150 },
        [5] = { id = 2, duration = 150 },
        [6] = { id = 4, duration = 150 }
    }
}
