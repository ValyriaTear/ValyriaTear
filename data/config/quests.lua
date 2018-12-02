-- this is a flat list of all the quest descriptions and their titles.
-- format should be as follows: ["unique_string_id"] =
--      {"title", "description", completion_description, completion_event_group, completion_event_name, location_name, location_banner_filename, location_subname, location_subimage},
-- When the event value of the completion_event_name in the completion_event_group is equal to 1, the quest is considered complete.
-- location_name, location_subname, location_subimage and location_banner_filename are optional fields, and they are used to display the quest start location and associated banner image
-- All fields can be empty but are required.
-- There are two more optional parameters: not_completable_event_group, not_completable_event_name
-- If those two are set, once the respective event is set to 1, the quest is considered not completable anymore
-- and a red X will be shown next to it in the menu.

-- Use the 'GlobalManager:GetGameQuests():AddQuestLog("string_id");' luabind script command to add a quest entry in the player's quest log.

quests = {
    -- Quest id
    ["get_barley"] = {
        -- Title
        vt_system.Translate("Get some barley meal"),
        -- Description
        vt_system.Translate("Mom needs some for dinner!\n \nFlora might have some in her shop. I should go and have a look there first."),
        -- Completion Description
        vt_system.Translate("Finally, I found some. Lilly had what I sought in her house the whole time."),
        -- Group and event for the quest to be complete.
        "story", "quest1_barley_meal_done",
        -- Location name and banner image filename
        vt_system.Translate("Village of Layna"), "data/story/common/locations/mountain_village.png",
        -- Location sub-name and image filename
        vt_system.Translate("Bronann's home"), "data/entities/portraits/npcs/malta.png",
    },

    -- Quest id
    ["find_pen"] = {
        -- Title
        vt_system.Translate("Find Georges's pen"),
        -- Description
        vt_system.Translate("I need to get back Georges lost pen.\n \nHe said he lost it near a tree. He also told me that people in the village might know more."),
        -- Completion Description
        vt_system.Translate("Orlinn had the pen and I ran after him all morning to get it back. I hope he won't have any problems with Kalya..."),
        -- Group and event for the quest to be complete.
        "layna_center", "quest1_pen_given_done",
        -- Location name and banner image filename
        vt_system.Translate("Village of Layna"), "data/story/common/locations/mountain_village.png",
        -- Location sub-name and image filename
        vt_system.Translate("Village center"), "data/inventory/items/key_items/ink.png",
    },

    -- Quest id
    ["hide_n_seek_with_orlinn"] = {
        -- Title
        vt_system.Translate("Catch Orlinn!"),
        -- Description
        vt_system.Translate("Orlinn has the pen that I need to return to Georges! The only problem now is I have to find out where he's hiding. Hmm, it shouldn't take long."),
        -- Completion Description
        vt_system.Translate("After chasing him around the village all morning, I finally have the pen. Time to bring it back to Georges."),
        -- Group and event for the quest to be complete.
        "layna_riverbank", "quest1_orlinn_hide_n_seek3_done",
        -- Location name and banner image filename
        vt_system.Translate("Village of Layna"), "data/story/common/locations/mountain_village.png",
        -- Location sub-name and image filename
        vt_system.Translate("Village center"), "data/entities/portraits/npcs/orlinn.png",
    },

    -- Quest id
    ["catch_chicken"] = {
        -- Title
        vt_system.Translate("Catch the chickens"),
        -- Description
        vt_system.Translate("Grandma asked me to catch the chickens that flew away. There are three of them."),
        -- Completion Description
        vt_system.Translate("I was able to find them!"),
        -- Group and event for the quest to be complete.
        "game", "layna_village_chicken_reward_given",
        -- Location name and banner image filename
        vt_system.Translate("Village of Layna"), "data/story/common/locations/mountain_village.png",
        -- Location sub-name and image filename
        vt_system.Translate("Path to Kalya's house"), "",
        -- Events telling the quest isn't completable anymore
        "story", "layna_forest_crystal_event_done"
    },

    -- Quest id
    ["bring_orlinn_back"] = {
        -- Title
        vt_system.Translate("Orlinn might be in danger!"),
        -- Description
        vt_system.Translate("After seeing that strange light, Orlinn ran into the woods. He's just a kid, even if a quick one!\n \nMy father asked me to go there with Kalya and bring him back safe and sound.\n \nI can't let them down!"),
        -- Completion Description
        vt_system.Translate("We finally retrieved Orlinn in the center of the forest. A mysterious wolf was protecting him, and we came just in time to prevent him from approaching a strange crystal."),
        -- Group and event for the quest to be complete.
        "story", "layna_forest_crystal_event_done",
        -- Location name and banner image filename
        vt_system.Translate("Village of Layna"), "data/story/common/locations/mountain_village.png",
        -- Location sub-name and image filename
        vt_system.Translate("Village center"), "data/entities/portraits/npcs/orlinn.png",
    },

    -- Quest id
    ["flee_from_the_dark_soldiers"] = {
        -- Title
        vt_system.Translate("We must escape from the soldiers."),
        -- Description
        vt_system.Translate("After we returned to the village, Kalya was caught by strange soldiers. The country lord himself provoked me in battle and thanks to her, we escaped for now.\n \nBut this isn't over. The soldiers are after us and we have to make our way through."),
        -- Completion Description
        vt_system.Translate("We managed to find a way up to the mountain top."),
        -- Group and event for the quest to be complete.
        "none", "none",
        -- Location name and banner image filename
        vt_system.Translate("Village of Layna"), "data/story/common/locations/mountain_village.png",
        -- Location sub-name and image filename
        vt_system.Translate("Kalya's home basement"), "data/entities/portraits/kalya.png",
    },
}
