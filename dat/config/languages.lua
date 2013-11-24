-- Add a new language by creating a new entry in the languages table.
-- The first entry after the open-parenthesis is the name of the language
-- as it will appear in the Language Selection menu in the game.
-- The second entry corresponds to the name of the gettext PO file for that
-- language.
languages = {}
languages[1] = { "English", "en@quot" } -- The default language (used when translations are disabled/broken)
languages[2] = { "Deutsch", "de" }
languages[3] = { "Español", "es" }
languages[4] = { "Français", "fr" }
languages[5] = { "Galego", "gl" }
languages[6] = { "Italiano", "it" }
languages[7] = { "Português", "pt_PT" }
languages[8] = { "Pусский", "ru" }
--languages[x] = { "Português do Brasil", "pt_BR" }
