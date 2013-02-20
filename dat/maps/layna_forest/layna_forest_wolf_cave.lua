-- Valyria Tear map editor begin. Do not edit this line or put anything before this line. --

-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_wolf_cave = ns;
setfenv(1, ns);

-- The map name, subname and location image
map_name = "Layna Forest Cave"
map_image_filename = "img/menus/locations/desert_cave.png"
map_subname = ""

-- The number of rows, and columns that compose the map
num_tile_cols = 32
num_tile_rows = 24

-- The contexts names and inheritance definition
-- Tells the context id the current context inherit from
-- This means that the parent context will be used as a base, and the current
-- context will only have its own differences from it.
-- At least, the base context (id:0) can't a parent context, thus it should be equal to -1.
-- Note that a context cannot inherit from itself or a context with a higher id
-- since it would lead to nasty and useless loading use cases.
contexts = {}
contexts[0] = {}
contexts[0].name = "Base"
contexts[0].inherit_from = -1

-- The music file used as default background music on this map.
-- Other musics will have to handled through scripting.
music_filename = "mus/shrine-OGA-yd.ogg"

-- The names of the tilesets used, with the path and file extension omitted
tileset_filenames = {}
tileset_filenames[1] = "desert_cave"

-- The map grid to indicate walkability. The size of the grid is 4x the size of the tile layer tables
-- Walkability status of tiles for 32 contexts. Zero indicates walkable for all contexts. Valid range: [0:2^32-1]
-- Example: 1 (BIN 001) = wall for first context only, 2 (BIN 010) means wall for second context only, 5 (BIN 101) means Wall for first and third context.
map_grid = {}
map_grid[0] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[1] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[2] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1 }
map_grid[3] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1 }
map_grid[4] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[5] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[6] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1 }
map_grid[7] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1 }
map_grid[8] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1 }
map_grid[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1 }
map_grid[10] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1 }
map_grid[11] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1 }
map_grid[12] = { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }
map_grid[13] = { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }
map_grid[14] = { 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }
map_grid[15] = { 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }
map_grid[16] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }
map_grid[17] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }
map_grid[18] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }
map_grid[19] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }
map_grid[20] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 }
map_grid[21] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 }
map_grid[22] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 }
map_grid[23] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 }
map_grid[24] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 }
map_grid[25] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 }
map_grid[26] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 }
map_grid[27] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 }
map_grid[28] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 }
map_grid[29] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 }
map_grid[30] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 }
map_grid[31] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 }
map_grid[32] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 }
map_grid[33] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 }
map_grid[34] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[35] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[36] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[37] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[38] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[39] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[40] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[41] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[42] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[43] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[44] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[45] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[46] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
map_grid[47] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }

