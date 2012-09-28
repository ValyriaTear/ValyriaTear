-- Character IDs. Each ID can have only a single bit active as IDs are used in bitmask operations.
BRONANN  = 1;
KALYA    = 2;
THANIS   = 4;
SYLVE    = 8;

-- level cap
-- Set to lvl 15 as there is only one dungeon.
GlobalManager:SetMaxExperienceLevel(15);

function NewGame()
	-- Prepare the new game data
	GlobalManager:AddCharacter(BRONANN);
	GlobalManager:SetDrunes(0);

	local MM = hoa_map.MapMode("dat/maps/layna_village/layna_village_bronanns_home_first_floor.lua");
	ModeManager:Pop(false, false);
	ModeManager:Push(MM, false, true);
end

-- Common map functions.
-- Must be declared here, or the different map functions pointers (Update, Draw) won't work
-- due to invalid calls to the structs below.
enemy_ids = {}
map_functions = {}

map_functions[0] = function()
	return true;
end
