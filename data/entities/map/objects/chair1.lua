-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "data/entities/map/objects/chair1.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 1,
	columns = 1,
	-- set the image dimensions on the map (in pixels)
	frame_width = 31,
	frame_height = 47,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 100000 }
    }
}
