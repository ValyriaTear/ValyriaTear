-- Animation file descriptor
-- This file will describe the frames used to load the sprite animations

animation = {

    -- The file to load the frames from
    image_filename = "img/sprites/map/npcs/story/herth_spritesheet.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 4,
    columns = 6,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 19, duration = 150 },
        [1] = { id = 20, duration = 150 },
        [2] = { id = 21, duration = 150 },
        [3] = { id = 19, duration = 999999 }
    }
}
