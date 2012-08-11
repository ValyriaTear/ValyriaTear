-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "img/tilesets/village_exterior.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 16,
	columns = 16,
	-- set the image dimensions on the map (in pixels)
	frame_width = 32,
	frame_height = 32,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 125, duration = 100000 }
    }
}
