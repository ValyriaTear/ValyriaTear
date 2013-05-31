-- Desert cave scripted animation

local ns = {}
setmetatable(ns, {__index = _G})
layna_forest_caves_background_anim = ns;
setfenv(1, ns);

-- Animated image members
local fog_id = -1;
local background_id = -1;

-- Other fog related members
local fog_x_position = 300.0;
local fog_y_position = 500.0;
local fog_alpha = 0.0;
local fog_timer;
local fog_time_length = 8000;

-- c++ objects instances
local Map = {};
local Script = {};
local Effects = {};

function Initialize(map_instance)
    Map = map_instance;
    Script = Map:GetScriptSupervisor();
    Effects = Map:GetEffectSupervisor();
    -- Load the creatures animated background
    background_id = Script:AddImage("img/backdrops/cave_background.png", 1024.0, 768.0);

    -- Construct a timer used to display the fog with a custom alpha value and position
    fog_timer = vt_system.SystemTimer(fog_time_length, 0);
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
        fog_x_position = math.random(300.0, 600.0);
        fog_y_position = math.random(300.0, 450.0);
        fog_alpha = 0.0;
    end
    fog_timer:Update();
    -- update fog position and alpha
    -- Apply a small shifting
    fog_x_position = fog_x_position - (0.5 * fog_timer:PercentComplete());

    -- Apply parallax (the camera movement)
    fog_x_position = fog_x_position + Effects:GetCameraXMovement();
    -- Inverted y coords
    fog_y_position = fog_y_position - Effects:GetCameraYMovement();

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
    -- Draw background animation
    Script:DrawImage(background_id, 512.0, 768.0, white_color);

end

local fog_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawForeground()
    -- Draw a random fog effect
    fog_color:SetAlpha(fog_alpha);
    Script:DrawImage(fog_id, fog_x_position,
                     fog_y_position,
                     fog_color);
end
