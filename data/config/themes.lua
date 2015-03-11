-- The themes table lists the files to load per theme.

-- One and one only theme should be set as default.

-- If no valid theme is defined or if the default one is invalid, the game won't start
-- and log why.

themes = {
    -- The default theme used when no settings is given, or when settings are bad.
    default_theme = "Royal Silk";

    -- Theme id = { theme name, win background file path, windows border file, cursor file }
    ["Royal Silk"] = {
        name = vt_system.Translate("Royal Silk"),
        win_background_file = "data/gui/themes/royal_silk/win_background.png",
        win_border_file = "data/gui/themes/royal_silk/win_border.png",
        cursor_file = "data/gui/themes/royal_silk/cursor.png"
    },

    ["Black Sleet"] = {
        name = vt_system.Translate("Black Sleet"),
        win_background_file = "data/gui/themes/black_sleet/black_sleet_texture.png",
        win_border_file = "data/gui/themes/black_sleet/black_sleet_skin.png",
        cursor_file = "data/gui/themes/black_sleet/cursor.png"
    },

    ["Elvish Feather"] = {
        name = vt_system.Translate("Elvish Feather"),
        win_background_file = "data/gui/themes/elvish_feather/win_background.png",
        win_border_file = "data/gui/themes/elvish_feather/win_border.png",
        cursor_file = "data/gui/themes/elvish_feather/cursor.png"
    }
}
