-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "img/sprites/map/characters/bronann_herostance_unarmed.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 1,
	columns = 3,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 200 },
        [1] = { id = 1, duration = 150 },
        [2] = { id = 2, duration = 1000000 }
    }
}
