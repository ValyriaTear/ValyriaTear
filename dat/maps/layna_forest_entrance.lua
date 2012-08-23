-- Valyria Tear map editor begin. Do not edit this line or put anything before this line. --

-- Set the namespace according to the map name.
local ns = {};
setmetatable(ns, {__index = _G});
layna_forest_entrance = ns;
setfenv(1, ns);

-- A reference to the C++ MapMode object that was created with this file
map = {}

-- The map name, subname and location image
map_name = "Layna Forest"
map_image_filename = "img/menus/locations/layna_forest.png"
map_subname = "Forest entrance"

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
music_filename = "mus/house_in_a_forest_loop_horrorpen_oga.ogg"

-- The names of the tilesets used, with the path and file extension omitted
tileset_filenames = {}
tileset_filenames[1] = "mountain_landscape"
tileset_filenames[2] = "wood_tileset"
tileset_filenames[3] = "castle_exterior_01"
tileset_filenames[4] = "village_exterior"

-- The map grid to indicate walkability. The size of the grid is 4x the size of the tile layer tables
-- Walkability status of tiles for 32 contexts. Zero indicates walkable for all contexts. Valid range: [0:2^32-1]
-- Example: 1 (BIN 001) = wall for first context only, 2 (BIN 010) means wall for second context only, 5 (BIN 101) means Wall for first and third context.
map_grid = {}
map_grid[0] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[2] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[3] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[4] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[5] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[6] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[7] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[8] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[15] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[18] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[19] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[21] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[22] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[23] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[24] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[25] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[26] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[27] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[28] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[29] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[30] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[31] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[33] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[34] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[35] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[36] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[37] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[38] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[39] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[41] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[42] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[43] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[44] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[45] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[46] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
map_grid[47] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

-- The tile layers. The numbers are indeces to the tile_mappings table.
layers = {}
layers[0] = {}
layers[0].type = "ground"
layers[0].name = "Background"
layers[0][0] = { 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321 }
layers[0][1] = { 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321 }
layers[0][2] = { 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321 }
layers[0][3] = { 321, 321, 337, 337, 337, 257, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 337, 320, 321, 321 }
layers[0][4] = { 337, 337, 338, 289, 272, 273, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 256, 257, 289, 289, 320, 321, 321 }
layers[0][5] = { 289, 289, 289, 289, 288, 289, 289, 289, 289, 288, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 272, 273, 289, 289, 320, 321, 321 }
layers[0][6] = { 289, 289, 289, 289, 288, 289, 289, 289, 289, 289, 289, 289, 289, 289, 273, 289, 273, 289, 289, 289, 289, 288, 289, 289, 289, 273, 289, 289, 289, 336, 337, 337 }
layers[0][7] = { 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 273, 289, 289, 289, 289, 273, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289 }
layers[0][8] = { 289, 289, 289, 289, 289, 289, 288, 289, 289, 289, 288, 273, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 273, 289, 289, 289, 288, 289, 289, 289, 289, 289 }
layers[0][9] = { 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 261, 262, 262, 262, 263, 289, 288, 289, 289, 273, 289, 289, 289, 289, 273, 273, 289, 289, 289, 289, 289 }
layers[0][10] = { 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 277, 278, 278, 278, 279, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289 }
layers[0][11] = { 289, 289, 289, 289, 289, 289, 289, 289, 258, 259, 260, 293, 294, 294, 294, 295, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289 }
layers[0][12] = { 289, 273, 289, 289, 289, 289, 289, 258, 275, 275, 275, 259, 259, 259, 259, 259, 260, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289 }
layers[0][13] = { 259, 259, 259, 259, 259, 259, 259, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 259, 259, 259, 259, 259, 259, 259, 260, 289, 289, 256, 257, 289, 289, 289 }
layers[0][14] = { 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 259, 259, 259, 259, 259, 259, 259 }
layers[0][15] = { 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 291, 291, 291, 291, 291, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275 }
layers[0][16] = { 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 292, 289, 289, 289, 289, 289, 290, 291, 291, 291, 291, 291, 291, 291, 275, 275, 275, 275, 275, 275, 275, 275 }
layers[0][17] = { 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 306, 289, 289, 289, 289, 289, 289, 289, 289, 290, 291, 291, 291, 291, 291, 291, 291 }
layers[0][18] = { 289, 289, 289, 289, 289, 289, 289, 289, 289, 288, 289, 289, 289, 289, 289, 322, 289, 286, 322, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289 }
layers[0][19] = { 289, 289, 289, 289, 289, 273, 289, 289, 289, 289, 289, 272, 289, 289, 272, 338, 273, 286, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289 }
layers[0][20] = { 289, 289, 288, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 273, 289, 288, 273, 289, 289, 289, 289, 289, 289, 289, 288, 289, 289, 289 }
layers[0][21] = { 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 289, 288, 289, 289, 272, 289, 289, 289, 289, 289, 289, 289, 289, 273, 289, 289, 289, 289, 273, 289, 289, 289 }
layers[0][22] = { 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305, 305 }
layers[0][23] = { 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321, 321 }

