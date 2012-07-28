-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "img/sprites/map/objects/cat1.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 1,
	columns = 2,
	-- set the image dimensions on the map (in pixels)
	frame_width = 22,
	frame_height = 25,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 300 },
        [1] = { id = 1, duration = 3000 },
        [2] = { id = 0, duration = 400 },
        [3] = { id = 1, duration = 300 }
    }
}
