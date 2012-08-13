-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "img/tilesets/building_interior_objects_01.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 16,
	columns = 16,
	-- set the image dimensions on the map (in pixels)
	frame_width = 32,
	frame_height = 32,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 240, duration = 150 },
        [1] = { id = 241, duration = 150 },
        [2] = { id = 242, duration = 300 },
        [3] = { id = 243, duration = 150 },
        [4] = { id = 244, duration = 150 },
        [5] = { id = 245, duration = 150 },
        [6] = { id = 246, duration = 300 },
        [7] = { id = 247, duration = 150 }
    }
}