layers[1] = {}
layers[1].type = "ground"
layers[1].name = "Background 2"
layers[1][0] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][1] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][2] = { -1, -1, -1, -1, 269, 270, 271, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 269, 270, 271, -1 }
layers[1][3] = { -1, -1, -1, -1, 285, 286, 287, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 285, 286, 287, -1 }
layers[1][4] = { -1, -1, -1, -1, 301, 302, 303, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 301, 302, 303, -1 }
layers[1][5] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 839, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][6] = { -1, -1, -1, 838, -1, -1, -1, -1, -1, -1, 838, -1, -1, -1, -1, -1, -1, 838, 823, -1, -1, -1, 839, -1, -1, -1, -1, 839, -1, -1, -1, -1 }
layers[1][7] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 578, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 828, -1, -1, -1, -1, -1, -1, 838 }
layers[1][8] = { -1, -1, -1, -1, 838, -1, -1, -1, -1, -1, -1, -1, -1, 592, -1, -1, -1, -1, 839, -1, -1, 838, -1, 823, -1, -1, -1, -1, -1, -1, 838, 838 }
layers[1][9] = { -1, -1, 838, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 608, -1, -1, -1, -1, -1, -1, -1, 823, -1, -1, -1, -1, -1, -1, -1, -1, 838, -1 }
layers[1][10] = { -1, -1, -1, -1, -1, -1, 838, -1, 823, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 821, -1, -1, -1, -1, -1, 828, -1, -1 }
layers[1][11] = { -1, 838, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 839, -1, -1, -1, 838, 823, 821, -1, 828, -1, -1, -1 }
layers[1][12] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 838, -1, -1, -1, 839, -1, -1, -1 }
layers[1][13] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][14] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][15] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][16] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][17] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 269, 270, 269, 270, 270, 271, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][18] = { -1, -1, 821, -1, -1, -1, -1, -1, -1, -1, 828, -1, -1, 285, 286, 285, 286, 269, 270, 271, 839, -1, -1, -1, -1, -1, -1, 838, -1, -1, -1, -1 }
layers[1][19] = { -1, 839, -1, -1, -1, -1, -1, 821, -1, -1, -1, -1, -1, 301, 302, 301, 302, 285, 286, 287, -1, -1, 821, -1, 823, -1, 838, -1, -1, 838, -1, 821 }
layers[1][20] = { -1, -1, -1, 828, -1, -1, 821, -1, -1, 839, 821, -1, -1, -1, -1, -1, 301, 302, 302, 303, -1, -1, -1, 839, 821, -1, -1, -1, -1, -1, 828, -1 }
layers[1][21] = { 828, -1, -1, 839, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 269, 270, 271, -1, 821, -1, -1, -1, 821, -1, -1, -1, 821, -1, -1 }
layers[1][22] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 285, 286, 287, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[1][23] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 301, 302, 303, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }

layers[2] = {}
layers[2].type = "ground"
layers[2].name = "Background 3"
layers[2][0] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][1] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][2] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][3] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][4] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][5] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[2][6] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
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
layers[3][6] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][7] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 578, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][8] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][9] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][11] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][12] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][13] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][14] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][15] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][16] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][17] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][18] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][19] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][20] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][21] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][22] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
layers[3][23] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }


-- Valyria Tear map editor end. Do not edit this line. Place your scripts after this line. --

-- the main character handler
local hero = {};

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
	_CreateEnemies();

	-- Set the camera focus on hero
	Map:SetCamera(hero);
	-- This is a dungeon map, we'll use the front battle member sprite as default sprite.
	Map.object_supervisor:SetPartyMemberVisibleSprite(hero);

	_CreateEvents();
	_CreateZones();

	-- Add clouds overlay
	Map:GetEffectSupervisor():EnableAmbientOverlay("img/ambient/clouds.png", 5.0, 5.0, true);

	Map:AddSavePoint(19, 27, hoa_map.MapMode.CONTEXT_01);
end

-- the map update function handles checks done on each game tick.
function Update()
	-- Check whether the character is in one of the zones
	_CheckZones();
end

