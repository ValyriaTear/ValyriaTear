tileset = {}

tileset.image = "img/tilesets/desert_cave_walls.png"
tileset.num_tile_cols = 16
tileset.num_tile_rows = 16

-- The general walkability of the tiles in the tileset. Zero indicates walkable. One tile has four walkable quadrants listed as: NW corner, NE corner, SW corner, SE corner.
tileset.walkability = {}
tileset.walkability[0] = {}
tileset.walkability[0][0] = { 0, 0, 0, 1 }
tileset.walkability[0][1] = { 0, 0, 1, 0 }
tileset.walkability[0][2] = { 0, 0, 1, 1 }
tileset.walkability[0][3] = { 0, 0, 1, 0 }
tileset.walkability[0][4] = { 1, 1, 1, 1 }
tileset.walkability[0][5] = { 1, 1, 1, 1 }
tileset.walkability[0][6] = { 1, 1, 1, 1 }
tileset.walkability[0][7] = { 1, 1, 1, 1 }
tileset.walkability[0][8] = { 1, 1, 1, 1 }
tileset.walkability[0][9] = { 1, 1, 1, 1 }
tileset.walkability[0][10] = { 1, 1, 1, 1 }
tileset.walkability[0][11] = { 1, 1, 1, 1 }
tileset.walkability[0][12] = { 1, 1, 1, 1 }
tileset.walkability[0][13] = { 1, 1, 1, 1 }
tileset.walkability[0][14] = { 1, 1, 1, 1 }
tileset.walkability[0][15] = { 1, 1, 1, 1 }
tileset.walkability[1] = {}
tileset.walkability[1][0] = { 0, 1, 0, 0 }
tileset.walkability[1][1] = { 1, 1, 1, 1 }
tileset.walkability[1][2] = { 1, 1, 1, 1 }
tileset.walkability[1][3] = { 1, 0, 0, 0 }
tileset.walkability[1][4] = { 1, 1, 1, 1 }
tileset.walkability[1][5] = { 1, 1, 1, 1 }
tileset.walkability[1][6] = { 1, 1, 1, 1 }
tileset.walkability[1][7] = { 1, 1, 1, 1 }
tileset.walkability[1][8] = { 1, 1, 1, 1 }
tileset.walkability[1][9] = { 1, 1, 1, 1 }
tileset.walkability[1][10] = { 1, 1, 1, 1 }
tileset.walkability[1][11] = { 1, 1, 1, 1 }
tileset.walkability[1][12] = { 1, 1, 1, 1 }
tileset.walkability[1][13] = { 1, 1, 1, 1 }
tileset.walkability[1][14] = { 1, 1, 1, 1 }
tileset.walkability[1][15] = { 1, 1, 1, 1 }
tileset.walkability[2] = {}
tileset.walkability[2][0] = { 0, 1, 0, 1 }
tileset.walkability[2][1] = { 1, 1, 1, 1 }
tileset.walkability[2][2] = { 1, 1, 1, 1 }
tileset.walkability[2][3] = { 1, 0, 1, 1 }
tileset.walkability[2][4] = { 1, 1, 1, 1 }
tileset.walkability[2][5] = { 1, 1, 1, 1 }
tileset.walkability[2][6] = { 1, 1, 1, 1 }
tileset.walkability[2][7] = { 1, 1, 1, 1 }
tileset.walkability[2][8] = { 1, 1, 1, 1 }
tileset.walkability[2][9] = { 1, 1, 1, 1 }
tileset.walkability[2][10] = { 1, 1, 1, 1 }
tileset.walkability[2][11] = { 1, 1, 1, 1 }
tileset.walkability[2][12] = { 1, 1, 1, 1 }
tileset.walkability[2][13] = { 1, 1, 1, 1 }
tileset.walkability[2][14] = { 1, 1, 1, 1 }
tileset.walkability[2][15] = { 1, 1, 1, 1 }
tileset.walkability[3] = {}
tileset.walkability[3][0] = { 1, 1, 1, 1 }
tileset.walkability[3][1] = { 1, 1, 1, 1 }
tileset.walkability[3][2] = { 1, 1, 1, 1 }
tileset.walkability[3][3] = { 1, 1, 1, 1 }
tileset.walkability[3][4] = { 1, 1, 1, 1 }
tileset.walkability[3][5] = { 1, 1, 1, 1 }
tileset.walkability[3][6] = { 1, 1, 1, 1 }
tileset.walkability[3][7] = { 1, 1, 1, 1 }
tileset.walkability[3][8] = { 1, 1, 1, 1 }
tileset.walkability[3][9] = { 1, 1, 1, 1 }
tileset.walkability[3][10] = { 1, 1, 1, 1 }
tileset.walkability[3][11] = { 1, 1, 1, 1 }
tileset.walkability[3][12] = { 1, 1, 1, 1 }
tileset.walkability[3][13] = { 1, 1, 1, 1 }
tileset.walkability[3][14] = { 1, 1, 1, 1 }
tileset.walkability[3][15] = { 1, 1, 1, 1 }
tileset.walkability[4] = {}
tileset.walkability[4][0] = { 1, 1, 1, 1 }
tileset.walkability[4][1] = { 1, 1, 1, 1 }
tileset.walkability[4][2] = { 1, 1, 1, 1 }
tileset.walkability[4][3] = { 1, 1, 1, 0 }
tileset.walkability[4][4] = { 1, 1, 1, 1 }
tileset.walkability[4][5] = { 1, 1, 1, 1 }
tileset.walkability[4][6] = { 1, 1, 1, 1 }
tileset.walkability[4][7] = { 1, 1, 1, 1 }
tileset.walkability[4][8] = { 1, 1, 1, 1 }
tileset.walkability[4][9] = { 1, 1, 1, 1 }
tileset.walkability[4][10] = { 0, 0, 0, 0 }
tileset.walkability[4][11] = { 0, 0, 0, 0 }
tileset.walkability[4][12] = { 0, 0, 0, 0 }
tileset.walkability[4][13] = { 0, 0, 0, 0 }
tileset.walkability[4][14] = { 0, 0, 1, 1 }
tileset.walkability[4][15] = { 1, 1, 1, 1 }
tileset.walkability[5] = {}
tileset.walkability[5][0] = { 1, 1, 1, 1 }
tileset.walkability[5][1] = { 1, 1, 1, 1 }
tileset.walkability[5][2] = { 1, 1, 1, 1 }
tileset.walkability[5][3] = { 1, 1, 1, 1 }
tileset.walkability[5][4] = { 1, 1, 1, 1 }
tileset.walkability[5][5] = { 1, 1, 1, 1 }
tileset.walkability[5][6] = { 1, 1, 1, 1 }
tileset.walkability[5][7] = { 1, 1, 1, 1 }
tileset.walkability[5][8] = { 1, 1, 1, 1 }
tileset.walkability[5][9] = { 1, 1, 1, 1 }
tileset.walkability[5][10] = { 0, 0, 0, 0 }
tileset.walkability[5][11] = { 0, 0, 0, 0 }
tileset.walkability[5][12] = { 1, 1, 1, 1 }
tileset.walkability[5][13] = { 1, 0, 1, 1 }
tileset.walkability[5][14] = { 1, 1, 1, 1 }
tileset.walkability[5][15] = { 1, 1, 1, 1 }
tileset.walkability[6] = {}
tileset.walkability[6][0] = { 1, 1, 1, 1 }
tileset.walkability[6][1] = { 1, 1, 1, 1 }
tileset.walkability[6][2] = { 1, 1, 1, 1 }
tileset.walkability[6][3] = { 1, 1, 1, 1 }
tileset.walkability[6][4] = { 1, 1, 1, 1 }
tileset.walkability[6][5] = { 1, 1, 1, 1 }
tileset.walkability[6][6] = { 1, 1, 1, 1 }
tileset.walkability[6][7] = { 1, 1, 1, 1 }
tileset.walkability[6][8] = { 1, 1, 1, 1 }
tileset.walkability[6][9] = { 1, 1, 1, 1 }
tileset.walkability[6][10] = { 0, 0, 0, 0 }
tileset.walkability[6][11] = { 0, 0, 0, 0 }
tileset.walkability[6][12] = { 0, 0, 0, 0 }
tileset.walkability[6][13] = { 0, 0, 0, 0 }
tileset.walkability[6][14] = { 0, 0, 0, 0 }
tileset.walkability[6][15] = { 0, 0, 0, 0 }
tileset.walkability[7] = {}
tileset.walkability[7][0] = { 0, 0, 0, 0 }
tileset.walkability[7][1] = { 0, 0, 0, 0 }
tileset.walkability[7][2] = { 0, 0, 0, 0 }
tileset.walkability[7][3] = { 0, 0, 0, 0 }
tileset.walkability[7][4] = { 1, 1, 1, 1 }
tileset.walkability[7][5] = { 1, 1, 1, 1 }
tileset.walkability[7][6] = { 1, 1, 1, 1 }
tileset.walkability[7][7] = { 1, 1, 1, 1 }
tileset.walkability[7][8] = { 1, 1, 1, 1 }
tileset.walkability[7][9] = { 1, 1, 1, 1 }
tileset.walkability[7][10] = { 0, 0, 0, 0 }
tileset.walkability[7][11] = { 0, 0, 0, 0 }
tileset.walkability[7][12] = { 1, 1, 1, 1 }
tileset.walkability[7][13] = { 1, 1, 1, 1 }
tileset.walkability[7][14] = { 1, 1, 1, 1 }
tileset.walkability[7][15] = { 1, 1, 1, 1 }
tileset.walkability[8] = {}
tileset.walkability[8][0] = { 1, 1, 1, 1 }
tileset.walkability[8][1] = { 1, 1, 1, 1 }
tileset.walkability[8][2] = { 1, 1, 1, 1 }
tileset.walkability[8][3] = { 1, 0, 1, 1 }
tileset.walkability[8][4] = { 1, 1, 1, 1 }
tileset.walkability[8][5] = { 1, 1, 1, 1 }
tileset.walkability[8][6] = { 1, 1, 1, 1 }
tileset.walkability[8][7] = { 1, 1, 1, 1 }
tileset.walkability[8][8] = { 1, 1, 1, 1 }
tileset.walkability[8][9] = { 1, 1, 1, 1 }
tileset.walkability[8][10] = { 0, 0, 0, 0 }
tileset.walkability[8][11] = { 0, 0, 0, 0 }
tileset.walkability[8][12] = { 0, 0, 0, 1 }
tileset.walkability[8][13] = { 0, 0, 1, 0 }
tileset.walkability[8][14] = { 0, 0, 1, 1 }
tileset.walkability[8][15] = { 0, 0, 0, 0 }
tileset.walkability[9] = {}
tileset.walkability[9][0] = { 1, 1, 1, 1 }
tileset.walkability[9][1] = { 1, 1, 1, 1 }
tileset.walkability[9][2] = { 1, 1, 1, 1 }
tileset.walkability[9][3] = { 1, 1, 1, 1 }
tileset.walkability[9][4] = { 1, 1, 1, 1 }
tileset.walkability[9][5] = { 1, 1, 1, 1 }
tileset.walkability[9][6] = { 1, 1, 0, 1 }
tileset.walkability[9][7] = { 1, 1, 1, 1 }
tileset.walkability[9][8] = { 1, 1, 1, 1 }
tileset.walkability[9][9] = { 1, 1, 1, 1 }
tileset.walkability[9][10] = { 0, 0, 0, 0 }
tileset.walkability[9][11] = { 0, 0, 0, 0 }
tileset.walkability[9][12] = { 1, 1, 1, 1 }
tileset.walkability[9][13] = { 1, 1, 1, 1 }
tileset.walkability[9][14] = { 1, 1, 1, 1 }
tileset.walkability[9][15] = { 0, 0, 1, 1 }
tileset.walkability[10] = {}
tileset.walkability[10][0] = { 1, 1, 1, 1 }
tileset.walkability[10][1] = { 1, 1, 1, 1 }
tileset.walkability[10][2] = { 1, 1, 1, 1 }
tileset.walkability[10][3] = { 1, 1, 1, 1 }
tileset.walkability[10][4] = { 1, 1, 1, 1 }
tileset.walkability[10][5] = { 1, 1, 1, 1 }
tileset.walkability[10][6] = { 1, 1, 1, 1 }
tileset.walkability[10][7] = { 1, 1, 1, 1 }
tileset.walkability[10][8] = { 0, 0, 0, 0 }
tileset.walkability[10][9] = { 0, 0, 0, 0 }
tileset.walkability[10][10] = { 1, 1, 1, 1 }
tileset.walkability[10][11] = { 1, 1, 1, 1 }
tileset.walkability[10][12] = { 1, 1, 1, 1 }
tileset.walkability[10][13] = { 1, 1, 1, 1 }
tileset.walkability[10][14] = { 1, 1, 1, 1 }
tileset.walkability[10][15] = { 1, 1, 1, 1 }
tileset.walkability[11] = {}
tileset.walkability[11][0] = { 1, 1, 1, 1 }
tileset.walkability[11][1] = { 1, 1, 1, 1 }
tileset.walkability[11][2] = { 1, 1, 1, 1 }
tileset.walkability[11][3] = { 1, 1, 1, 1 }
tileset.walkability[11][4] = { 1, 1, 1, 1 }
tileset.walkability[11][5] = { 1, 1, 1, 1 }
tileset.walkability[11][6] = { 1, 1, 1, 1 }
tileset.walkability[11][7] = { 1, 1, 1, 1 }
tileset.walkability[11][8] = { 0, 0, 0, 0 }
tileset.walkability[11][9] = { 0, 0, 0, 0 }
tileset.walkability[11][10] = { 1, 1, 1, 1 }
tileset.walkability[11][11] = { 1, 1, 1, 1 }
tileset.walkability[11][12] = { 1, 1, 1, 1 }
tileset.walkability[11][13] = { 1, 1, 1, 1 }
tileset.walkability[11][14] = { 1, 1, 1, 1 }
tileset.walkability[11][15] = { 1, 1, 1, 1 }
tileset.walkability[12] = {}
tileset.walkability[12][0] = { 1, 1, 1, 1 }
tileset.walkability[12][1] = { 1, 1, 1, 1 }
tileset.walkability[12][2] = { 1, 1, 1, 1 }
tileset.walkability[12][3] = { 1, 1, 1, 1 }
tileset.walkability[12][4] = { 1, 1, 1, 1 }
tileset.walkability[12][5] = { 1, 1, 1, 1 }
tileset.walkability[12][6] = { 1, 1, 1, 1 }
tileset.walkability[12][7] = { 1, 1, 1, 1 }
tileset.walkability[12][8] = { 1, 1, 1, 1 }
tileset.walkability[12][9] = { 1, 1, 1, 1 }
tileset.walkability[12][10] = { 1, 1, 1, 1 }
tileset.walkability[12][11] = { 1, 1, 1, 1 }
tileset.walkability[12][12] = { 1, 1, 1, 1 }
tileset.walkability[12][13] = { 1, 1, 1, 1 }
tileset.walkability[12][14] = { 1, 1, 1, 1 }
tileset.walkability[12][15] = { 1, 1, 1, 1 }
tileset.walkability[13] = {}
tileset.walkability[13][0] = { 1, 1, 1, 1 }
tileset.walkability[13][1] = { 1, 1, 1, 1 }
tileset.walkability[13][2] = { 1, 1, 1, 1 }
tileset.walkability[13][3] = { 1, 1, 1, 1 }
tileset.walkability[13][4] = { 1, 1, 1, 1 }
tileset.walkability[13][5] = { 1, 1, 1, 1 }
tileset.walkability[13][6] = { 1, 1, 1, 1 }
tileset.walkability[13][7] = { 1, 1, 1, 1 }
tileset.walkability[13][8] = { 1, 1, 1, 1 }
tileset.walkability[13][9] = { 1, 1, 1, 1 }
tileset.walkability[13][10] = { 1, 1, 1, 1 }
tileset.walkability[13][11] = { 1, 1, 1, 1 }
tileset.walkability[13][12] = { 1, 1, 1, 1 }
tileset.walkability[13][13] = { 1, 1, 1, 1 }
tileset.walkability[13][14] = { 1, 1, 1, 1 }
tileset.walkability[13][15] = { 1, 1, 1, 1 }
tileset.walkability[14] = {}
tileset.walkability[14][0] = { 1, 1, 1, 1 }
tileset.walkability[14][1] = { 1, 1, 1, 1 }
tileset.walkability[14][2] = { 1, 1, 1, 1 }
tileset.walkability[14][3] = { 1, 1, 1, 1 }
tileset.walkability[14][4] = { 1, 1, 1, 1 }
tileset.walkability[14][5] = { 1, 1, 1, 1 }
tileset.walkability[14][6] = { 1, 1, 1, 1 }
tileset.walkability[14][7] = { 1, 1, 1, 1 }
tileset.walkability[14][8] = { 1, 1, 1, 1 }
tileset.walkability[14][9] = { 1, 1, 1, 1 }
tileset.walkability[14][10] = { 1, 1, 1, 1 }
tileset.walkability[14][11] = { 1, 1, 1, 1 }
tileset.walkability[14][12] = { 1, 1, 1, 1 }
tileset.walkability[14][13] = { 1, 1, 1, 1 }
tileset.walkability[14][14] = { 1, 1, 1, 1 }
tileset.walkability[14][15] = { 1, 1, 1, 1 }
tileset.walkability[15] = {}
tileset.walkability[15][0] = { 1, 1, 1, 1 }
tileset.walkability[15][1] = { 1, 1, 1, 1 }
tileset.walkability[15][2] = { 1, 1, 1, 1 }
tileset.walkability[15][3] = { 1, 1, 1, 1 }
tileset.walkability[15][4] = { 1, 1, 1, 1 }
tileset.walkability[15][5] = { 1, 1, 1, 1 }
tileset.walkability[15][6] = { 1, 1, 1, 1 }
tileset.walkability[15][7] = { 1, 1, 1, 1 }
tileset.walkability[15][8] = { 1, 1, 1, 1 }
tileset.walkability[15][9] = { 1, 1, 1, 1 }
tileset.walkability[15][10] = { 1, 1, 1, 1 }
tileset.walkability[15][11] = { 1, 1, 1, 1 }
tileset.walkability[15][12] = { 1, 1, 1, 1 }
tileset.walkability[15][13] = { 1, 1, 1, 1 }
tileset.walkability[15][14] = { 1, 1, 1, 1 }
tileset.walkability[15][15] = { 1, 1, 1, 1 }

