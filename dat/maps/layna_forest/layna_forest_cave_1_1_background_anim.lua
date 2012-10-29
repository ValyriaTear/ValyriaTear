-- Desert cave scripted animation

local ns = {}
setmetatable(ns, {__index = _G})
layna_forest_cave_1_1_background_anim = ns;
setfenv(1, ns);

-- Animation members
local rock_id = -1;
local fog_id = -1;
local animation_id = -1;

-- Fog related members
local fog_x_position = 300.0;
local fog_origin_x_position = 300.0;
local fog_y_position = 500.0;
local fog_alpha = 0.0;
local fog_timer;
local fog_time_length = 8000;

function Initialize(battle_instance)
	Battle = battle_instance;
	Script = Battle:GetScriptSupervisor();
	-- Load the creatures animated background
	animation_id = Script:AddImage("img/backdrops/cave_background.png", 1024.0, 768.0);

	-- Construct a timer used to display the fog with a custom alpha value and position
	fog_timer = hoa_system.SystemTimer(fog_time_length, 0);
	-- Load a fog image used later to be displayed dynamically on the battle ground
	fog_id = Script:AddImage("img/ambient/fog.png", 320.0, 256.0);

    fog_timer:Run();
end


function Update()
	-- fog
	-- Start the timer only at normal battle stage
	if (fog_timer:IsRunning() == false) then
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
	-- Draw background animation
	Script:DrawImage(animation_id, 512.0, 768.0, hoa_video.Color(1.0, 1.0, 1.0, 1.0));

end


function DrawForeground()
	-- Draw a random fog effect
	Script:DrawImage(fog_id, fog_x_position,
						fog_y_position,
						hoa_video.Color(1.0, 1.0, 1.0, fog_alpha));
end