-- Character creation
function _CreateCharacters()
	-- Default hero and position
	hero = _CreateSprite(Map, "Bronann", 3, 30);
	hero:SetDirection(hoa_map.MapMode.EAST);
	hero:SetMovementSpeed(hoa_map.MapMode.NORMAL_SPEED);
	hero:SetNoCollision(false);

	-- Load previous save point data
	local x_position = GlobalManager:GetSaveLocationX();
	local y_position = GlobalManager:GetSaveLocationY();
	if (x_position ~= 0 and y_position ~= 0) then
		-- Use the save point position, and clear the save position data for next maps
		GlobalManager:UnsetSaveLocation();
		-- Make the character look at us in that case
		hero:SetDirection(hoa_map.MapMode.SOUTH);
		hero:SetPosition(x_position, y_position);
	end

	Map:AddGroundObject(hero);

end

function _CreateObjects()
	local object = {}
	local npc = {}
	
	-- Heal point
	-- TODO: Add trigger point support
	npc = _CreateSprite(Map, "Butterfly", 27, 23);
	npc:SetNoCollision(true);
	npc:SetVisible(false);
	Map:AddGroundObject(npc);
	dialogue = hoa_map.SpriteDialogue();
	text = hoa_system.Translate("Your party feels better...");
	dialogue:AddLineEvent(text, npc, "Forest entrance heal");
	DialogueManager:AddDialogue(dialogue);
	npc:AddDialogueReference(dialogue);

	npc = _CreateSprite(Map, "Butterfly", 42, 18);
	npc:SetNoCollision(true);
	Map:AddGroundObject(npc);
	event = hoa_map.RandomMoveSpriteEvent("Butterfly1 random move", npc, 1000, 1000);
	event:AddEventLinkAtEnd("Butterfly1 random move", 4500); -- Loop on itself
	EventManager:RegisterEvent(event);
	EventManager:StartEvent("Butterfly1 random move");

	npc = _CreateSprite(Map, "Butterfly", 12, 30);
	npc:SetNoCollision(true);
	Map:AddGroundObject(npc);
	event = hoa_map.RandomMoveSpriteEvent("Butterfly2 random move", npc, 1000, 1000);
	event:AddEventLinkAtEnd("Butterfly2 random move", 4500); -- Loop on itself
	EventManager:RegisterEvent(event);
	EventManager:StartEvent("Butterfly2 random move", 2400);

	npc = _CreateSprite(Map, "Butterfly", 50, 25);
	npc:SetNoCollision(true);
	Map:AddGroundObject(npc);
	event = hoa_map.RandomMoveSpriteEvent("Butterfly3 random move", npc, 1000, 1000);
	event:AddEventLinkAtEnd("Butterfly3 random move", 4500); -- Loop on itself
	EventManager:RegisterEvent(event);
	EventManager:StartEvent("Butterfly3 random move", 1050);

	npc = _CreateSprite(Map, "Butterfly", 40, 30);
	npc:SetNoCollision(true);
	Map:AddGroundObject(npc);
	event = hoa_map.RandomMoveSpriteEvent("Butterfly4 random move", npc, 1000, 1000);
	event:AddEventLinkAtEnd("Butterfly4 random move", 4500); -- Loop on itself
	EventManager:RegisterEvent(event);
	EventManager:StartEvent("Butterfly4 random move", 3050);

	npc = _CreateSprite(Map, "Squirrel", 18, 24);
	npc:SetNoCollision(true);
	Map:AddGroundObject(npc);
	event = hoa_map.RandomMoveSpriteEvent("Squirrel1 random move", npc, 1000, 1000);
	event:AddEventLinkAtEnd("Squirrel1 random move", 4500); -- Loop on itself
	EventManager:RegisterEvent(event);
	EventManager:StartEvent("Squirrel1 random move");

	npc = _CreateSprite(Map, "Squirrel", 40, 14);
	npc:SetNoCollision(true);
	Map:AddGroundObject(npc);
	event = hoa_map.RandomMoveSpriteEvent("Squirrel2 random move", npc, 1000, 1000);
	event:AddEventLinkAtEnd("Squirrel2 random move", 4500); -- Loop on itself
	EventManager:RegisterEvent(event);
	EventManager:StartEvent("Squirrel2 random move", 1800);

	object = _CreateObject(Map, "Tree Small3", 23, 18);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 40, 14);
	Map:AddGroundObject(object);

	-- Forest entrance treasure chest
	local chest1 = _CreateTreasure(Map, "forest_entrance_chest", "Wood_Chest1", 8, 3);
	if (chest1 ~= nil) then
		chest1:SetDrunes(50);
		Map:AddGroundObject(chest1);
	end
	-- Trees above the pathway
	object = _CreateObject(Map, "Tree Small3", 3, 5);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 8, 6);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 11, 2);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 15, 4.5);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 19, 6.2);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 23, 7.2);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 26, 4);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 30, 5);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 34, 7);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 36, 8);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 39, 9);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 42, 7.5);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 45, 6);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 48, 10);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 47.5, 8);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 50, 12);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 54, 15);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 59, 17);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 62, 20);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 62, 12);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 54, 8);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 60, 4);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 55, 20);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 61, 26);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 45, 23);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 11, 20);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 2, 24);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 3, 14);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 7, 10);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 2, 8);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 4, 17);
	Map:AddGroundObject(object);

	-- Trees below the pathway
	object = _CreateObject(Map, "Tree Small3", 2, 40);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 7, 38);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 9, 42);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 4, 47);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 12, 41);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 11, 45);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 14, 46);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 16.5, 48);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 19, 46.5);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 23, 48);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 26, 39);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 36, 43);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 27, 49);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 30.5, 51);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 33, 50);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 37, 48.5);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 40.5, 51);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 44, 50.5);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 47, 49.5);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 49.5, 48.5);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 52, 50);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 55, 48);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 58, 50.5);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 58, 47);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 62, 46.5);
	Map:AddGroundObject(object);
	object = _CreateObject(Map, "Tree Small3", 62, 49);
	Map:AddGroundObject(object);