-- The tile layers. The numbers are indeces to the tile_mappings table.
layers = {}
layers[0] = {}
layers[0].type = "ground"
layers[0].name = "Background"
layers[0][0] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 97, 99, 101, 200, 99, 100, 101, 99, 102, -1 }
layers[0][1] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, 97, 99, 100, 101, 99, 100, 101, 102, -1, -1, -1, -1, 97, 113, 115, 192, 216, 115, 192, 117, 115, 119, 129 }
layers[0][2] = { -1, -1, -1, -1, -1, -1, -1, -1, 97, 113, 115, 116, 117, 115, 116, 117, 119, 103, -1, -1, 98, 113, 88, 10, 208, 11, 11, 208, 11, 12, 87, 145 }
layers[0][3] = { -1, -1, -1, -1, 97, 99, 100, 101, 113, 88, 37, 0, 0, 0, 0, 35, 87, 119, 129, 128, 113, 88, 37, 26, 27, 27, 27, 27, 27, 28, 35, 145 }
layers[0][4] = { -1, -1, -1, 97, 113, 115, 116, 117, 88, 37, 0, 0, 0, 3, 4, 5, 35, 87, 145, 144, 88, 37, 0, 26, 27, 27, 27, 27, 27, 28, 0, 145 }
layers[0][5] = { -1, -1, 97, 113, 88, 36, 36, 36, 37, 0, 1, 16, 1, 19, 20, 21, 1, 35, 145, 144, 37, 1, 1, 42, 43, 43, 43, 43, 43, 44, 1, 145 }
layers[0][6] = { -1, 128, 113, 88, 37, 1, 0, 0, 0, 1, 1, 32, 0, 35, 36, 37, 1, 0, 145, 160, 1, 1, 1, 1, 1, 3, 4, 5, 1, 1, 1, 145 }
layers[0][7] = { -1, 144, 88, 37, 1, 16, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 145, -1, 1, 1, 1, 0, 0, 19, 20, 21, 1, 0, 1, 145 }
layers[0][8] = { -1, 144, 37, 1, 1, 32, 1, 0, 1, 1, 1, 1, 1, 1, 16, 1, 1, 1, 145, -1, -1, 0, 0, 0, 0, 35, 36, 37, 1, 1, 0, 161 }
layers[0][9] = { -1, 144, 1, 1, 0, 0, 0, 3, 4, 4, 4, 5, 1, 1, 32, 1, 1, 1, 145, -1, -1, -1, 0, 0, 0, 0, 1, 0, 0, 1, 0, -1 }
layers[0][10] = { -1, 144, 1, 1, 0, 1, 3, 20, 20, 20, 20, 37, 1, 1, 1, 0, 0, 1, 161, -1, -1, -1, 1, 1, 0, 1, 1, 16, 1, 0, -1, -1 }
layers[0][11] = { -1, 160, 1, 1, 0, 1, 35, 36, 36, 36, 37, 1, 1, 1, 0, 0, 1, 1, -1, -1, -1, 128, 0, 1, 1, 1, 1, 32, 0, 0, -1, -1 }
layers[0][12] = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, -1, -1, -1, -1, 144, 0, 1, 1, 1, 0, 0, 0, 0, -1, -1 }
layers[0][13] = { 0, 0, 0, 0, -1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, -1, -1, -1, -1, 96, 1, 1, 1, 0, 0, 0, 0, -1, -1, -1 }
layers[0][14] = { -1, -1, -1, -1, -1, -1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 129, -1, -1, 97, 113, 0, 1, 1, 16, 0, 0, 0, -1, -1, -1 }
layers[0][15] = { -1, -1, -1, -1, -1, -1, -1, 1, 1, 16, 1, 0, 0, 0, 0, 1, 1, 145, -1, 128, 112, 88, 1, 1, 1, 32, 1, 1, 1, -1, -1, -1 }
layers[0][16] = { -1, -1, -1, -1, -1, -1, -1, 1, 0, 32, 0, 0, 1, 1, 0, 1, 1, 145, -1, 132, 88, 37, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1 }
layers[0][17] = { -1, -1, -1, -1, -1, -1, -1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 145, -1, 132, 37, 1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1 }
layers[0][18] = { -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 104, -1, 96, 1, 0, 0, 1, 0, 1, 0, 1, -1, -1, -1, -1 }
layers[0][19] = { -1, -1, -1, -1, -1, -1, -1, -1, 1, 0, 1, 0, 0, 0, 0, 1, 3, 120, 99, 112, 5, 0, 0, 0, 0, 1, 0, 1, -1, -1, -1, -1 }
layers[0][20] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 16, 1, 1, 35, 87, 115, 88, 37, 1, 1, 1, 0, 0, 0, 0, -1, -1, -1, -1 }
layers[0][21] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 32, 0, 0, 0, 35, 36, 37, 1, 1, 0, 1, 0, 1, 1, -1, -1, -1, -1, -1 }
layers[0][22] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 1, 1, 0, 1, 0, 17, 18, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1 }
layers[0][23] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1 }

layers[1] = {}
layers[1].type = "ground"
layers[1].name = "Background 2"
layers[1][0] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][1] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 165, -1 }
layers[1][2] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][3] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 74, -1, -1, -1, -1, -1, -1, 165, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][4] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 74, 74, 107, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][5] = { -1, -1, -1, -1, -1, 74, -1, -1, -1, -1, -1, 74, 74, -1, -1, -1, 90, 74, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][6] = { -1, -1, -1, -1, -1, -1, -1, -1, 92, -1, -1, 74, 74, 107, -1, -1, 74, 107, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][7] = { -1, -1, -1, -1, -1, -1, -1, 76, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][8] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, 92, -1, 106, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][9] = { -1, -1, -1, -1, -1, 76, -1, -1, -1, -1, 76, -1, -1, 90, -1, -1, -1, 108, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 92, 76, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][11] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, 106, -1, -1, 92, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][12] = { -1, -1, -1, -1, -1, -1, 76, -1, -1, -1, -1, 106, -1, -1, 90, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][13] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][14] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 106, -1, 92, 108, -1, 76, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][15] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][16] = { -1, -1, -1, -1, -1, -1, -1, -1, 75, 76, -1, -1, -1, -1, -1, 106, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][17] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 90, -1, 76, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][18] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 90, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][19] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 75, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][20] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 90, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][21] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 90, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 90, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][22] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][23] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }

