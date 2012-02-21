local ns = {}
setmetatable(ns, {__index = _G})
betrayal_room = ns;
setfenv(1, ns);

-- The map name and location graphic
map_name = " "
location_filename = "blank.png"

-- Allacrost map editor begin. Do not edit this line. --

-- A reference to the C++ MapMode object that was created with this file
map = {}

-- The number of contexts, rows, and columns that compose the map
num_map_contexts = 1
num_tile_cols = 32
num_tile_rows = 24

-- The sound files used on this map.
sound_filenames = {}

-- The music files used as background music on this map.
music_filenames = {}
music_filenames[1] = "mus/Betrayal.ogg"

-- The names of the contexts used to improve Editor user-friendliness
context_names = {}

-- The names of the tilesets used, with the path and file extension omitted
tileset_filenames = {}
tileset_filenames[1] = "desert_cave"
tileset_filenames[2] = "desert_cave_ground"
tileset_filenames[3] = "desert_cave_walls"
tileset_filenames[4] = "desert_cave_walls2"
tileset_filenames[5] = "desert_cave_water"

-- The map grid to indicate walkability. The size of the grid is 4x the size of the tile layer tables
-- Walkability status of tiles for 32 contexts. Zero indicates walkable. Valid range: [0:2^32-1]
map_grid = {}
map_grid[0] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[1] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[2] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[3] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[4] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[5] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[6] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[7] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[8] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[10] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[11] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[12] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[13] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[14] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[15] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[16] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[17] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[18] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[19] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[20] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[21] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[22] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[23] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[24] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[25] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[26] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[27] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[28] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[29] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[30] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[31] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[32] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[33] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[34] = { 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1 }
map_grid[35] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[36] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[37] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[38] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[39] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[41] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[42] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[43] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[44] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[45] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[46] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[47] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

-- The lower tile layer. The numbers are indeces to the tile_mappings table.
lower_layer = {}
lower_layer[0] = { 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 281, 298, 268, 266, 299, 297, 265, 299, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367 }
lower_layer[1] = { 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 281, 282, 300, 297, 267, 282, 299, 283, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367 }
lower_layer[2] = { 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 265, 266, 266, 281, 268, 284, 298, 266, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367 }
lower_layer[3] = { 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 282, 266, 265, 265, 282, 298, 297, 267, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367 }
lower_layer[4] = { 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 283, 282, 266, 284, 283, 283, 284, 281, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367 }
lower_layer[5] = { 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 298, 266, 282, 300, 284, 284, 300, 284, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367 }
lower_layer[6] = { 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 284, 268, 284, 298, 282, 281, 299, 297, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367 }
lower_layer[7] = { 474, 475, 476, 477, 474, 475, 476, 477, 474, 475, 476, 477, 265, 266, 299, 284, 283, 266, 281, 283, 474, 475, 476, 477, 474, 475, 476, 477, 474, 475, 476, 477 }
lower_layer[8] = { 490, 491, 492, 493, 490, 491, 492, 493, 490, 491, 492, 493, 367, 267, 283, 282, 298, 265, 298, 367, 490, 491, 492, 493, 490, 491, 492, 493, 490, 491, 492, 493 }
lower_layer[9] = { 506, 507, 508, 509, 506, 507, 508, 509, 506, 507, 508, 509, 367, 284, 265, 297, 298, 300, 299, 367, 506, 507, 508, 509, 506, 507, 508, 509, 506, 507, 508, 509 }
lower_layer[10] = { 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 297, 299, 300, 297, 282, 266, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367 }
lower_layer[11] = { 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 267, 268, 282, 284, 283, 299, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367 }
lower_layer[12] = { 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 298, 267, 268, 298, 265, 300, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367 }
lower_layer[13] = { 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 298, 284, 268, 282, 300, 299, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367 }
lower_layer[14] = { 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 367, 266, 267, 267, 267, 265, 265, 367, 367, 367, -1, -1, -1, -1, 367, 367, 367, 367, 367, 367 }
lower_layer[15] = { 367, 367, 367, 367, 367, 367, 479, 476, 477, 478, 367, 367, 367, 267, 266, 268, 283, 297, 284, 367, 367, 367, 479, 476, 477, 478, 367, 367, 367, 367, 367, 367 }
lower_layer[16] = { 367, 367, 367, 367, 367, 367, 495, 492, 493, 494, 367, 367, 367, 297, 297, 266, 298, 266, 299, 367, 367, 367, 495, 492, 493, 494, 367, 367, 367, 367, 367, 367 }
lower_layer[17] = { 479, 476, 477, 478, 367, 367, 511, 508, 509, 510, 367, 367, 367, 299, 283, 267, 265, 282, 298, 367, 367, 367, 511, 508, 509, 510, 367, 367, 479, 476, 477, 478 }
lower_layer[18] = { 495, 492, 493, 494, 367, 367, -1, -1, -1, -1, 367, 367, 367, 266, 267, 266, 284, 265, 299, 367, 367, 367, -1, -1, -1, -1, 367, 367, 495, 492, 493, 494 }
lower_layer[19] = { 511, 508, 509, 510, 367, 367, -1, -1, -1, -1, 367, 367, 367, 284, 282, 268, 297, 268, 267, 367, 367, 367, -1, -1, -1, -1, 367, 367, 511, 508, 509, 510 }
lower_layer[20] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 297, 298, 283, 282, 367, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
lower_layer[21] = { 284, 265, 284, 284, 267, 298, 265, 266, 298, 297, 297, 299, 268, 267, 298, 266, 297, 268, 267, 300, 298, 266, 267, 266, 282, 265, 281, 300, 300, 298, 297, 266 }
lower_layer[22] = { 268, 299, 283, 267, 265, 282, 298, 298, 268, 298, 268, 298, 298, 266, 265, 283, 281, 267, 282, 266, 268, 282, 265, 284, 300, 284, 298, 284, 297, 268, 267, 266 }
lower_layer[23] = { 298, 265, 283, 267, 284, 297, 268, 299, 267, 284, 265, 266, 298, 283, 300, 267, 298, 281, 281, 266, 298, 281, 297, 282, 281, 267, 297, 267, 267, 268, 284, 266 }

