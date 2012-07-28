-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "img/backdrops/battle/desert_cave/desert_cave_creatures.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 2,
	columns = 5,
	-- set the image dimensions on battles (in pixels)
	frame_width = 168.0,
	frame_height = 178.0,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 5000 },
        [1] = { id = 1, duration = 2000 },
        [2] = { id = 2, duration = 2000 },
        [3] = { id = 3, duration = 140 },
        [4] = { id = 4, duration = 140 },
        [5] = { id = 5, duration = 140 },
        [6] = { id = 6, duration = 150 },
        [7] = { id = 7, duration = 150 },
        [8] = { id = 8, duration = 1000 },
        [9] = { id = 9, duration = 2000 }
    }
}
