-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "img/sprites/map/enemies/andromalius_spritesheet.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 5,
    columns = 8,
    -- set the image dimensions on battles (in pixels)
    frame_width = 57.0 * 2.0,
    frame_height = 88.0 * 2.0,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 32, duration = 150 },
        [1] = { id = 33, duration = 150 },
        [2] = { id = 34, duration = 150 },
        [3] = { id = 35, duration = 150 },
        [4] = { id = 36, duration = 150 },
        [5] = { id = 37, duration = 150 },
        [6] = { id = 38, duration = 150 },
        [7] = { id = 39, duration = 150 },
    }
}
