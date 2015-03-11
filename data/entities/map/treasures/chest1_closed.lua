-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "data/entities/map/treasures/chest1.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 1,
	columns = 4,
	-- set the image dimensions on the map (in pixels)
	frame_width = 32,
	frame_height = 32,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 100000 }
    }
}
