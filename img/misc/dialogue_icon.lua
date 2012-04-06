-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "img/misc/dialogue_icon.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 4,
	columns = 4,
	-- set the image dimensions (in pixels)
	frame_width = 32.0,
	frame_height = 32.0,
	-- The frames duration in milliseconds
	frames_duration = { 100, 100, 100, 100,
						100, 100, 100, 100,
						100, 100, 100, 100,
						100, 100, 100, 100 }
}
