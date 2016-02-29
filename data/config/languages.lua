-- Add a new language by creating a new entry in the languages table.
-- The first entry after the open-parenthesis is the name of the language
-- as it will appear in the Language Selection menu in the game.
-- The second entry corresponds to the name of the gettext PO file for that
-- language.
languages = {}
languages[1] = { "English", "en_GB" } -- The default language (used when translations are disabled/broken)
languages[2] = { "Deutsch", "de" }
languages[3] = { "Dutch (Netherlands)", "nl_NL" }
languages[4] = { "Español", "es" }
languages[5] = { "Français", "fr" }
languages[6] = { "Galego", "gl" }
languages[7] = { "Italiano", "it" }
languages[7] = { "Japanese (test)", "ja" }
languages[8] = { "Português", "pt_PT" }
languages[9] = { "Pусский", "ru" }
languages[10] = { "Svenska", "sv" }
languages[11] = { "Українська", "uk_UA" }
--languages[x] = { "Português do Brasil", "pt_BR" }
