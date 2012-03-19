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
	frames_duration = { 5000, 2000, 2000, 150, 150,
						150, 150, 150, 1000, 2000 }
}
