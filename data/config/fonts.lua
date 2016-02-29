-- The fonts table is used to load the available fonts in game.

-- One and one only font should be set as default.

-- If no valid font is defined or if the default one is invalid, the game won't start
-- and log why.

fonts = {
    -- The font is white with a black shadow anyway.
    font_default_style = "text22",

    -- Fonts loaded for every languages.
    ["default"] = {
        -- Text style internal name = { "font file path", font size }
        -- TODO: Rename the text style to some non size dependant name.
        ["title20"] = {font = "data/fonts/LinLibertine_aBS.ttf", size = 18},
        ["title22"] = {font = "data/fonts/LinLibertine_aBS.ttf", size = 20},
        ["title24"] = {font = "data/fonts/LinLibertine_aBS.ttf", size = 22},
        ["title28"] = {font = "data/fonts/LinLibertine_aBS.ttf", size = 24},

        ["text14"] = {font = "data/fonts/LinBiolinum_RBah.ttf", size = 14},
        ["text18"] = {font = "data/fonts/LinBiolinum_RBah.ttf", size = 16},
        ["text20"] = {font = "data/fonts/LinBiolinum_RBah.ttf", size = 18},
        ["text22"] = {font = "data/fonts/LinBiolinum_RBah.ttf", size = 20},
        ["text24"] = {font = "data/fonts/LinBiolinum_RBah.ttf", size = 22},
        ["text24.2"] = {font = "data/fonts/LinBiolinum_RBah.ttf", size = 24},
        ["text26"] = {font = "data/fonts/LinBiolinum_RBah.ttf", size = 26},
        ["text28"] = {font = "data/fonts/LinBiolinum_RBah.ttf", size = 28},
        ["text36"] = {font = "data/fonts/LinBiolinum_RBah.ttf", size = 36},
        ["text48"] = {font = "data/fonts/LinBiolinum_RBah.ttf", size = 48},

        -- Map title font
        ["map_title"] = {font = "data/fonts/Berenika-Oblique.ttf", size = 30}
    },

    -- Fonts loaded for the 'ru' locale (Russian)
    ["ru"] = {
        -- Text style internal name = { "font file path", font size }
        -- The fonts listed here are appended to the default catalog when selecting the given locale,
        -- overwritting possible same named TextStyle fonts.

        -- Map title font
        -- We're using this font to prevent a bug in map title display here.
        ["map_title"] = {font = "data/fonts/LinBiolinum_RBah.ttf", size = 30}
    },

    -- Fonts loaded for the 'ja' locale (Japanese)
    ["ja"] = {
        -- Text style internal name = { "font file path", font size }
        ["title20"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 18},
        ["title22"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 20},
        ["title24"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 22},
        ["title28"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 24},

        ["text14"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 14},
        ["text18"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 16},
        ["text20"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 18},
        ["text22"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 20},
        ["text24"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 22},
        ["text24.2"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 24},
        ["text26"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 26},
        ["text28"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 28},
        ["text36"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 36},
        ["text48"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 48},

        -- Map title font
        ["map_title"] = {font = "data/fonts/SourceHanSansJP-Medium.otf", size = 30}
    }
}
