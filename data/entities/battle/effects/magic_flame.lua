-- Animation file descriptor
-- This file will describe the frames used to load an animation

animation = {

    -- The file to load the frames from
    image_filename = "data/entities/battle/effects/magic_flame.png",
    -- The number of rows and columns of images, will be used to compute
    -- the images width and height, and also the frames number (row x col)
    rows = 7,
    columns = 8,
    -- set the image dimensions on battles (in pixels)
    frame_width = 256.0,
    frame_height = 300.0,
    -- The frames duration in milliseconds
    frames = {
        [0] = { id = 0, duration = 30 },
        [1] = { id = 1, duration = 30 },
        [2] = { id = 2, duration = 30 },
        [3] = { id = 3, duration = 30 },
        [4] = { id = 4, duration = 30 },
        [5] = { id = 5, duration = 30 },
        [6] = { id = 6, duration = 30 },
        [7] = { id = 7, duration = 30 },
        [8] = { id = 8, duration = 30 },
        [9] = { id = 9, duration = 30 },
        [10] = { id = 10, duration = 30 },
        [11] = { id = 11, duration = 30 },
        [12] = { id = 12, duration = 30 },
        [13] = { id = 13, duration = 30 },
        [14] = { id = 14, duration = 30 },
        [15] = { id = 15, duration = 30 },
        [16] = { id = 16, duration = 30 },
        [17] = { id = 17, duration = 30 },
        [18] = { id = 18, duration = 30 },
        [19] = { id = 19, duration = 30 },
        [20] = { id = 20, duration = 30 },
        [21] = { id = 21, duration = 30 },
        [22] = { id = 22, duration = 30 },
        [23] = { id = 23, duration = 30 },
        [24] = { id = 24, duration = 30 },
        [25] = { id = 25, duration = 30 },
        [26] = { id = 26, duration = 30 },
        [27] = { id = 27, duration = 30 },
        [28] = { id = 28, duration = 30 },
        [29] = { id = 29, duration = 30 },
        [30] = { id = 30, duration = 30 },
        [31] = { id = 31, duration = 30 },
        [32] = { id = 32, duration = 30 },
        [33] = { id = 33, duration = 30 },
        [34] = { id = 34, duration = 30 },
        [35] = { id = 35, duration = 30 },
        [36] = { id = 36, duration = 30 },
        [37] = { id = 37, duration = 30 },
        [38] = { id = 38, duration = 30 },
        [39] = { id = 39, duration = 30 },
        [40] = { id = 40, duration = 30 },
        [41] = { id = 41, duration = 30 },
        [42] = { id = 42, duration = 30 },
        [43] = { id = 43, duration = 30 },
        [44] = { id = 44, duration = 30 },
        [45] = { id = 45, duration = 30 },
        [46] = { id = 46, duration = 30 },
        [47] = { id = 47, duration = 30 },
        [48] = { id = 48, duration = 30 },
        [49] = { id = 49, duration = 30 },
        [50] = { id = 50, duration = 0 }, -- 0 means forever
    }
}