layers[2] = {}
layers[2].type = "ground"
layers[2].name = "Background 3"
layers[2][0] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][1] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][2] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][3] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][4] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][5] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 75, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][6] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 76, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][7] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][8] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][9] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][11] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][12] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][13] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][14] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][15] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][16] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][17] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][18] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][19] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][20] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][21] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][22] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][23] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }

layers[3] = {}
layers[3].type = "sky"
layers[3].name = "Sky"
layers[3][0] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][1] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][2] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][3] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][4] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][5] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][6] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 150, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][7] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 131, 150, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][8] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 131, 150, -1, -1, -1, -1, -1, -1, -1, 149, -1 }
layers[3][9] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 133, -1, -1, -1, -1, -1, -1, 149, 130, -1 }
layers[3][10] = { 48, 144, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 149, -1, -1, -1, -1, 133, -1, -1, -1, -1, -1, -1, 132, -1, -1 }
layers[3][11] = { 48, 160, 147, 147, 150, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 149, 130, -1, -1, -1, -1, 166, -1, -1, -1, -1, -1, -1, 132, -1, -1 }
layers[3][12] = { 48, 48, 48, 48, 131, 150, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 132, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 149, 130, -1, -1 }
layers[3][13] = { 48, 48, 48, 48, -1, 131, 150, -1, -1, -1, -1, -1, -1, -1, -1, -1, 132, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 132, -1, -1, -1 }
layers[3][14] = { -1, -1, -1, -1, -1, -1, 131, 150, -1, -1, -1, -1, -1, -1, -1, -1, 165, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 132, -1, -1, -1 }
layers[3][15] = { -1, -1, -1, -1, -1, -1, -1, 133, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 132, -1, -1, -1 }
layers[3][16] = { -1, -1, -1, -1, -1, -1, -1, 133, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 149, 130, -1, -1, -1 }
layers[3][17] = { -1, -1, -1, -1, -1, -1, -1, 133, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 132, -1, -1, -1, -1 }
layers[3][18] = { -1, -1, -1, -1, -1, -1, -1, 131, 150, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 132, -1, -1, -1, -1 }
layers[3][19] = { -1, -1, -1, -1, -1, -1, -1, -1, 131, 150, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 132, -1, -1, -1, -1 }
layers[3][20] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, 133, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 149, 130, -1, -1, -1, -1 }
layers[3][21] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, 131, 150, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 149, 130, -1, -1, -1, -1, -1 }
layers[3][22] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 131, 150, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 149, 130, -1, -1, -1, -1, -1, -1 }
layers[3][23] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 131, 150, -1, 149, 147, 148, 148, 147, 148, 147, 148, 147, 148, 130, -1, -1, -1, -1, -1, -1, -1 }


-- Valyria Tear map editor end. Do not edit this line. Place your scripts after this line. --

-- c++ objects instances
local Map = {};
local ObjectManager = {};
local DialogueManager = {};
local EventManager = {};
local GlobalEvents = {};

-- the main character handler
local hero = {};

-- Forest dialogue secondary hero
local kalya_sprite = {};

-- Name of the main sprite. Used to reload the good one at the end of the dialogue events.
local main_sprite_name = "";

-- the main map loading code
function Load(m)

	Map = m;
	ObjectManager = Map.object_supervisor;
	DialogueManager = Map.dialogue_supervisor;
	EventManager = Map.event_supervisor;
	GlobalEvents = Map.map_event_group;

	Map.unlimited_stamina = false;

	_CreateCharacters();
	_CreateObjects();
	--_CreateEnemies();

	-- Set the camera focus on hero
	Map:SetCamera(hero);
	-- This is a dungeon map, we'll use the front battle member sprite as default sprite.
	Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

	_CreateEvents();
	_CreateZones();

	-- Add a mediumly dark overlay
	Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/dark.png", 0.0, 0.0, false);

    -- Trigger the dialogue at entrance if not done yet.
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") == 0 and
            GlobalManager:DoesEventExist("story", "kalya_speech_in_wolf_cave") == false) then
        hero:SetMoving(false);
        hero:SetDirection(hoa_map.MapMode.NORTH);
        EventManager:StartEvent("Wolf cave entrance dialogue", 200);
    end