-- The middle tile layer. The numbers are indeces to the tile_mappings table.
middle_layer = {}
middle_layer[0] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[1] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[2] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[3] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[4] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[5] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[6] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[7] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[8] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 427, -1, -1, -1, -1, -1, -1, 428, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[9] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 443, -1, -1, -1, -1, -1, -1, 444, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 427, -1, -1, -1, -1, -1, -1, 428, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[11] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 443, -1, -1, -1, -1, -1, -1, 444, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[12] = { -1, -1, -1, -1, -1, -1, 412, -1, -1, 430, -1, -1, 427, -1, -1, -1, -1, -1, -1, 428, -1, -1, 412, -1, -1, 430, -1, -1, -1, -1, -1, -1 }
middle_layer[13] = { -1, -1, -1, -1, -1, -1, 427, -1, -1, 428, -1, -1, 443, -1, -1, -1, -1, -1, -1, 444, -1, -1, 427, -1, -1, 428, -1, -1, -1, -1, -1, -1 }
middle_layer[14] = { 412, -1, -1, 430, -1, -1, 443, -1, -1, 444, -1, -1, 427, -1, -1, -1, -1, -1, -1, 428, -1, -1, 443, -1, -1, 444, -1, -1, 412, -1, -1, 430 }
middle_layer[15] = { 427, -1, -1, 428, -1, -1, -1, -1, -1, -1, -1, -1, 443, -1, -1, -1, -1, -1, -1, 444, -1, -1, -1, -1, -1, -1, -1, -1, 427, -1, -1, 428 }
middle_layer[16] = { 443, -1, -1, 444, -1, -1, -1, -1, -1, -1, -1, -1, 427, -1, -1, -1, -1, -1, -1, 428, -1, -1, -1, -1, -1, -1, -1, -1, 443, -1, -1, 444 }
middle_layer[17] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 443, -1, -1, -1, -1, -1, -1, 444, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[18] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 427, -1, -1, -1, -1, -1, -1, 428, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[19] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 443, -1, -1, -1, -1, -1, -1, 444, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[20] = { 415, 431, 415, 431, 415, 431, 415, 431, 415, 431, 415, 431, 459, 298, 299, -1, -1, -1, -1, 460, 461, 431, 415, 431, 415, 431, 415, 431, 415, 431, 415, 431 }
middle_layer[21] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[22] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
middle_layer[23] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }

