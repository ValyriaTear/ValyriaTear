-- this is a flat file that lists all of the world locations
-- the assumption is that the calling script will set the correct map
-- to display the desired location, so this is just a large table of
-- indicies and their location display parameters
-- format is as follows: ["unique_location_id"] = {"x_location_as_string","y_location_as_string", "location_name", "location_banner_filename"}
-- x and y locations are strings because of the need to have normalized LUA data. These values are relative to the
-- world_map IMAGE, NOT to the entire screen. Thus, two locations can have the same x / y position.
-- location_id refers to the image and description ID you see at the bottom window. This is cross-referenced in another LUA config file.

world_locations = {
    -- world location id
    ["layna village"] = {
        -- X location
        "150",
        -- Y location
        "150",
        -- location name
        "Layna Village",
        -- banner filename,
        "img/menus/locations/mountain_village.png"
    },
    -- world location id
    ["The forest"] = {
        -- X location
        "200",
        -- Y location
        "100",
        -- location name
        "The Forest",
        -- banner filename,
        "img/menus/locations/layna_forest.png"
    },
}
