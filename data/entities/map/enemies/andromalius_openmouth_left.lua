-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

animation = {

    -- The file to load the frames from
    image_filename = "img/sprites/map/enemies/andromalius_spritesheet.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 5,
    columns = 8,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 24, duration = 150 },
        [1] = { id = 25, duration = 150 },
        [2] = { id = 26, duration = 150 },
        [3] = { id = 27, duration = 150 },
        [4] = { id = 28, duration = 150 },
        [5] = { id = 29, duration = 150 },
        [6] = { id = 30, duration = 150 },
        [7] = { id = 31, duration = 150 },
    }
}
