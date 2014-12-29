local ns = {}
setmetatable(ns, {__index = _G})
show_smoke_cloud_script = ns;
setfenv(1, ns);

local smoke_cloud = nil
local display_time = 0;
local flash_alpha = 0.0;
local cloud_alpha = 0.0;

local smoke_sound_triggered = false;

-- c++ objects instances
local Map = nil
local Script = nil

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();
    Effects = Map:GetEffectSupervisor();

    smoke_cloud = Script:CreateImage("img/backdrops/boot/cloudfield.png");
    smoke_cloud:SetDimensions(496.0, 240.0);

    flash_alpha = 0.0;
    cloud_alpha = 0.0;

    smoke_sound_triggered = false;
end

local flash_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function Update()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("scripts_events", "layna_village_riverbank_smoke") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("scripts_events", "layna_village_riverbank_smoke") == 0) then
        return;
    end

    if (smoke_sound_triggered == false) then
        AudioManager:PlaySound("snd/steam_hisses.ogg");
        smoke_sound_triggered = true;
    end

    local time_expired = SystemManager:GetUpdateTime();

    -- Handle the timer
    display_time = display_time + time_expired;

    -- Start the timer
    if (display_time > 4300) then
        display_time = 0
        -- Disable the event at the end of it
        GlobalManager:SetEventValue("scripts_events", "layna_village_riverbank_smoke", 0);
    end

    -- The flash alpha
    if (display_time >= 0 and display_time <= 500) then
		flash_alpha = display_time / 500;
    elseif (display_time > 500 and display_time <= 1500) then
        flash_alpha = 1.0;
    elseif (display_time > 1500 and display_time <= 4000) then
        flash_alpha = 1.0 - (display_time - 1500) / (4000 - 1500);
    elseif (display_time > 4000) then
        flash_alpha = 0.0;
    end
    flash_color:SetAlpha(flash_alpha);

    -- The smoke cloud alpha
    if (display_time > 1500 and display_time <= 4100) then
        cloud_alpha = 0.9 - (display_time - 1500) / (4100 - 1500);
    elseif (display_time > 4100) then
        cloud_alpha = 0.0;
    end

    Map:GetEffectSupervisor():EnableLightingOverlay(flash_color);
end

local cloud_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawForeground()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("scripts_events", "layna_village_riverbank_smoke") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("scripts_events", "layna_village_riverbank_smoke") == 0) then
        return;
    end

    cloud_color:SetAlpha(cloud_alpha);
    VideoManager:Move(452.0 + display_time * 0.05, 424.0);
    smoke_cloud:Draw(cloud_color);
    VideoManager:Move(452.0, 424.0 + display_time * 0.05);
    smoke_cloud:Draw(cloud_color);
    VideoManager:Move(452.0 - display_time * 0.05, 424.0 - display_time * 0.02);
    smoke_cloud:Draw(cloud_color);
end
