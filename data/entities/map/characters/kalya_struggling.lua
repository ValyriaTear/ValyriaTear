-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "data/entities/map/characters/kalya_struggling.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 1,
    columns = 4,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 150 },
        [1] = { id = 3, duration = 150 },
        [2] = { id = 0, duration = 150 },
        [3] = { id = 3, duration = 150 },
        [4] = { id = 1, duration = 800 },
        [5] = { id = 2, duration = 75 }
    }
}
