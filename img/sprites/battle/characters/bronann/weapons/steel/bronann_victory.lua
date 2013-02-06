-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "img/sprites/battle/characters/bronann/weapons/steel/bronann_victory.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 2,
	columns = 5,
	-- set the image dimensions on battles (in pixels)
	frame_width = 128.0,
	frame_height = 128.0,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 150 },
        [1] = { id = 1, duration = 75 },
        [2] = { id = 2, duration = 75 },
        [3] = { id = 3, duration = 75 },
        [4] = { id = 4, duration = 75 },
        [5] = { id = 5, duration = 150 },
        [6] = { id = 6, duration = 150 },
        [7] = { id = 7, duration = 75 },
        [8] = { id = 8, duration = 75 },
        [9] = { id = 9, duration = 10000000 }
    }
}
