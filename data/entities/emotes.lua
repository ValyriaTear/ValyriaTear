------------------------------------------------------------------------------[[
-- Filename: emotes.lua
--
-- Description: This file pre-load the emote animations for the map and battle
-- characters, thus permitting them to express more their feelings.
--
-- See: http://en.wikipedia.org/wiki/Manga_iconography
------------------------------------------------------------------------------]]


local ANIM_SOUTH = vt_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = vt_map.MapMode.ANIM_NORTH;
local ANIM_WEST = vt_map.MapMode.ANIM_WEST;
local ANIM_EAST = vt_map.MapMode.ANIM_EAST;

-- All item definitions are stored in this table
if (emotes == nil) then
   emotes = {}
end


emotes = {
    ["sweat drop"] = {
         animation = "data/entities/emotes/sweat_drop.lua",
         -- emote sprite head offsets in pixels
         [ANIM_SOUTH] = { x = -7, y = 25 },
         [ANIM_NORTH] = { x = -7, y = 25 },
         [ANIM_WEST] =  { x = 3, y = 25 },
         [ANIM_EAST] =  { x = -7, y = 25 }
    },

    ["thinking dots"] = {
         animation = "data/entities/emotes/thinking_dots.lua",
         -- emote sprite head offsets in pixels
         [ANIM_SOUTH] = { x = 0, y = 0 },
         [ANIM_NORTH] = { x = 0, y = 0 },
         [ANIM_WEST] =  { x = 0, y = 0 },
         [ANIM_EAST] =  { x = 0, y = 0 }
    },

    ["interrogation"] = {
         animation = "data/entities/emotes/interrogation.lua",
         -- emote sprite head offsets in pixels
         [ANIM_SOUTH] = { x = 0, y = 0 },
         [ANIM_NORTH] = { x = 0, y = 0 },
         [ANIM_WEST] =  { x = 0, y = 0 },
         [ANIM_EAST] =  { x = 0, y = 0 }
    },

    ["exclamation"] = {
         animation = "data/entities/emotes/exclamation.lua",
         -- emote sprite head offsets in pixels
         [ANIM_SOUTH] = { x = 0, y = 0 },
         [ANIM_NORTH] = { x = 0, y = 0 },
         [ANIM_WEST] =  { x = 0, y = 0 },
         [ANIM_EAST] =  { x = 0, y = 0 }
    },

    ["popping veins"] =  {
        animation = "data/entities/emotes/popping_veins.lua",
         -- emote sprite head offsets in pixels
         [ANIM_SOUTH] = { x = 7, y = 25 },
         [ANIM_NORTH] = { x = 7, y = 25 },
         [ANIM_WEST] =  { x = 3, y = 25 },
         [ANIM_EAST] =  { x = 7, y = 25 }

    },

    --["dizzy spirals"] = "data/entities/emotes/dizzy_spirals.lua",
    --["sigh breath"] = "data/entities/emotes/sigh_breath.lua",
    --["black steam of anger"] = "data/entities/emotes/black_steam.lua"
}
