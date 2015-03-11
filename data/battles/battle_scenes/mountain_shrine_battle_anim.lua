local ns = {}
setmetatable(ns, {__index = _G})
mountain_shrine_battle_anim = ns;
setfenv(1, ns);

local Battle = nil
local Script = nil

local fire = nil
local torch = nil
local flare = nil

function Initialize(battle_instance)
    Battle = battle_instance;
    Script = Battle:GetScriptSupervisor();

    fire = Script:CreateAnimation("img/sprites/map/objects/flame1.lua");
    fire:SetDimensions(32.0, 48.0);
    torch = Script:CreateAnimation("data/visuals/lights/torch_light_mask2.lua");
    torch:SetDimensions(340.0, 340.0);
    flare = Script:CreateImage("data/visuals/lights/sun_flare_light.png");
    flare:SetDimensions(154.0, 161.0);
end


function Update()
    -- Updates the animations
    fire:Update();
    torch:Update();
end

local torch_color = vt_video.Color(0.85, 0.32, 0.0, 0.5);
local flare_color = vt_video.Color(0.99, 1.0, 0.27, 0.3);
local white_color = vt_video.Color(1.0, 1.0, 1.0, 0.7);

function DrawBackground()
    Script:SetDrawFlag(vt_video.GameVideo.VIDEO_BLEND);
    VideoManager:Move(95.0, 25.0);
    fire:Draw(white_color);
    VideoManager:Move(688.0, 25.0);
    fire:Draw(white_color);

    Script:SetDrawFlag(vt_video.GameVideo.VIDEO_BLEND_ADD);
    VideoManager:Move(-50.0, -50.0);
    torch:Draw(torch_color);
    VideoManager:Move(540.0, -50.0);
    torch:Draw(torch_color);

    VideoManager:Move(30.0, -20.0);
    flare:Draw(flare_color);
    VideoManager:Move(620.0, -20.0);
    flare:Draw(flare_color);
end
