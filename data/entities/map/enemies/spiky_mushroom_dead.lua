-- Sprite animation file descriptor
-- This file will describe the frames used to load the sprite animations

animation = {

    -- The file to load the frames from
    image_filename = "data/entities/map/enemies/spiky_mushroom.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 4,
    columns = 10,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 19, duration = 999999, x_offset = 7.0 }
    }
}