-- The animated tiles table has one row per animated tile, with each entry in a row indicating which tile in the tileset is the next part of the animation, followed by the time in ms that the tile will be displayed for.
tileset.animated_tiles = {}
tileset.animated_tiles[1] = { 170, 2500, 172, 100, 174, 100, 192, 100, 194, 100, 196, 100, 198, 100, 200, 100, 202, 100, 204, 100, 206, 100, 224, 100, 226, 100, 228, 100, 230, 100, 232, 100, 234, 100, 236, 100, 238, 100, 170, 2500, 238, 100, 236, 100, 234, 100, 232, 100, 230, 100, 228, 100, 226, 100, 224, 100, 206, 100, 204, 100, 202, 100, 200, 100, 198, 100, 196, 100, 194, 100, 192, 100, 174, 100 }
tileset.animated_tiles[2] = { 171, 2500, 173, 100, 175, 100, 193, 100, 195, 100, 197, 100, 199, 100, 201, 100, 203, 100, 205, 100, 207, 100, 225, 100, 227, 100, 229, 100, 231, 100, 233, 100, 235, 100, 237, 100, 239, 100, 171, 2500, 239, 100, 237, 100, 235, 100, 233, 100, 231, 100, 229, 100, 227, 100, 225, 100, 207, 100, 205, 100, 203, 100, 201, 100, 199, 100, 197, 100, 195, 100, 193, 100, 175, 100 }
tileset.animated_tiles[3] = { 186, 2500, 188, 100, 190, 100, 208, 100, 210, 100, 212, 100, 214, 100, 216, 100, 218, 100, 220, 100, 222, 100, 240, 100, 242, 100, 244, 100, 246, 100, 248, 100, 250, 100, 252, 100, 254, 100, 186, 2500, 254, 100, 252, 100, 250, 100, 248, 100, 246, 100, 244, 100, 242, 100, 240, 100, 222, 100, 220, 100, 218, 100, 216, 100, 214, 100, 212, 100, 210, 100, 208, 100, 190, 100 }
tileset.animated_tiles[4] = { 187, 2500, 189, 100, 191, 100, 209, 100, 211, 100, 213, 100, 215, 100, 217, 100, 219, 100, 221, 100, 223, 100, 241, 100, 243, 100, 245, 100, 247, 100, 249, 100, 251, 100, 253, 100, 255, 100, 187, 2500, 255, 100, 253, 100, 251, 100, 249, 100, 247, 100, 245, 100, 243, 100, 241, 100, 223, 100, 221, 100, 219, 100, 217, 100, 215, 100, 213, 100, 211, 100, 209, 100, 191, 100 }