end

-- the map update function handles checks done on each game tick.
function Update()
	-- Check whether the character is in one of the zones
	_CheckZones();
end


-- Character creation
function _CreateCharacters()
	-- Default hero and position
	hero = CreateSprite(Map, "Bronann", 26, 46);
	hero:SetDirection(hoa_map.MapMode.NORTH);
	hero:SetMovementSpeed(hoa_map.MapMode.NORMAL_SPEED);

    if (GlobalManager:GetPreviousLocation() == "from_layna_cave_1_2") then
		hero:SetDirection(hoa_map.MapMode.EAST);
		hero:SetPosition(3, 27);
	end

	Map:AddGroundObject(hero);

    -- Create secondary character for dialogue at map entrance
    kalya_sprite = CreateSprite(Map, "Kalya",
                                hero:GetXPosition(), hero:GetYPosition());

    kalya_sprite:SetDirection(hoa_map.MapMode.NORTH);
    kalya_sprite:SetMovementSpeed(hoa_map.MapMode.NORMAL_SPEED);
    kalya_sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    kalya_sprite:SetVisible(false);
    Map:AddGroundObject(kalya_sprite);
end

-- a necklace obtained by kalya and triggering the seconde fight against the fenrir.
local necklace = {};
local necklace_npc = {};
local light_tilt = {};

local wolf = {};

-- The heal particle effect map object
local heal_effect = {};

function _CreateObjects()
	local object = {};
	local npc = {};
	local event = {};

    -- Adapt the light color according to the time of the day.
    local light_color_red = 1.0;
    local light_color_green = 1.0;
    local light_color_blue = 1.0;
    local light_color_alpha = 0.8;
    if (GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        local tw_value = GlobalManager:GetEventValue("story", "layna_forest_twilight_value");
        if (tw_value >= 4 and tw_value < 6) then
            light_color_red = 0.83;
            light_color_green = 0.72;
            light_color_blue = 0.70;
            light_color_alpha = 0.29;
        elseif (tw_value >= 6 and tw_value < 8) then
            light_color_red = 0.62;
            light_color_green = 0.50;
            light_color_blue = 0.59;
            light_color_alpha = 0.49;
        elseif (tw_value >= 8) then
            light_color_red = 0.30;
            light_color_green = 0.30;
            light_color_blue = 0.46;
            light_color_alpha = 0.60;
        end
    end

	-- Add a halo showing the cave entrances
	Map:AddHalo("img/misc/lights/torch_light_mask.lua", 28, 59,
		    hoa_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha), hoa_map.MapMode.CONTEXT_01);

    -- Add different halo light, representing holes of light coming from the ceiling
	Map:AddHalo("img/misc/lights/right_ray_light.lua", 28, 17,
		    hoa_video.Color(light_color_red, light_color_green, light_color_blue, light_color_alpha), hoa_map.MapMode.CONTEXT_01);

    -- Add the wolfpain necklace, triggering the second battle with the fenrir
    -- As this object is special, we're not using the object catalogue to only load that one once.
    necklace = hoa_map.PhysicalObject();
	necklace:SetObjectID(Map.object_supervisor:GenerateObjectID());
	necklace:SetContext(hoa_map.MapMode.CONTEXT_01);
	necklace:SetPosition(30, 9);
	necklace:SetCollHalfWidth(0.5);
	necklace:SetCollHeight(1.0);
	necklace:SetImgHalfWidth(0.5);
	necklace:SetImgHeight(1.0);
	necklace:AddAnimation("dat/maps/layna_forest/wolfpain_necklace.lua");
    Map:AddGroundObject(necklace);

    -- Adds a light tilting to catch the player attention
    light_tilt = hoa_map.PhysicalObject();
	light_tilt:SetObjectID(Map.object_supervisor:GenerateObjectID());
	light_tilt:SetContext(hoa_map.MapMode.CONTEXT_01);
    light_tilt:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
	light_tilt:SetPosition(30, 9.1);
	light_tilt:SetCollHalfWidth(0.5);
	light_tilt:SetCollHeight(1.0);
	light_tilt:SetImgHalfWidth(0.5);
	light_tilt:SetImgHeight(1.0);
	light_tilt:AddAnimation("img/misc/lights/light_reverb.lua");
    Map:AddGroundObject(light_tilt);

    -- Adds an associated npc to permit the dialogue to trigger
    necklace_npc = CreateSprite(Map, "Butterfly", 30, 9.2);
    necklace_npc:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    necklace_npc:SetVisible(false);
    necklace_npc:SetName(""); -- Unset the speaker name
    Map:AddGroundObject(necklace_npc);
    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("(Bronann looks on the ground ...)");
    dialogue:AddLineEvent(text, necklace_npc, "", "wolfpain necklace dialogue start");
    DialogueManager:AddDialogue(dialogue);
    necklace_npc:AddDialogueReference(dialogue);

    -- Place all the jewel related object out of reach when the event is already done
    -- or the wolf beaten in the crystal map.
    if (GlobalManager:DoesEventExist("story", "wolfpain_necklace_obtained") == true
            or GlobalManager:GetEventValue("story", "layna_forest_crystal_event_done") == 1) then
        necklace_npc:SetPosition(0, 0);

        light_tilt:SetVisible(false);
        necklace:SetVisible(false);
        necklace:SetPosition(0, 0);
    end

    -- The boss map sprite
	wolf = CreateSprite(Map, "Fenrir", 0, 0); -- pre place it at the right place.
	wolf:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
	wolf:SetMovementSpeed(hoa_map.MapMode.VERY_FAST_SPEED);
	wolf:SetVisible(false);
	wolf:SetDirection(hoa_map.MapMode.NORTH);
	Map:AddGroundObject(wolf);


    -- Drink at the fountain
    npc = CreateSprite(Map, "Butterfly", 53, 12);
    npc:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
    npc:SetVisible(false);
    npc:SetName(""); -- Unset the speaker name
    Map:AddGroundObject(npc);
    -- Add the dialogue options on the fountain
    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("This water looks weird. Shall we drink it anyway?");
    dialogue:AddLine(text, hero);
    text = hoa_system.Translate("...");
    dialogue:AddLine(text, npc);
    text = hoa_system.Translate("Yes, I'm so thirsty.");
    dialogue:AddOption(text, 2);
    text = hoa_system.Translate("No way, we'd get sick.");
    dialogue:AddOption(text, 3);
    -- [Line 2] Drink it
    text = hoa_system.Translate("The party drinks the water and feels... still right?");
    dialogue:AddLineEvent(text, npc, 4, "", "Fountain heal"); -- 4 = Past the dialogue lines number. Makes the dialogue ends.
    -- [Line 3] Won't drink it
    text = hoa_system.Translate("The party won't drink it.");
    dialogue:AddLine(text, npc);
	DialogueManager:AddDialogue(dialogue);
    npc:AddDialogueReference(dialogue);

    -- Load the spring heal effect.
    heal_effect = hoa_map.ParticleObject("dat/effects/particles/heal_sp_particle.lua",
                                            0, 0, hoa_map.MapMode.CONTEXT_01);
	heal_effect:SetObjectID(Map.object_supervisor:GenerateObjectID());
    heal_effect:Stop(); -- Don't run it until the character heals itself
    Map:AddGroundObject(heal_effect);