end

function _CreateEnemies()
	local enemy = {};
	local roam_zone = {};

	-- Hint: left, right, top, bottom
	roam_zone = hoa_map.EnemyZone(49, 62, 26, 39);

	enemy = CreateEnemySprite(Map, "slime");
	_SetBattleEnvironment(enemy);
	enemy:NewEnemyParty();
	enemy:AddEnemy(1);
	enemy:AddEnemy(1);
	enemy:AddEnemy(1);
	enemy:NewEnemyParty();
	enemy:AddEnemy(1);
	enemy:AddEnemy(2);
	roam_zone:AddEnemy(enemy, Map, 1);

	Map:AddZone(roam_zone);
end

-- Creates all events and sets up the entire event sequence chain
function _CreateEvents()
	local event = {};
	local dialogue = {};
	local text = {};

	-- Triggered events
	event = hoa_map.MapTransitionEvent("exit forest", "dat/maps/vt_layna_center.lua", "from_layna_forest_entrance");
	EventManager:RegisterEvent(event);
	
	-- Heal point
	event = hoa_map.ScriptedEvent("Forest entrance heal", "heal_party", "heal_done");
	EventManager:RegisterEvent(event);
end

-- Create the different map zones triggering events
function _CreateZones()
	-- N.B.: left, right, top, bottom
	forest_entrance_exit_zone = hoa_map.CameraZone(0, 1, 26, 34, hoa_map.MapMode.CONTEXT_01);
	Map:AddZone(forest_entrance_exit_zone);
end

-- Check whether the active camera has entered a zone. To be called within Update()
function _CheckZones()
	if (forest_entrance_exit_zone:IsCameraEntering() == true) then
		hero:SetMoving(false);
		EventManager:StartEvent("exit forest");
	end
end

-- Sets common battle environment settings for enemy sprites
function _SetBattleEnvironment(enemy)
	enemy:SetBattleMusicTheme("mus/Battle_Jazz.ogg");
	enemy:SetBattleBackground("img/backdrops/battle/forest_background.png");
	-- Add tutorial battle dialog with Kalya and Bronann
	enemy:AddBattleScript("dat/battles/tutorial_battle_dialogs.lua");
end

-- Map Custom functions
-- Used through scripted events
if (map_functions == nil) then
	map_functions = {}
end

local heal_effect_time = 0;

map_functions = {

    heal_party = function()
        Map:PushState(hoa_map.MapMode.STATE_SCENE);
        hero:SetMoving(false);
	-- Should be sufficient to heal anybody
	GlobalManager:GetActiveParty():AddHitPoints(10000);
	GlobalManager:GetActiveParty():AddSkillPoints(10000);
	AudioManager:PlaySound("snd/heal_spell.wav");
	Map:GetParticleManager():AddParticleEffect("dat/effects/particles/heal_particle.lua", 512.0, 390.0);
	heal_effect_time = 0;
    end,

    heal_done = function()
       heal_effect_time = heal_effect_time + SystemManager:GetUpdateTime();
       if (heal_effect_time > 2000) then
	   Map:PopState();
           return true;
       end
       return false;
    end
}
