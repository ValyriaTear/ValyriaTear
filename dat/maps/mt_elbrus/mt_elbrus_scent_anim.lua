local ns = {}
setmetatable(ns, {__index = _G})
mt_elbrus_scent_anim = ns;
setfenv(1, ns);

-- Animated image members
local scent = {};

-- Other fog related members
local scent_x_position = 300.0;
local scent_y_position = 500.0;
local scent_alpha = 0.0;
local scent_timer;
local scent_time_length = 8000;

local poison_appliance_cooldown = 0;

-- c++ objects instances
local Map = {};
local Script = {};
local Effects = {};

function Initialize(map_instance)
    Map = map_instance;
    Script = Map:GetScriptSupervisor();
    Effects = Map:GetEffectSupervisor();

    -- Construct a timer used to display the scent with a custom alpha value and position
    scent_timer = vt_system.SystemTimer(scent_time_length, 0);
    -- Load a scent image used to be displayed dynamically on the map.
    scent = Script:CreateImage("img/ambient/fog.png");
    scent:SetDimensions(320.0, 256.0);

    scent_timer:Run();

    poison_appliance_cooldown = 0;
end

function _ApplyPoison()

    local index = 0;
    for index = 0, 3 do
        local char = GlobalManager:GetCharacter(index);
        if (char ~= nil and char:IsAlive() == true) then

            -- Only apply up to a moderate poison
            local intensity = Map:GetActiveStatusEffectIntensity(char, vt_global.GameGlobal.GLOBAL_STATUS_HP);
            if (intensity > vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_MODERATE) then
                -- Makes this be applied through the map mode, to make it applied correctly
                -- and shown to the player in a visual way.
                Map:ChangeActiveStatusEffect(char,
                                             vt_global.GameGlobal.GLOBAL_STATUS_HP,
                                             vt_global.GameGlobal.GLOBAL_INTENSITY_NEG_LESSER,
                                             18000); -- multiple of 9000 on purpose
                -- Give the characters a 5 seconds poison relisiance time
                -- so they're not poisoned multiple times within a few miliseconds.
                poison_appliance_cooldown = 5000;
            end
        end
    end
end


function Update()
    -- Start the timer only at normal battle stage
    if (scent_timer:IsRunning() == false) then
        scent_timer:Run();
    end

    if (scent_timer:IsFinished()) then
        scent_timer:Initialize(scent_time_length, 0);
        scent_timer:Run();
        -- Make the fog appear at random position
        scent_x_position = math.random(200.0, 700.0);
        scent_y_position = math.random(200.0, 650.0);
        scent_alpha = 0.0;
    end

    scent_timer:Update();
    -- update scent position and alpha
    -- Apply a small shifting
    scent_x_position = scent_x_position - (0.5 * scent_timer:PercentComplete());

    -- Apply parallax (the camera movement)
    scent_x_position = scent_x_position + Effects:GetCameraXMovement();
    -- Inverted y coords
    scent_y_position = scent_y_position + Effects:GetCameraYMovement();

    if (scent_timer:PercentComplete() <= 0.5) then
        -- fade in
        scent_alpha = scent_timer:PercentComplete() * 0.3 / 0.5;
    else
        -- fade out
        scent_alpha = 0.3 - (0.3 * (scent_timer:PercentComplete() - 0.5) / 0.5);
    end

    if (poison_appliance_cooldown >= 0) then
        poison_appliance_cooldown = poison_appliance_cooldown - SystemManager:GetUpdateTime();
        --print(poison_appliance_cooldown)
        return;
    end

    -- Apply potential collision effects.
    local camera = Map.camera;
    local state = Map:CurrentState();
    if (camera ~= nil and state == vt_map.MapMode.STATE_EXPLORE) then
        local x_pos = Map:GetScreenXCoordinate(camera:GetXPosition())
        local y_pos = Map:GetScreenYCoordinate(camera:GetYPosition())
        -- check using a simple bounding box, X centered, Y bottom aligned.
        if (x_pos > scent_x_position - 32.0 and x_pos < scent_x_position + 32.0 and
            y_pos > scent_y_position - 48.0 and y_pos < scent_y_position) then
            --print("colliding with "..scent_x_position..", "..scent_y_position)
            _ApplyPoison();
        end
    end
end

local scent_color = vt_video.Color(0.4, 1.0, 0.4, 0.8);

function DrawForeground()
    -- Draw a random fog effect
    scent_color:SetAlpha(scent_alpha);
    VideoManager:Move(scent_x_position, scent_y_position);
    scent:Draw(scent_color);
end
