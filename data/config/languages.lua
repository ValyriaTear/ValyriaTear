-- The languages configuration file.

-- To add a new language, create a new entry in the languages table,
-- then invoke CMake again to regenerate the po targets list.

languages = {
    -- The default language (used when translations are disabled/broken, or at first start.)
    default_locale = "en_GB",

    -- The name part of the gettext PO file for that language.
    ["en_GB"] = {
        -- The name of the language as it will appear
        -- in the Language Selection menu in the game.
        name = "English",
        -- Whether the language is using interwords space.
        interwords_spaces = true
    },
    ["de"] = {
        name = "Deutsch",
        interwords_spaces = true
    },
    ["es"] = {
        name = "Español",
        interwords_spaces = true
    },
    ["fr"] = {
        name = "Français",
        interwords_spaces = true
    },
    ["gd"] = {
        name = "Gàidhlig",
        interwords_spaces = true
    },
    ["gl"] = {
        name = "Galego",
        interwords_spaces = true
    },
    ["it"] = {
        name = "Italiano",
        interwords_spaces = true
    },
    ["ja"] = {
        name = "Japanese",
        interwords_spaces = false
    },
    ["nl_NL"] = {
        name = "Nederlands",
        interwords_spaces = true
    },
    ["pt_PT"] = {
        name = "Português",
        interwords_spaces = true
    },
    ["ru"] = {
        name = "Pусский",
        interwords_spaces = true
    },
    ["sv"] = {
        name = "Svenska",
        interwords_spaces = true
    },
    ["uk_UA"] = {
        name = "Українська",
        interwords_spaces = true
    },
    --["pt_BR"] = {
    --    name = "Português do Brasil",
    --    locale = "pt_BR",
    --    interwords_spaces = true
    --},
}