-- The upper tile layer. The numbers are indeces to the tile_mappings table.
upper_layer = {}
upper_layer[0] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 642, 643, -1, -1, -1, -1, 640, 641, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
upper_layer[1] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 658, 659, -1, -1, -1, -1, 656, 657, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
upper_layer[2] = { -1, -1, -1, -1, -1, -1, 580, 581, 582, 583, 584, 585, 674, 675, -1, -1, -1, -1, 672, 673, 580, 581, 582, 583, 584, 585, -1, -1, -1, -1, -1, -1 }
upper_layer[3] = { -1, -1, -1, -1, -1, -1, 596, 597, 598, 599, 600, 601, 562, 563, -1, -1, -1, -1, 560, 561, 596, 597, 598, 599, 600, 601, -1, -1, -1, -1, -1, -1 }
upper_layer[4] = { 518, 519, 520, 521, 522, 523, 612, 613, 614, 615, 616, 617, 578, 579, -1, -1, -1, -1, 576, 577, 612, 613, 614, 615, 616, 617, 518, 519, 520, 521, 522, 523 }
upper_layer[5] = { 534, 535, 536, 537, 682, 683, 628, 629, 630, 631, 632, 633, 594, 595, -1, -1, -1, -1, 592, 593, 628, 629, 630, 631, 632, 633, 534, 535, 536, 537, 538, 539 }
upper_layer[6] = { 550, 551, 552, 553, 698, 699, 644, 645, 646, 647, 648, 649, 610, 611, -1, -1, -1, -1, 608, 609, 644, 645, 646, 647, 648, 649, 550, 551, 552, 553, 554, 555 }
upper_layer[7] = { 566, 567, 568, 569, 570, 571, 660, 661, 662, 663, 664, 665, 626, 627, -1, -1, -1, -1, 624, 625, 660, 661, 662, 663, 664, 665, 566, 567, 568, 569, 570, 571 }
upper_layer[8] = { -1, -1, -1, -1, -1, -1, 512, 513, 514, 515, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 512, 513, 514, 515, -1, -1, -1, -1, -1, -1 }
upper_layer[9] = { -1, -1, -1, -1, -1, -1, 528, 529, 530, 531, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 528, 529, 530, 531, -1, -1, -1, -1, -1, -1 }
upper_layer[10] = { 512, 513, 514, 515, -1, -1, 544, 545, 546, 547, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 544, 545, 546, 547, -1, -1, 512, 513, 514, 515 }
upper_layer[11] = { 528, 529, 530, 531, -1, -1, 560, 561, 562, 563, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 560, 561, 562, 563, -1, -1, 528, 529, 530, 531 }
upper_layer[12] = { 544, 545, 546, 547, -1, -1, 576, 577, 578, 579, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 576, 577, 578, 579, -1, -1, 544, 545, 546, 547 }
upper_layer[13] = { 560, 561, 562, 563, -1, -1, 592, 593, 594, 595, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 592, 593, 594, 595, -1, -1, 560, 561, 562, 563 }
upper_layer[14] = { 576, 577, 578, 579, -1, -1, 608, 609, 610, 611, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 608, 609, 610, 611, -1, -1, 576, 577, 578, 579 }
upper_layer[15] = { 592, 593, 594, 595, -1, -1, 624, 625, 626, 627, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 624, 625, 626, 627, -1, -1, 592, 593, 594, 595 }
upper_layer[16] = { 608, 609, 610, 611, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 608, 609, 610, 611 }
upper_layer[17] = { 624, 625, 626, 627, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 624, 625, 626, 627 }
upper_layer[18] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
upper_layer[19] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
upper_layer[20] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
upper_layer[21] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
upper_layer[22] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
upper_layer[23] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }

-- All, if any, existing contexts follow.
-- Allacrost map editor end. Do not edit this line. --

battle_scene = false;

