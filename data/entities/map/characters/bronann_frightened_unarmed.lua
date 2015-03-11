-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "data/entities/map/characters/bronann_frightened_unarmed.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 1,
	columns = 6,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 90 },
        [1] = { id = 1, duration = 90 },
        [2] = { id = 2, duration = 120 },
        [3] = { id = 3, duration = 120 },
        [4] = { id = 4, duration = 90 },
        [5] = { id = 5, duration = 90 }
    }
}
