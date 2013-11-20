------------------------------------------------------------------------------[[
-- Description: Display the basic commands in a corner of the screen.
------------------------------------------------------------------------------]]

local ns = {}
setmetatable(ns, {__index = _G})
in_game_move_and_interact_anim = ns;
setfenv(1, ns);

local move_title_text = {};
local move_left_text = {};
local move_right_text = {};
local move_up_text = {};
local move_down_text = {};
local action_title_text = {};
local confirm_text = {};
local cancel_text = {};
local menu_text = {};
local game_title_text = {};
local pause_text = {};
local quit_text = {};
local help_text = {};

local display_time = 0;

local text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local move_header_color = vt_video.Color(1.0, 0.4, 0.4, 1.0);
local action_header_color = vt_video.Color(1.0, 1.0, 0.4, 1.0);
local game_header_color = vt_video.Color(0.6, 0.4, 0.0, 1.0);

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();
    Effects = Map:GetEffectSupervisor();

    display_time = 0;

    -- NOTE: a trick to add 'Left', 'Right', 'Up' and 'Down', ... to the translatable strings
    -- if not done already.
    local dummy = vt_system.Translate("Left");
    dummy = vt_system.Translate("Right");
    dummy = vt_system.Translate("Up");
    dummy = vt_system.Translate("Down");
    dummy = vt_system.Translate("Escape");
    dummy = vt_system.Translate("Space");
    dummy = vt_system.Translate("Ctrl");
    dummy = vt_system.Translate("Alt");
    dummy = vt_system.Translate("Shift");

    move_title_text = Script:CreateText(vt_system.Translate("Move your character:"), vt_video.TextStyle("text22"));
    move_left_text = Script:CreateText(vt_system.Translate(InputManager:GetLeftKeyName()), vt_video.TextStyle("text22"));
    move_right_text = Script:CreateText(vt_system.Translate(InputManager:GetRightKeyName()), vt_video.TextStyle("text22"));
    move_up_text = Script:CreateText(vt_system.Translate(InputManager:GetUpKeyName()), vt_video.TextStyle("text22"));
    move_down_text = Script:CreateText(vt_system.Translate(InputManager:GetDownKeyName()), vt_video.TextStyle("text22"));

    action_title_text = Script:CreateText(vt_system.UTranslate("Possible actions:"), vt_video.TextStyle("text22"));
    confirm_text = Script:CreateText(vt_system.VTranslate("Confirm / Open / Talk: %s", vt_system.Translate(InputManager:GetConfirmKeyName())),
                   vt_video.TextStyle("text22"));

    cancel_text = Script:CreateText(vt_system.VTranslate("Run / Cancel: %s", vt_system.Translate(InputManager:GetCancelKeyName())),
                  vt_video.TextStyle("text22"));

    menu_text = Script:CreateText(vt_system.VTranslate("Party Menu: %s", vt_system.Translate(InputManager:GetMenuKeyName())),
                                  vt_video.TextStyle("text22"));

    game_title_text = Script:CreateText(vt_system.Translate("Game commands:"), vt_video.TextStyle("text22"));
    pause_text = Script:CreateText(vt_system.VTranslate("Pause Menu: %s", vt_system.Translate(InputManager:GetPauseKeyName())),
                 vt_video.TextStyle("text22"))
    quit_text = Script:CreateText(vt_system.VTranslate("Quit Menu: %s", vt_system.Translate(InputManager:GetQuitKeyName())),
                vt_video.TextStyle("text22"));

    help_text = Script:CreateText(vt_system.VTranslate("Help Menu: %s", vt_system.Translate(InputManager:GetHelpKeyName())),
                vt_video.TextStyle("text22"));
end

function Update()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("game", "show_move_interact_info") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("game", "show_move_interact_info") == 0) then
        return;
    end

    -- Handle the timer
    display_time = display_time + SystemManager:GetUpdateTime();

    -- Apply a dark overlay first.
    local text_alpha = 1.0;
    if (display_time >= 0 and display_time <= 2500) then
        text_alpha = 1.0 * (display_time / 2500);
        text_color:SetAlpha(text_alpha);
        move_header_color:SetAlpha(text_alpha);
        action_header_color:SetAlpha(text_alpha);
        game_header_color:SetAlpha(text_alpha);
    end

end

function DrawPostEffects()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("game", "show_move_interact_info") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("game", "show_move_interact_info") == 0) then
        return;
    end

    VideoManager:Move(142.0, 148.0);
    move_title_text:Draw(move_header_color);
    VideoManager:Move(92.0, 208.0);
    move_left_text:Draw(text_color);
    VideoManager:Move(212.0, 208.0);
    move_right_text:Draw(text_color);
    VideoManager:Move(142.0, 178.0);
    move_up_text:Draw(text_color);
    VideoManager:Move(142.0, 238.0);
    move_down_text:Draw(text_color);

    VideoManager:Move(142.0, 308.0);
    action_title_text:Draw(action_header_color);

    VideoManager:Move(142.0, 458.0);
    game_title_text:Draw(game_header_color);

    -- Align the commands on the left for other languages...
    Script:SetDrawFlag(vt_video.GameVideo.VIDEO_X_LEFT);
    VideoManager:Move(32.0, 338.0);
    confirm_text:Draw(text_color);
    VideoManager:Move(32.0, 368.0);
    cancel_text:Draw(text_color);
    VideoManager:Move(32.0, 398.0);
    menu_text:Draw(text_color);
    VideoManager:Move(32.0, 488.0);
    pause_text:Draw(text_color);
    VideoManager:Move(32.0, 518.0);
    quit_text:Draw(text_color);
    VideoManager:Move(32.0, 558.0);
    help_text:Draw(text_color);
end
