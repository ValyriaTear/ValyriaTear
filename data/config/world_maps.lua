-- this is a flat file that lists all of the world map and their locations
-- the assumption is that the calling script will set the correct map
-- to display the desired location, so this is just a large table of
-- indices and their location display parameters
-- format is as follows: ["unique_location_id"] = {"x_location_as_string","y_location_as_string", "location_name", "location_banner_filename"}
-- x and y locations are strings because of the need to have normalized LUA data. These values are relative to the
-- world_map IMAGE, NOT to the entire screen. Thus, two locations can have the same x / y position.
-- location_id refers to the image and description ID you see at the bottom window. This is cross-referenced in another LUA config file.

world_maps = {
    ["Layna present"] = {
      image = "data/story/common/world_maps/world_map_layna_present.png",
      map_locations = {
          -- world location id
          ["layna village"] = {
              -- X location
              "722",
              -- Y location
              "333",
              -- location name
              vt_system.Translate("Village of Layna"),
              -- banner filename,
              "data/story/common/locations/mountain_village.png"
          },
          -- world location id
          ["layna forest"] = {
              -- X location
              "761",
              -- Y location
              "328",
              -- location name
              vt_system.Translate("Layna Forest"),
              -- banner filename,
              "data/story/common/locations/layna_forest.png"
          },
          -- world location id
          ["mt elbrus"] = {
              -- X location
              "710",
              -- Y location
              "243",
              -- location name
              vt_system.Translate("Mt. Elbrus"),
              -- banner filename,
              "data/story/common/locations/mt_elbrus.png"
          },
      }
    }
}