function Load(m)
	-- First, record the current map in the "map" variable that is global to this script
	map = m;
	dialogue_supervisor = m.dialogue_supervisor;
	event_supervisor = m.event_supervisor;

	local sprite;
	local dialogue;
	local event;
	local chest;

	if (GlobalManager:GetEventGroup("kyle_story"):DoesEventExist("betrayal") == true and GlobalManager:GetEventGroup("kyle_story"):DoesEventExist("betrayal_battle") == false) then
		battle_scene = true;
	end

	CreateDialogue();

	-- Create the player''s sprite
	sprite = ConstructSprite("Claudius", 1000, 32, 44);
	map:AddGroundObject(sprite);
	-- Set the camera to focus on the player''s sprite
	map:SetCamera(sprite);

	-- Add NPCs
	kyle = ConstructSprite("Kyle", 2, 38, 30, 0.0, 0.0);
	kyle:LoadAttackAnimations("img/sprites/map/kyle_attack_w.png");
	kyle:SetMovementSpeed(hoa_map.MapMode.VERY_FAST_SPEED);
	kyle:SetNoCollision(true);
	if (battle_scene == true) then
		map:AddGroundObject(kyle);
	end

	captain = ConstructSprite("Captain", 3, 26, 30, 0.0, 0.0);
	captain:SetNoCollision(true);
	if (battle_scene == true) then
		map:AddGroundObject(captain);
	end

	hector = ConstructSprite("Karlate", 4, 16, 44, 0.0, 0.0);
	hector:SetName(hoa_system.Translate("Hector"));
	hector:SetNoCollision(true);
	hector:SetVisible(false);
	if (battle_scene == true) then
		map:AddGroundObject(hector);
	end

	charles = ConstructSprite("Karlate", 5, 48, 44, 0.0, 0.0);
	charles:SetName(hoa_system.Translate("Charles"));
	charles:SetNoCollision(true);
	charles:SetVisible(false);

	if (battle_scene == true) then
		map:AddGroundObject(charles);
	end

	-- Create a zone for exiting the map, to be used as a trigger
	exit_zone = hoa_map.MapZone();
	-- Add a section to the zone to enable the user to exit the map
	exit_zone:AddSection(hoa_map.ZoneSection(30, 45, 34, 48));
	map:AddZone(exit_zone);

	-- Register events
	event = hoa_map.DialogueEvent(11000, 1); -- initial dialogue
	event_supervisor:RegisterEvent(event);

	event = hoa_map.PathMoveSpriteEvent(11001, kyle, 26, 30); -- Kyle moves
	event:AddEventLink(11002, false, 0);
	event_supervisor:RegisterEvent(event);

	event = hoa_map.AnimateSpriteEvent(11002, kyle); -- Kyle strikes
	event:AddFrame(12, 850);
	event:AddEventLink(11003, false, 0);
	event_supervisor:RegisterEvent(event);

	event = hoa_map.ScriptedEvent(11003, 6, 0); -- call function #6 to remove captain
	event:AddEventLink(11004, false, 0);
	event_supervisor:RegisterEvent(event);

	event = hoa_map.DialogueEvent(11004, 2); -- next dialogue
	-- dialogue is programmed to call event #11005
	event_supervisor:RegisterEvent(event);

	event = hoa_map.BattleEncounterEvent(11005, 107); -- final battle
	event:SetMusic("mus/Betrayal_Battle.ogg");
	event:SetBackground("img/backdrops/battle/desert_cave.png");
	event:AddBattleEvent(1);
	event:AddEventLink(11006, false, 0);
	event_supervisor:RegisterEvent(event);

	event = hoa_map.DialogueEvent(11006, 3); -- post-battle dialogue
	-- dialogue is programmed to call event #11007
	event_supervisor:RegisterEvent(event);

	event = hoa_map.ScriptedEvent(11007, 5, 0); -- calls function #5 to remove Kyle, finish
	event:AddEventLink(11008, false, 0);
	event_supervisor:RegisterEvent(event);

	event = hoa_map.DialogueEvent(11008, 4); -- closing dialogue
	event_supervisor:RegisterEvent(event);

	event = hoa_map.PathMoveSpriteEvent(12000, kyle, 32, 2); -- Kyle leaves
	event_supervisor:RegisterEvent(event);

	event = hoa_map.PathMoveSpriteEvent(12001, charles, 38, 44); -- Charles enters
	event_supervisor:RegisterEvent(event);

	event = hoa_map.PathMoveSpriteEvent(12002, hector, 26, 44); -- Hector enters
	event_supervisor:RegisterEvent(event);


	event = hoa_map.MapTransitionEvent(22111, "dat/maps/new_cave.lua");
	event_supervisor:RegisterEvent(event);

	if (battle_scene == true) then
		GlobalManager:GetEventGroup("kyle_story"):AddNewEvent("betrayal_battle", 1);
		event_supervisor:StartEvent(11000);
	end
end

function Draw()
	map:DrawMapLayers();
end

function Update()
	-- Check if the map camera is in the exit zone
	if (exit_zone:IsInsideZone(map.camera.x_position, map.camera.y_position) == true) then
		if (event_supervisor:IsEventActive(22111) == false) then
			event_supervisor:StartEvent(22111);
		end
	end
end

