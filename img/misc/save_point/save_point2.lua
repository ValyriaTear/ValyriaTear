-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "img/misc/save_point/save_point2.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 2,
	columns = 5,
	-- set the image dimensions (in pixels)
	frame_width = 64.0,
	frame_height = 54.0,
	-- The frames duration in milliseconds
	frames_duration = { 2000, 100, 100, 100, 100,
                        100, 100, 100, 100, 2000}
}
