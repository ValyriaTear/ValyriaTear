local ns = {}
setmetatable(ns, {__index = _G})
show_smoke_cloud_script = ns;
setfenv(1, ns);

local smoke_cloud_id = 0;
local display_time = 0;
local flash_alpha = 0.0;
local cloud_alpha = 0.0;

-- c++ objects instances
local Map = {};
local Script = {};

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();
    Effects = Map:GetEffectSupervisor();

    smoke_cloud_id = Script:AddImage("img/backdrops/boot/cloudfield.png", 496.0, 240.0);

    flash_alpha = 0.0;
    cloud_alpha = 0.0;
end

function Update()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("scripts_events", "layna_village_riverbank_smoke") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("scripts_events", "layna_village_riverbank_smoke") == 0) then
        return;
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

    -- The smoke cloud alpha
    if (display_time > 1500 and display_time <= 4100) then
        cloud_alpha = 0.9 - (display_time - 1500) / (4100 - 1500);
    elseif (display_time > 4100) then
        cloud_alpha = 0.0;
    end

    Map:GetEffectSupervisor():EnableLightingOverlay(vt_video.Color(1.0, 1.0, 1.0, flash_alpha));

end

function DrawForeground()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("scripts_events", "layna_village_riverbank_smoke") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("scripts_events", "layna_village_riverbank_smoke") == 0) then
        return;
    end

    Script:DrawImage(smoke_cloud_id, 452.0 + display_time * 0.05, 424.0, vt_video.Color(1.0, 1.0, 1.0, cloud_alpha));
    Script:DrawImage(smoke_cloud_id, 452.0, 424.0 + display_time * 0.05, vt_video.Color(1.0, 1.0, 1.0, cloud_alpha));
    Script:DrawImage(smoke_cloud_id, 452.0 - display_time * 0.05, 424.0 - display_time * 0.02, vt_video.Color(1.0, 1.0, 1.0, cloud_alpha));
end
