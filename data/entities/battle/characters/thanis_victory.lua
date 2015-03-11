-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "data/entities/battle/characters/thanis_victory.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 2,
	columns = 4,
	-- set the image dimensions on battles (in pixels)
	frame_width = 128.0,
	frame_height = 128.0,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 1000 },
        [1] = { id = 1, duration = 75 },
        [2] = { id = 2, duration = 75 },
        [3] = { id = 3, duration = 75 },
        [4] = { id = 4, duration = 75 },
        [5] = { id = 5, duration = 150 },
        [6] = { id = 6, duration = 300 },
        [7] = { id = 7, duration = 10000000 }
    }
}
