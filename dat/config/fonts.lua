-- The fonts table is used to load the available fonts in game.

-- One and one only font should be set as default.

-- If no valid font is defined or if the default one is invalid, the game won't start
-- and log why.

fonts = {
    -- Text style internal name = { "font file path", font size }
    -- TODO: Rename the text style to some non size dependant name.
    ["title20"] = {font = "img/fonts/LinLibertine_aBS.ttf", size = 18},
    ["title22"] = {font = "img/fonts/LinLibertine_aBS.ttf", size = 20},
    ["title24"] = {font = "img/fonts/LinLibertine_aBS.ttf", size = 22},
    ["title28"] = {font = "img/fonts/LinLibertine_aBS.ttf", size = 24},

    ["text18"] = {font = "img/fonts/LinBiolinum_RBah.ttf", size = 16},
    ["text20"] = {font = "img/fonts/LinBiolinum_RBah.ttf", size = 18},
    ["text22"] = {font = "img/fonts/LinBiolinum_RBah.ttf", size = 20},
    ["text24"] = {font = "img/fonts/LinBiolinum_RBah.ttf", size = 22},
    ["text24.2"] = {font = "img/fonts/LinBiolinum_RBah.ttf", size = 24},
    ["text26"] = {font = "img/fonts/LinBiolinum_RBah.ttf", size = 26},
    ["text28"] = {font = "img/fonts/LinBiolinum_RBah.ttf", size = 28},
    ["text36"] = {font = "img/fonts/LinBiolinum_RBah.ttf", size = 36},
    ["text48"] = {font = "img/fonts/LinBiolinum_RBah.ttf", size = 48},

     -- Map title font
    ["map_title"] = {font = "img/fonts/Berenika-Oblique.ttf", size = 30}
}

-- The font is white with a black shadow anyway.
font_default = "text22";
