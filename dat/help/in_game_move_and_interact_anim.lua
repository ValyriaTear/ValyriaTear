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

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();
    Effects = Map:GetEffectSupervisor();

    display_time = 0;

    -- a trick to add 'Left', 'Right', 'Up' and 'Down', ... to the translatable strings
    -- if not done already.
    -- TODO: Move this elsewhere once a proper place has been found.
    local dummy = hoa_system.Translate("Left");
    dummy = hoa_system.Translate("Right");
    dummy = hoa_system.Translate("Up");
    dummy = hoa_system.Translate("Down");
    dummy = hoa_system.Translate("Escape");
    dummy = hoa_system.Translate("Space");
    dummy = hoa_system.Translate("Ctrl");
    dummy = hoa_system.Translate("Alt");
    dummy = hoa_system.Translate("Shift");

    move_title_text = hoa_system.Translate("Move your character:");
    move_left_text = hoa_system.Translate(InputManager:GetLeftKeyName());
    move_right_text = hoa_system.Translate(InputManager:GetRightKeyName());
    move_up_text = hoa_system.Translate(InputManager:GetUpKeyName());
    move_down_text = hoa_system.Translate(InputManager:GetDownKeyName());

    action_title_text = hoa_system.Translate("Possible actions:");
    confirm_text = hoa_system.VTranslate("Confirm / Open / Talk: %s", hoa_system.Translate(InputManager:GetConfirmKeyName()));
    cancel_text = hoa_system.VTranslate("Run / Cancel: %s", hoa_system.Translate(InputManager:GetCancelKeyName()));
    menu_text = hoa_system.VTranslate("Party Menu: %s", hoa_system.Translate(InputManager:GetMenuKeyName()));

    game_title_text = hoa_system.Translate("Game commands:");
    pause_text = hoa_system.VTranslate("Pause Menu: %s", hoa_system.Translate(InputManager:GetPauseKeyName()));
    quit_text = hoa_system.VTranslate("Quit Menu: %s", hoa_system.Translate(InputManager:GetQuitKeyName()));

    help_text = hoa_system.VTranslate("Help Menu: %s", hoa_system.Translate(InputManager:GetHelpKeyName()));
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


end

function DrawPostEffects()
    -- Only show the image if requested by the events
    if (GlobalManager:DoesEventExist("game", "show_move_interact_info") == false) then
        return;
    end

    if (GlobalManager:GetEventValue("game", "show_move_interact_info") == 0) then
        return;
    end

    -- Apply a dark overlay first.
    local text_alpha = 1.0;
    if (display_time >= 0 and display_time <= 2500) then
        text_alpha = 1.0 * (display_time / 2500);
    end

    VideoManager:DrawText(move_title_text, 142.0, 148.0, hoa_video.Color(1.0, 0.4, 0.4, text_alpha));
    VideoManager:DrawText(move_left_text, 92.0, 208.0, hoa_video.Color(1.0, 1.0, 1.0, text_alpha));
    VideoManager:DrawText(move_right_text, 212.0, 208.0, hoa_video.Color(1.0, 1.0, 1.0, text_alpha));
    VideoManager:DrawText(move_up_text, 142.0, 178.0, hoa_video.Color(1.0, 1.0, 1.0, text_alpha));
    VideoManager:DrawText(move_down_text, 142.0, 238.0, hoa_video.Color(1.0, 1.0, 1.0, text_alpha));

    VideoManager:DrawText(action_title_text, 142.0, 308.0, hoa_video.Color(1.0, 1.0, 0.4, text_alpha));

    VideoManager:DrawText(game_title_text, 142.0, 458.0, hoa_video.Color(0.6, 0.4, 0.0, text_alpha));

    -- Align the commands on the left for other languages...
    Script:SetDrawFlag(hoa_video.GameVideo.VIDEO_X_LEFT);
    VideoManager:DrawText(confirm_text, 32.0, 338.0, hoa_video.Color(1.0, 1.0, 1.0, text_alpha));
    VideoManager:DrawText(cancel_text, 32.0, 368.0, hoa_video.Color(1.0, 1.0, 1.0, text_alpha));
    VideoManager:DrawText(menu_text, 32.0, 398.0, hoa_video.Color(1.0, 1.0, 1.0, text_alpha));

    VideoManager:DrawText(pause_text, 32.0, 488.0, hoa_video.Color(1.0, 1.0, 1.0, text_alpha));
    VideoManager:DrawText(quit_text, 32.0, 518.0, hoa_video.Color(1.0, 1.0, 1.0, text_alpha));

    VideoManager:DrawText(help_text, 32.0, 558.0, hoa_video.Color(1.0, 1.0, 1.0, text_alpha));

end