function CreateDialogue()
	local dialogue = hoa_map.MapDialogue(1);

	text = hoa_system.Translate("I’m very disappointed in you, Kyle.");
	dialogue:AddText(text, 3, 1, 0, false);
	text = hoa_system.Translate("Over here, Claudius!");
	dialogue:AddText(text, 3, -1, 11001, false);

	dialogue_supervisor:AddDialogue(dialogue);

	-- Dialogue with kyle
	dialogue = hoa_map.MapDialogue(2);

	text = hoa_system.Translate("No!");
	dialogue:AddText(text, 1000, 1, 0, false);
	text = hoa_system.Translate("What have you done…");
	dialogue:AddText(text, 1000, 2, 0, false);
	text = hoa_system.Translate("I’m sorry.  I didn't want to have to do that.");
	dialogue:AddText(text, 2, 3, 0, false);
	text = hoa_system.Translate("What are you doing?");
	dialogue:AddText(text, 2, 4, 0, false);
	text = hoa_system.Translate("I can’t let you get away.");
	dialogue:AddText(text, 1000, 5, 0, false);
	text = hoa_system.Translate("I’m your friend!");
	dialogue:AddText(text, 2, 6, 0, false);
	text = hoa_system.Translate("It’s my duty!");
	dialogue:AddText(text, 1000, 7, 0, false);
	text = hoa_system.Translate("Don’t do this.");
	dialogue:AddText(text, 2, 8, 0, false);
	text = hoa_system.Translate("You’ve already done it.");
	dialogue:AddText(text, 1000, 9, 0, false);
	text = hoa_system.Translate("I don’t want to fight you.");
	dialogue:AddText(text, 2, 10, 0, false);
	text = hoa_system.Translate("Then lower your weapon and turn yourself in.");
	dialogue:AddText(text, 1000, 11, 0, false);
	text = hoa_system.Translate("You know I can’t.");
	dialogue:AddText(text, 2, 12, 0, false);
	text = hoa_system.Translate("And you know I can’t just let you go.");
	dialogue:AddText(text, 1000, 13, 0, false);
	text = hoa_system.Translate("Isn’t our friendship more important than your duty?");
	dialogue:AddText(text, 2, 14, 0, false);
	text = hoa_system.Translate("You killed the Captain!");
	dialogue:AddText(text, 1000, -1, 11005, false); -- start boss battle event (11005)

	dialogue_supervisor:AddDialogue(dialogue);

	dialogue = hoa_map.MapDialogue(3);

	text = hoa_system.Translate("I just took some supplies, just enough to start a new life, away from this servitude. Can’t you understand?");
	dialogue:AddText(text, 2, 1, 0, false);
	text = hoa_system.Translate("You’ve tarnished your honor and disgraced the Harrvahan knighthood!");
	dialogue:AddText(text, 1000, 2, 0, false);
	text = hoa_system.Translate("To hell with the knighthood!");
	dialogue:AddText(text, 2, -1, 11007, false);

	dialogue_supervisor:AddDialogue(dialogue);

	dialogue = hoa_map.MapDialogue(4);

	text = hoa_system.Translate("Claudius! Are you all right?");
	dialogue:AddText(text, 5, 1, 0, false);
	text = hoa_system.Translate("I’m fine… But the captain… He fell…");
	dialogue:AddText(text, 1000, 2, 0, false);
	text = hoa_system.Translate("Everyone down to the cave floor! He might still be alive!");
	dialogue:AddText(text, 4, 3, 0, false);
	text = hoa_system.Translate("Damn you, Kyle.");
	dialogue:AddText(text, 1000, -1, 0, false);

	dialogue_supervisor:AddDialogue(dialogue);
end

map_functions[1] = function()
end

map_functions[2] = function()
end

map_functions[3] = function()
end

map_functions[4] = function()
end

-- Removes Kyle sprite from the map, bring in other soldiers
map_functions[5] = function()
	AudioManager:PlaySound("snd/rumble.wav");

	rock1 = ConstructSprite("Rubble", 11, 28, 36, 0.0, 0.0);
	rock2 = ConstructSprite("Rubble", 12, 31, 36, 0.0, 0.0);
	rock3 = ConstructSprite("Rubble", 13, 33, 36, 0.0, 0.0);
	rock4 = ConstructSprite("Rubble", 14, 36, 36, 0.0, 0.0);

	map:AddGroundObject(rock1);
	map:AddGroundObject(rock2);
	map:AddGroundObject(rock3);
	map:AddGroundObject(rock4);

	event_supervisor:StartEvent(12000); -- Kyle escapes
	event_supervisor:StartEvent(12001); -- Charles enters
	event_supervisor:StartEvent(12002); -- Hector enters

	charles:SetVisible(true);
	charles:SetNoCollision(false);
	hector:SetVisible(true);
	hector:SetNoCollision(false);
end

-- Remove captain from map
map_functions[6] = function()
	captain:SetVisible(false);
	captain:SetNoCollision(true);
	captain:SetUpdatable(false);
	captain:SetContext(2);
end
