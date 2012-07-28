-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "img/misc/save_point/save_point4.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 2,
	columns = 3,
	-- set the image dimensions (in pixels)
	frame_width = 64.0,
	frame_height = 54.0,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 150 },
        [1] = { id = 1, duration = 150 },
        [2] = { id = 2, duration = 75 },
        [3] = { id = 3, duration = 75 },
        [4] = { id = 4, duration = 150 },
        [5] = { id = 5, duration = 150 }
    }
}
