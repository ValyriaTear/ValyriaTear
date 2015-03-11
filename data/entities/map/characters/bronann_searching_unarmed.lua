-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "data/entities/map/characters/bronann_searching_unarmed.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 1,
	columns = 3,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 250 },
        [1] = { id = 1, duration = 150 },
        [2] = { id = 2, duration = 250 },
        [3] = { id = 1, duration = 150 },
        [4] = { id = 0, duration = 250 },
        [5] = { id = 1, duration = 150 },
        [6] = { id = 2, duration = 250 },
        [7] = { id = 1, duration = 150 },
    }
}
