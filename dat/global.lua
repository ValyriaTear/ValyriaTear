-- Character IDs. Each ID can have only a single bit active as IDs are used in bitmask operations.
BRONANN  = 1;
KALYA    = 2;
THANIS   = 4;
SYLVE    = 8;

function NewGame()
	-- Prepare the new game data
	GlobalManager:AddCharacter(BRONANN);
	GlobalManager:AddNewEventGroup("global_events"); -- this group stores the primary list of events completed in the game
	GlobalManager:SetDrunes(0);

	local MM = hoa_map.MapMode("dat/maps/vt_bronanns_room.lua");
	--local MM = hoa_map.MapMode("dat/maps/vt_bronanns_home.lua");
	--local MM = hoa_map.MapMode("dat/maps/vt_opening_village.lua");
	ModeManager:Pop(false, false);
	ModeManager:Push(MM, false, true);
end


-- Dummy functions

enemy_ids = {}
map_functions = {}

map_functions[0] = function()
	return true;
end
