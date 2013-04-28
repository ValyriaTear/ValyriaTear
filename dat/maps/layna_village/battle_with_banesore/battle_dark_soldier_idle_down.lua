-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "img/sprites/map/enemies/dark_soldier01_spritesheet.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 4,
	columns = 6,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 999999 },
    }
}
