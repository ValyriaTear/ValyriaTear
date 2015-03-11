-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

	-- The file to load the frames from
	image_filename = "img/sprites/map/characters/bronann_bed_animation.png",
	-- The number of rows and columns of images, will be used to compute
	-- the images width and height, and also the frames number (row x col)
	rows = 3,
	columns = 3,
	-- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 2000 },
        [1] = { id = 2, duration = 400 },
        [2] = { id = 1, duration = 150 },
        [3] = { id = 4, duration = 1000 },
        [4] = { id = 3, duration = 1000 },
        [5] = { id = 5, duration = 2000 },
        [6] = { id = 2, duration = 200 },
        [7] = { id = 5, duration = 500 },
        [8] = { id = 2, duration = 200 },
        [9] = { id = 5, duration = 500 },
        [10]= { id = 6, duration = 150 },
        [11]= { id = 7, duration = 150 },
        [12]= { id = 6, duration = 150 },
        [13]= { id = 7, duration = 150 },
        [14]= { id = 8, duration = 9999999 }
    }
}
