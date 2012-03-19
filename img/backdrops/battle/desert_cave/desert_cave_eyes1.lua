-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "img/backdrops/battle/desert_cave/desert_cave_eyes1.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 1,
	columns = 2,
	-- set the image dimensions on battles (in pixels)
	frame_width = 28.0,
	frame_height = 32.0,
	-- The frames duration in milliseconds
	frames_duration = { 4000, 3000 }
}
