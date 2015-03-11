-- Desert cave scripted animation

local ns = {}
setmetatable(ns, {__index = _G})
desert_cave_battle_anim = ns;
setfenv(1, ns);

-- Animation members
local rock_image = nil
local fog_image = nil
local anims = {};

-- Fog related members
local fog_x_position = 300.0;
local fog_origin_x_position = 300.0;
local fog_y_position = 500.0;
local fog_alpha = 0.0;
local fog_timer;
local fog_time_length = 8000;

local Battle = nil
local Script = nil

function Initialize(battle_instance)
    Battle = battle_instance;
    Script = Battle:GetScriptSupervisor();
    -- Load the creatures animated background
    anims[0] = Script:CreateAnimation("data/battles/battle_scenes/desert_cave/desert_cave_creatures.lua");
    -- Load small eyes animations
    anims[1] = Script:CreateAnimation("data/battles/battle_scenes/desert_cave/desert_cave_eyes1.lua");
    anims[2] = Script:CreateAnimation("data/battles/battle_scenes/desert_cave/desert_cave_eyes2.lua");
    -- Load the water drop animation
    anims[3] = Script:CreateAnimation("data/battles/battle_scenes/desert_cave/desert_cave_waterdrop.lua");
    -- Load the water underground river animation
    anims[4] = Script:CreateAnimation("data/battles/battle_scenes/desert_cave/desert_cave_water.lua");

    -- Construct a timer used to display the fog with a custom alpha value and position
    fog_timer = vt_system.SystemTimer(fog_time_length, 0);
    -- Load a fog image used later to be displayed dynamically on the battle ground
    fog_image = Script:CreateImage("data/visuals/ambient/fog.png");
    fog_image:SetDimensions(320.0, 256.0);

    -- Load a rock displayed in the foreground
    rock_image = Script:CreateImage("data/battles/battle_scenes/rock.png");
    rock_image:SetDimensions(54.0, 54.0);
end


function Update()
    -- Updates the animations
    anims[0]:Update()
    anims[1]:Update()
    anims[2]:Update()
    anims[3]:Update()
    anims[4]:Update()

    -- fog
    -- Start the timer only at normal battle stage
    if ((fog_timer:IsRunning() == false)
        and (Battle:GetState() ~= vt_battle.BattleMode.BATTLE_STATE_INITIAL)) then
        fog_timer:Run();
    end
    if (fog_timer:IsFinished()) then
        fog_timer:Initialize(fog_time_length, 0);
        fog_timer:Run();
        -- Make the fog appear at random position
        fog_x_position = math.random(100.0, 700.0);
        fog_y_position = math.random(218.0, 568.0);
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

local white_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawBackground()
    -- Draw background main animations
    -- Creatures
    VideoManager:Move(814.0, 9.0);
    anims[0]:Draw(white_color)
    -- Eyes 1
    VideoManager:Move(28.0, 112.0);
    anims[1]:Draw(white_color)
    -- Eyes 2
    VideoManager:Move(503.0, 21.0);
    anims[2]:Draw(white_color);
    -- Water drop
    VideoManager:Move(200.0, 63.0);
    anims[3]:Draw(white_color);
    -- Water
    VideoManager:Move(235.0, 110.0);
    anims[4]:Draw(white_color);

    -- Draw a rock in the background
    VideoManager:Move(800.0, 248.0);
    rock_image:Draw(white_color);
end

local fog_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawForeground()
    -- Draw the rock in the foreground
    VideoManager:Move(300.0, 618.0);
    rock_image:Draw(white_color);

    -- Draw a random fog effect
    fog_color:SetAlpha(fog_alpha);
    VideoManager:Move(fog_x_position, fog_y_position);
    fog_image:Draw(fog_color);
end
