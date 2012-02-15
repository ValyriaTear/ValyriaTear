------------------------------------------------------------------------------[[
-- Filename: battle_events.lua
--
-- Description: This file contains the definitions of all scripted events in
-- Hero of Allacrost battles, generally to be used for boss battles.
------------------------------------------------------------------------------]]

-- All item definitions are stored in this table
if (battle_events == nil) then
	battle_events = {}
end 

battle_events[1] = {
	name = "Duel with Kyle",
	--TODO: Add dialogue!

	Before = function(bat_mode)
	end,

	During = function(bat_mode)
	end,

	After = function(bat_mode)
		bat_mode:AddDialogue("Kyle", "Do you really think it's going to matter if I take a little treasure? I just want enough to start a new life, away from this servitude. Can't you understand that?");
		bat_mode:AddDialogue("Claudius", "You’ve tarnished your honor and disgraced the Harrvahan knighthood!");
		bat_mode:AddDialogue("Kyle", "To hell with the knighthood!");
		bat_mode:ShowDialogue();
	end
}
