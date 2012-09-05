------------------------------------------------------------------------------[[
-- Filename: emotes.lua
--
-- Description: This file pre-load the emote animations for the map and battle
-- characters, thus permitting them to express more their feelings.
--
-- See: http://en.wikipedia.org/wiki/Manga_iconography
------------------------------------------------------------------------------]]


local ANIM_SOUTH = hoa_map.MapMode.ANIM_SOUTH;
local ANIM_NORTH = hoa_map.MapMode.ANIM_NORTH;
local ANIM_WEST = hoa_map.MapMode.ANIM_WEST;
local ANIM_EAST = hoa_map.MapMode.ANIM_EAST;

-- All item definitions are stored in this table
if (emotes == nil) then
   emotes = {}
end


emotes = {
    ["sweat drop"] = {
         animation = "img/effects/emotes/sweat_drop.lua",
         -- emote sprite head offsets in pixels
         [ANIM_SOUTH] = { x = -7, y = 25 },
         [ANIM_NORTH] = { x = -7, y = 25 },
         [ANIM_WEST] =  { x = 3, y = 25 },
         [ANIM_EAST] =  { x = -7, y = 25 }
    },

    ["thinking dots"] = {
         animation = "img/effects/emotes/thinking_dots.lua",
         -- emote sprite head offsets in pixels
         [ANIM_SOUTH] = { x = 0, y = 0 },
         [ANIM_NORTH] = { x = 0, y = 0 },
         [ANIM_WEST] =  { x = 0, y = 0 },
         [ANIM_EAST] =  { x = 0, y = 0 }
    },

    ["interrogation"] = {
         animation = "img/effects/emotes/interrogation.lua",
         -- emote sprite head offsets in pixels
         [ANIM_SOUTH] = { x = 0, y = 0 },
         [ANIM_NORTH] = { x = 0, y = 0 },
         [ANIM_WEST] =  { x = 0, y = 0 },
         [ANIM_EAST] =  { x = 0, y = 0 }
    },

    ["exclamation"] = {
         animation = "img/effects/emotes/exclamation.lua",
         -- emote sprite head offsets in pixels
         [ANIM_SOUTH] = { x = 0, y = 0 },
         [ANIM_NORTH] = { x = 0, y = 0 },
         [ANIM_WEST] =  { x = 0, y = 0 },
         [ANIM_EAST] =  { x = 0, y = 0 }
    }
    --["cruciform popping veins"] = "img/effects/emotes/cruciform_veins.lua",
    --["dizzy spirals"] = "img/effects/emotes/dizzy_spirals.lua",
    --["sigh breath"] = "img/emotes/sigh_breath.lua",
    --["black steam of anger"] = "img/emotes/black_steam.lua"
}
