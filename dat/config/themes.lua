-- The themes table lists the files to load per theme.

-- One and one only theme should be set as default.

-- If no valid theme is defined or if the default one is invalid, the game won't start
-- and log why.

themes = {
    -- Theme name = { win background file path, windows border file, cursor file }
    ["Royal Silk"] = {
        win_background_file = "img/menus/themes/royal_silk/win_background.png",
        win_border_file = "img/menus/themes/royal_silk/win_border.png",
        cursor_file = "img/menus/themes/royal_silk/cursor.png"
    },

    ["Black Sleet"] = {
        win_background_file = "img/menus/themes/black_sleet/black_sleet_texture.png",
        win_border_file = "img/menus/themes/black_sleet/black_sleet_skin.png",
        cursor_file = "img/menus/themes/black_sleet/cursor.png"
    }
}

-- The default theme used when no settings is given, or when settings are bad.
default_theme = "Royal Silk";
