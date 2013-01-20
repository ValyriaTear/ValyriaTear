-- this is a flat list of all the quest descriptions and their titles.
-- format should be as follows: ["unique_string_id"] = {"title", "description", completion_event_group, completion_event_name, location_name, location_banner_filename},
-- When the event value of the completion_event_name in the completion_event_group is equal to 1, the quest is considered complete.
-- location_name and location_banner_filename are optional fields, and they are used to display the quest start location and associated banner image
-- All fields can be empty but are required.

-- Use the 'GlobalManager:AddQuestLog("string_id");' luabind script command to add a quest entry in the player's quest log.

quests = {
    -- Quest id
    ["get_barley"] = {
        -- Title
        hoa_system.Translate("Get Some Barley Meal"),
        -- Description
        hoa_system.Translate("Mom needs some for dinner!\n \nFlora might have some in her shop. I should go and have a look there first."),
        -- Group and event for the quest to be complete.
        "story", "quest1_barley_meal_done",
        -- Location name and banner image filename
        hoa_system.Translate("Village of Layna\n \nBronann's home"), "img/menus/locations/mountain_village.png"
    },

    -- Quest id
    ["find_pen"] = {
        -- Title
        hoa_system.Translate("Find Georges' pen"),
        -- Description
        hoa_system.Translate("In order to know whom Georges gave the rest of barley meal to, I need to get back his lost pen.\n \nHe spoke about losing it near a tree but also told me people in the village might know more..."),
        -- Group and event for the quest to be complete.
        "layna_center", "quest1_pen_given_done",
        -- Location name and banner image filename
        hoa_system.Translate("Village of Layna\n \nVillage center"), "img/menus/locations/mountain_village.png"
    },

    -- Quest id
    ["wants_to_go_into_the_forest"] = {
        -- Title
        hoa_system.Translate("Let's go in the woods..."),
        -- Description
        hoa_system.Translate("It's been weeks since I could go outside the village.\n \nThe entrance has been shut with stones, and now they won't even let me go and breath in the only place I could still go before?!?\n \nWhatever, I need a sword just in case and I'll get there alone!"),
        -- Group and event for the quest to be complete.
        "story", "kalya_has_joined",
        -- Location name and banner image filename
        hoa_system.Translate("Village of Layna\n \nBronann's home"), "img/menus/locations/mountain_village.png"
    },

    -- Quest id
    ["bring_orlinn_back"] = {
        -- Title
        hoa_system.Translate("Orlinn might be in danger!"),
        -- Description
        hoa_system.Translate("After seeing that strange light, Orlinn just ran away into the woods. He's just a kid, even if a quick one!\n \nMy own father asked me to go there with Kalya and get him back safe and sound.\n \nI can't let them down!"),
        -- Group and event for the quest to be complete.
        "none", "none",
        -- Location name and banner image filename
        hoa_system.Translate("Village of Layna\n \nVillage center"), "img/menus/locations/mountain_village.png"
    }
}
