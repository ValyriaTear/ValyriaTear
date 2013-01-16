-- this is a flat list of all the quest descriptions and their titles.
-- format should be as follows: ["unique_string_id"] = {"title", "description", completion_event_group, completion_event_name},
-- When the event value of the completion_event_name in the completion_event_group is equal to 1, the quest is considered complete.

-- Use the 'GlobalManager:AddQuestLog("string_id");' luabind script command to add a quest entry in the player's quest log.

quests = {
    -- Quest id
    ["get_barley"] = {
        -- Title
        hoa_system.Translate("Get Some Barley Meal"),
        -- Description
        hoa_system.Translate("Need some for dinner!\nFlora might have some.\nI should go and have a look in her shop first."),
        -- Group and event for the quest to be complete.
        "story", "quest1_barley_meal_done"
    },

    -- Quest id
    ["find_pen"] = {
        -- Title
        hoa_system.Translate("Find Georges' pen"),
        -- Description
        hoa_system.Translate("Georges has lost his pen. I need to get it back to him so he can tell me \nwhom he gave the barley meal to."),
        -- Group and event for the quest to be complete.
        "layna_center", "quest1_pen_given_done"
    }
}
