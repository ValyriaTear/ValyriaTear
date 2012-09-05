------------------------------------------------------------------------------[[
-- Filename: emotes.lua
--
-- Description: This file pre-load the emote animations for the map and battle
-- characters, thus permitting them to express more their feelings.
--
-- See: http://en.wikipedia.org/wiki/Manga_iconography
------------------------------------------------------------------------------]]

-- All item definitions are stored in this table
if (emotes == nil) then
   emotes = {}
end


emotes = {
    ["sweat drop"] = "img/effects/emotes/sweat_drop.lua",
    ["thinking dots"] = "img/effects/emotes/thinking_dots.lua",
    ["interrogation"] = "img/effects/emotes/interrogation.lua",
    ["exclamation"] = "img/effects/emotes/exclamation.lua"
    --["cruciform popping veins"] = "img/effects/emotes/cruciform_veins.lua",
    --["dizzy spirals"] = "img/effects/emotes/dizzy_spirals.lua",
    --["sigh breath"] = "img/emotes/sigh_breath.lua",
    --["black steam of anger"] = "img/emotes/black_steam.lua"
}
