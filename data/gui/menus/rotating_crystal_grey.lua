-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "data/gui/menus/rotating_crystal_grey.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 1,
	columns = 8,
	-- set the image dimensions (in pixels)
	frame_width = 16.0,
	frame_height = 16.0,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 120 },
        [1] = { id = 1, duration = 120 },
        [2] = { id = 2, duration = 120 },
        [3] = { id = 3, duration = 120 },
        [4] = { id = 4, duration = 120 },
        [5] = { id = 5, duration = 120 },
        [6] = { id = 6, duration = 120 },
        [7] = { id = 7, duration = 120 }
    }
}
