-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "img/sprites/map/knight_attack.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 1,
	columns = 5,
	-- The frames duration in milliseconds
	frames_duration = { 75, 75, 75, 75, 75 }
}
