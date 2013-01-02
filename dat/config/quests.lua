-- this is a flat list of all the quest descriptions and their titles.
-- format should be as follows: ["unique_string_id"] = {"title", "description"},

-- Use the 'GlobalManager:AddQuest("quest2","story","quest1_pen_given_done","find_pen");' luabind script command to add a quest entry in the player's quest log
-- and the '' command to set it to done.

quests = {
    -- Quest id
    ["get_barley"] = { 
        -- Title
        hoa_system.Translate("Get Some Barley Meal"),
        -- Description
        hoa_system.Translate("Need some for dinner! Flora might have some.\nI should go and have a look in her shop first.")
    },

    -- Quest id
    ["find_pen"] = {
        -- Title
        hoa_system.Translate("Find Georges' pen"),
        -- Description
        hoa_system.Translate("Georges has lost his pen. I need to get it back to him so he can tell me\nwhom he gave the barley meal to.")
    }
}