end

-- Special event references which destinations must be updated just before being called.
local move_next_to_hero_event = {}
local move_back_to_hero_event = {}
local move_next_to_hero_event2 = {}
local move_back_to_hero_event2 = {}

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
	local event = {};
	local dialogue = {};
	local text = {};

    -- Map transition events
	event = hoa_map.MapTransitionEvent("to cave 1-2", "dat/maps/layna_forest/layna_forest_cave1_2.lua", "from_layna_wolf_cave");
	EventManager:RegisterEvent(event);

	event = hoa_map.MapTransitionEvent("to south east exit", "dat/maps/layna_forest/layna_forest_south_east.lua", "from_layna_wolf_cave");
	EventManager:RegisterEvent(event);

    -- SP Heal event on fountain
	event = hoa_map.ScriptedEvent("Fountain heal", "heal_party_sp", "heal_done");
	EventManager:RegisterEvent(event);

    -- Dialogue events
    event = hoa_map.LookAtSpriteEvent("Kalya looks at Bronann", kalya_sprite, hero);
	EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Kalya looks north", kalya_sprite, hoa_map.MapMode.NORTH);
	EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Kalya looks west", kalya_sprite, hoa_map.MapMode.WEST);
	EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Kalya looks south", kalya_sprite, hoa_map.MapMode.SOUTH);
	EventManager:RegisterEvent(event);
    event = hoa_map.LookAtSpriteEvent("Bronann looks at Kalya", hero, kalya_sprite);
	EventManager:RegisterEvent(event);
    event = hoa_map.ChangeDirectionSpriteEvent("Bronann looks south", hero, hoa_map.MapMode.SOUTH);
	EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedSpriteEvent("kalya_sprite:SetCollision(NONE)", kalya_sprite, "Sprite_Collision_off", "");
	EventManager:RegisterEvent(event);
    event = hoa_map.ScriptedSpriteEvent("kalya_sprite:SetCollision(ALL)", kalya_sprite, "Sprite_Collision_on", "");
	EventManager:RegisterEvent(event);

	event = hoa_map.ScriptedEvent("Wolf cave entrance dialogue", "cave_entrance_dialogue_start", "");
    event:AddEventLinkAtEnd("Kalya moves next to Bronann", 50);
	EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_hero_event = hoa_map.PathMoveSpriteEvent("Kalya moves next to Bronann", kalya_sprite, 0, 0, false);
    move_next_to_hero_event:AddEventLinkAtEnd("kalya_sprite:SetCollision(ALL)");
    move_next_to_hero_event:AddEventLinkAtEnd("Kalya looks north");
    move_next_to_hero_event:AddEventLinkAtEnd("Kalya Tells about the smell");
    EventManager:RegisterEvent(move_next_to_hero_event);

    dialogue = hoa_map.SpriteDialogue();
	text = hoa_system.Translate("Yiek, it's stinking in here.");
	dialogue:AddLineEventEmote(text, kalya_sprite, "Bronann looks at Kalya", "Kalya looks at Bronann", "exclamation");
	text = hoa_system.Translate("Look at all those bones. We should be careful.");
	dialogue:AddLine(text, hero);
	text = hoa_system.Translate("I hope Orlinn is going well.");
	dialogue:AddLineEmote(text, kalya_sprite, "sweat drop");
	DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Kalya Tells about the smell", dialogue);
    event:AddEventLinkAtEnd("kalya_sprite:SetCollision(NONE)");
    event:AddEventLinkAtEnd("Set Camera back to Bronann");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedSpriteEvent("Set Camera back to Bronann", hero, "SetCamera", "");
    event:AddEventLinkAtEnd("kalya goes back to party");
    EventManager:RegisterEvent(event);

    move_back_to_hero_event = hoa_map.PathMoveSpriteEvent("kalya goes back to party", kalya_sprite, hero, false);
    move_back_to_hero_event:AddEventLinkAtEnd("end of cave entrance dialogue");
    EventManager:RegisterEvent(move_back_to_hero_event);

    event = hoa_map.ScriptedEvent("end of cave entrance dialogue", "end_of_cave_entrance_dialogue", "");
	EventManager:RegisterEvent(event);

    -- Wolfpain necklace dialogue
    event = hoa_map.ScriptedEvent("wolfpain necklace dialogue start", "wolfpain_necklace_dialogue_start", "");
    event:AddEventLinkAtEnd("necklace event: Kalya moves next to Bronann", 50);
	EventManager:RegisterEvent(event);

    -- NOTE: The actual destination is set just before the actual start call
    move_next_to_hero_event2 = hoa_map.PathMoveSpriteEvent("necklace event: Kalya moves next to Bronann", kalya_sprite, 0, 0, false);
    move_next_to_hero_event2:AddEventLinkAtEnd("kalya_sprite:SetCollision(ALL)");
    move_next_to_hero_event2:AddEventLinkAtEnd("Kalya looks west");
    move_next_to_hero_event2:AddEventLinkAtEnd("Kalya Tells about the necklace");
    EventManager:RegisterEvent(move_next_to_hero_event2);

    event = hoa_map.ScriptedEvent("make fenrir appear and necklace disappear", "wolf_appear_n_necklace_disappear", "");
	EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("make fenrir come", wolf, 30, 17, false);
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
	text = hoa_system.Translate("What a lovely necklace! I'll take it.");
	dialogue:AddLineEventEmote(text, kalya_sprite, "Bronann looks at Kalya", "Kalya looks north", "exclamation");
	text = hoa_system.Translate("Kalya ... We're in the middle of a cave and you're only thinking about wearing jewels.");
	dialogue:AddLine(text, hero);
	text = hoa_system.Translate("It suits me better than it would for you, don't be jealous.");
	dialogue:AddLineEvent(text, kalya_sprite, "make fenrir appear and necklace disappear", "make fenrir come");
	text = hoa_system.Translate("Kalya! You'd better be careful!");
	dialogue:AddLineEventEmote(text, hero, "Bronann looks south", "", "sweat drop");
	text = hoa_system.Translate("Don't even start, this one is all mine.");
	dialogue:AddLine(text, kalya_sprite);
	text = hoa_system.Translate("Not the necklace, the Fenrir!");
	dialogue:AddLineEmote(text, hero, "exclamation");
	text = hoa_system.Translate("You can't be serious, we would have heard it come.");
	dialogue:AddLineEvent(text, kalya_sprite, "Kalya looks at Bronann", "Kalya looks south");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Kalya Tells about the necklace", dialogue);
    event:AddEventLinkAtEnd("The Fenrir growls");
    EventManager:RegisterEvent(event);

    event = hoa_map.SoundEvent("The Fenrir growls", "snd/growl1_IFartInUrGeneralDirection_freesound.wav");
    event:AddEventLinkAtEnd("Kalya realizes for the Fenrir");
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("Ah well, finally you were serious, weren't you?");
    dialogue:AddLineEmote(text, kalya_sprite, "sweat drop");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Kalya realizes for the Fenrir", dialogue);
    event:AddEventLinkAtEnd("The Fenrir runs toward the hero");
    EventManager:RegisterEvent(event);

    event = hoa_map.PathMoveSpriteEvent("The Fenrir runs toward the hero", wolf, hero, true);
    event:AddEventLinkAtEnd("Second Fenrir battle");
    EventManager:RegisterEvent(event);

    event = hoa_map.BattleEncounterEvent("Second Fenrir battle");
    event:SetMusic("mus/accion-OGA-djsaryon.ogg");
    event:SetBackground("img/backdrops/battle/desert_cave/desert_cave.png");
    event:AddScript("dat/battles/desert_cave_battle_anim.lua");
    event:AddEnemy(7, 0, 0);
    event:AddEventLinkAtEnd("Make the fenrir disappear");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedEvent("Make the fenrir disappear", "make_wolf_invisible", "");
    event:AddEventLinkAtEnd("Get the wolfpain necklace");
    EventManager:RegisterEvent(event);

    event = hoa_map.TreasureEvent("Get the wolfpain necklace");
    event:AddObject(70003, 1); -- The wolfpain key item
    event:AddEventLinkAtEnd("Kalya talks after the battle");
    EventManager:RegisterEvent(event);

    dialogue = hoa_map.SpriteDialogue();
    text = hoa_system.Translate("It ran away again. I'm glad we survived. Let's get out of here before he comes back.");
    dialogue:AddLineEventEmote(text, kalya_sprite, "Kalya looks at Bronann", "", "sweat drop");
    DialogueManager:AddDialogue(dialogue);
    event = hoa_map.DialogueEvent("Kalya talks after the battle", dialogue);
    event:AddEventLinkAtEnd("kalya_sprite:SetCollision(NONE)");
    event:AddEventLinkAtEnd("Set Camera back to Bronann");
    EventManager:RegisterEvent(event);

    event = hoa_map.ScriptedSpriteEvent("Set Camera back to Bronann", hero, "SetCamera", "");
    event:AddEventLinkAtEnd("end of necklace dialogue");
    event:AddEventLinkAtEnd("necklace event: kalya goes back to party");
    EventManager:RegisterEvent(event);

    move_back_to_hero_event2 = hoa_map.PathMoveSpriteEvent("necklace event: kalya goes back to party", kalya_sprite, hero, false);
    EventManager:RegisterEvent(move_back_to_hero_event2);

    event = hoa_map.ScriptedEvent("end of necklace dialogue", "end_of_necklace_dialogue", "");
    EventManager:RegisterEvent(event);

