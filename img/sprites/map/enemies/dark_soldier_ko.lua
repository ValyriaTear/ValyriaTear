-- Animation file descriptor
-- This file will describe the frames used to load the animation

animation = {

    -- The file to load the frames from
    image_filename = "img/sprites/map/enemies/dark_soldier_ko.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 1,
    columns = 1,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 999999 }
    }
}
