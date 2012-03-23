-- Desert cave scripted animation

local ns = {}
setmetatable(ns, {__index = _G})
desert_cave_battle_anim = ns;
setfenv(1, ns);

-- Animation members
local rock_id = -1;
local fog_id = -1;
local anim_ids = {};

-- Fog related members
local fog_x_position = 300.0;
local fog_origin_x_position = 300.0;
local fog_y_position = 500.0;
local fog_alpha = 0.0;
local fog_timer;
local fog_time_length = 8000;

function Initialize(battle_instance)
	Battle = battle_instance;
	-- Load the creatures animated background
	anim_ids[0] = Battle:GetMedia():AddCustomAnimation("img/backdrops/battle/desert_cave/desert_cave_creatures.lua");
	-- Load small eyes animations
	anim_ids[1] = Battle:GetMedia():AddCustomAnimation("img/backdrops/battle/desert_cave/desert_cave_eyes1.lua");
	anim_ids[2] = Battle:GetMedia():AddCustomAnimation("img/backdrops/battle/desert_cave/desert_cave_eyes2.lua");
	-- Load the water drop animation
	anim_ids[3] = Battle:GetMedia():AddCustomAnimation("img/backdrops/battle/desert_cave/desert_cave_waterdrop.lua");
	-- Load the water underground river animation
	anim_ids[4] = Battle:GetMedia():AddCustomAnimation("img/backdrops/battle/desert_cave/desert_cave_water.lua");

	-- Construct a timer used to display the fog with a custom alpha value and position
	fog_timer = hoa_system.SystemTimer(fog_time_length, 0);
	-- Load a fog image used later to be displayed dynamically on the battle ground
	fog_id = Battle:GetMedia():AddCustomImage("img/ambient/fog.png", 320.0, 256.0);

	-- Load a rock displayed in the foreground
	rock_id = Battle:GetMedia():AddCustomImage("img/backdrops/battle/rock.png", 54.0, 54.0);
end


function Update()
	-- fog
	-- Start the timer only at normal battle stage
	if ((fog_timer:IsRunning() == false)
		and (Battle:GetState() ~= hoa_battle.BattleMode.BATTLE_STATE_INITIAL)) then
		fog_timer:Run();
	end
	if (fog_timer:IsFinished()) then
		fog_timer:Initialize(fog_time_length, 0);
		fog_timer:Run();
		-- Make the fog appear at random position
		fog_x_position = math.random(100.0, 700.0);
		fog_y_position = math.random(200.0, 550.0);
		fog_origin_x_position = fog_x_position;
		fog_alpha = 0.0;
	end
	fog_timer:Update();
	-- update fog position and alpha
	fog_x_position = fog_origin_x_position - (100 * fog_timer:PercentComplete());
	if (fog_timer:PercentComplete() <= 0.5) then
		-- fade in
		fog_alpha = fog_timer:PercentComplete() * 0.3 / 0.5;
	else
		-- fade out
		fog_alpha = 0.3 - (0.3 * (fog_timer:PercentComplete() - 0.5) / 0.5);
	end
end


function DrawBackground()
	-- Draw background main animations
	-- Creatures
	Battle:GetMedia():DrawCustomAnimation(anim_ids[0], 814.0, 769.0 - 9.0);
	-- Eyes 1
	Battle:GetMedia():DrawCustomAnimation(anim_ids[1], 28.0, 769.0 - 112.0);
	-- Eyes 2
	Battle:GetMedia():DrawCustomAnimation(anim_ids[2], 503.0, 769.0 - 21.0);
	-- Water drop
	Battle:GetMedia():DrawCustomAnimation(anim_ids[3], 200.0, 769.0 - 63.0);
	-- Water
	Battle:GetMedia():DrawCustomAnimation(anim_ids[4], 235.0, 769.0 - 110.0);

	-- Draw a rock in the background
	Battle:GetMedia():DrawCustomImage(rock_id, 800.0, 520.0, hoa_video.Color(1.0, 1.0, 1.0, 1.0));
end


function DrawForeground()
	-- Draw the rock in the foreground
	Battle:GetMedia():DrawCustomImage(rock_id, 300.0, 150.0, hoa_video.Color(1.0, 1.0, 1.0, 1.0));
end


function DrawEffects()
	-- Draw a random fog effect
	Battle:GetMedia():DrawCustomImage(fog_id, fog_x_position,
										fog_y_position,
										hoa_video.Color(1.0, 1.0, 1.0, fog_alpha));
end