end

-- zones
local to_cave_1_2_zone = {};
local to_cave_exit_zone = {};

-- Create the different map zones triggering events
function _CreateZones()
	-- N.B.: left, right, top, bottom
	to_cave_1_2_zone = hoa_map.CameraZone(0, 1, 24, 28, hoa_map.MapMode.CONTEXT_01);
	Map:AddZone(to_cave_1_2_zone);

	to_cave_exit_zone = hoa_map.CameraZone(24, 29, 47, 48, hoa_map.MapMode.CONTEXT_01);
	Map:AddZone(to_cave_exit_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
	if (to_cave_1_2_zone:IsCameraEntering() == true) then
		hero:SetMoving(false);
		EventManager:StartEvent("to cave 1-2");
    elseif (to_cave_exit_zone:IsCameraEntering() == true) then
		hero:SetMoving(false);
		EventManager:StartEvent("to south east exit");
    end
end

-- Effect time used when applying the heal light effect
local heal_effect_time = 0;

-- Map Custom functions
-- Used through scripted events
map_functions = {
    SetCamera = function(sprite)
        Map:SetCamera(sprite, 800);
    end,

    Sprite_Collision_on = function(sprite)
        if (sprite ~= nil) then
            sprite:SetCollisionMask(hoa_map.MapMode.ALL_COLLISION);
        end
    end,

    Sprite_Collision_off = function(sprite)
        if (sprite ~= nil) then
            sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);
        end
    end,

    heal_party_sp = function()
        hero:SetMoving(false);
        -- Should be sufficient to heal anybody's SP
        GlobalManager:GetActiveParty():AddSkillPoints(10000);
        AudioManager:PlaySound("snd/heal_spell.wav");
        heal_effect:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        heal_effect:Start();
        heal_effect_time = 0;
    end,

    heal_done = function()
        heal_effect_time = heal_effect_time + SystemManager:GetUpdateTime();

        if (heal_effect_time < 300.0) then
            Map:GetEffectSupervisor():EnableLightingOverlay(hoa_video.Color(0.0, 1.0, 0.0, heal_effect_time / 300.0 / 3.0 ));
            return false;
        end

        if (heal_effect_time < 1000.0) then
            Map:GetEffectSupervisor():EnableLightingOverlay(hoa_video.Color(0.0, 1.0, 0.0, ((1000.0 - heal_effect_time) / 700.0) / 3.0));
            return false;
        end
        return true;
    end,

    -- cave entrance - start event.
    cave_entrance_dialogue_start = function()
        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya_sprite:SetVisible(true);
        kalya_sprite:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        hero:SetCollisionMask(hoa_map.MapMode.ALL_COLLISION);
        kalya_sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);

        Map:SetCamera(kalya_sprite, 800);

        move_next_to_hero_event:SetDestination(hero:GetXPosition() - 2.0, hero:GetYPosition(), false);
    end,

    end_of_cave_entrance_dialogue = function()
        Map:PopState();
        kalya_sprite:SetPosition(0, 0);
        kalya_sprite:SetVisible(false);
        kalya_sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);

        -- Reload the hero back to default
        hero:ReloadSprite(main_sprite_name);
        hero:SetDirection(hoa_map.MapMode.NORTH);

        -- Set event as done
        GlobalManager:SetEventValue("story", "kalya_speech_in_wolf_cave", 1);
    end,

    -- Kalya takes the wolfpain necklace - start event.
    wolfpain_necklace_dialogue_start = function()
        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
        -- Keep a reference of the correct sprite for the event end.
        main_sprite_name = hero:GetSpriteName();

        -- Make the hero be Bronann for the event.
        hero:ReloadSprite("Bronann");

        kalya_sprite:SetVisible(true);
        kalya_sprite:SetPosition(hero:GetXPosition(), hero:GetYPosition());
        hero:SetCollisionMask(hoa_map.MapMode.ALL_COLLISION);
        kalya_sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);

        Map:SetCamera(kalya_sprite, 800);

        move_next_to_hero_event2:SetDestination(hero:GetXPosition() + 2.0, hero:GetYPosition(), false);
    end,

    wolf_appear_n_necklace_disappear = function()
        -- Make the necklace disappear
        necklace:SetVisible(false);
        necklace:SetPosition(0, 0);
        -- Prevents the event to start again
        necklace_npc:SetPosition(0, 0);
        light_tilt:SetVisible(false);

        -- Place the wolf
        wolf:SetVisible(true);
        wolf:SetPosition(27, 26);
        wolf:SetDirection(hoa_map.MapMode.NORTH);

        -- Set event as done
        GlobalManager:SetEventValue("story", "wolfpain_necklace_obtained", 1);
    end,

    make_wolf_invisible = function()
        wolf:SetVisible(false);
        wolf:SetPosition(0, 0);
    end,

    end_of_necklace_dialogue = function()
        kalya_sprite:SetPosition(0, 0);
        kalya_sprite:SetVisible(false);
        kalya_sprite:SetCollisionMask(hoa_map.MapMode.NO_COLLISION);

        -- Reload the hero back to default
        hero:ReloadSprite(main_sprite_name);
        hero:SetDirection(hoa_map.MapMode.NORTH);

        Map:PopState();
    end
}
