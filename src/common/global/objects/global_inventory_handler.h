////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_INVENTORY_HANDLER_HEADER__
#define __GLOBAL_INVENTORY_HANDLER_HEADER__

#include "global_armor.h"
#include "global_item.h"
#include "global_spirit.h"
#include "global_weapon.h"

#include "script/script_write.h"

namespace vt_global
{

class InventoryHandler
{
public:
    InventoryHandler()
    {}

    ~InventoryHandler();

    //! \brief Handles lua script loading. This provides access to the lua data.
    bool LoadScripts();
    void CloseScripts();

    /** \brief Deletes all data stored within the GameGlobal class object
    *** This function is meant to be called when the user quits the current game instance
    *** and returns to the boot screen. It will delete all characters, inventory, and other
    *** data relevant to the current game.
    **/
    void ClearAllData();

    /** \brief Adds a new object to the inventory
    *** \param obj_id The identifier value of the object to add
    *** \param obj_count The number of instances of the object to add (default == 1)
    *** If the item already exists in the inventory, then instead the GlobalObject::_count member is used to
    *** increment the count of the stored item.
    **/
    void AddToInventory(uint32_t obj_id, uint32_t obj_count = 1);

    /** \brief Adds a new object to the inventory
    *** \param object A pointer to the pre-created GlobalObject-type class to add
    **/
    void AddToInventory(const std::shared_ptr<GlobalObject>& object);

    /** \brief Removes an object from the inventory
    *** \param obj_id The identifier value of the object to remove
    *** \note If the object is not in the inventory, the function will do nothing.
    ***
    *** This function removes the item regardless of what the GlobalObject#_count member is set to.
    *** If you want to remove only a certain number of instances of the object, use the function
    *** GameGlobal#DecrementItemCount.
    **/
    void RemoveFromInventory(uint32_t obj_id);

    /** \brief Gets a copy of an object from the inventory
    *** \param obj_id The identifier value of the item to obtain
    *** \return A newly instantiated copy of the object, or nullptr if the object was not found in the inventory
    **/
    std::shared_ptr<GlobalObject> GetGlobalObject(uint32_t obj_id);

    /** \brief Increments the number (count) of an object in the inventory
    *** \param item_id The integer identifier of the item that will have its count incremented
    *** \param count The amount to increase the object's count by (default value == 1)
    ***
    *** If the item does not exist in the inventory, this function will do nothing. If the count parameter
    *** is set to zero, no change will take place.
    ***
    *** \note The callee can not assume that the function call succeeded, but rather has to check this themselves.
    **/
    void IncrementItemCount(uint32_t obj_id, uint32_t obj_count = 1);

    /** \brief Decrements the number (count) of an object in the inventory
    *** \param item_id The integer identifier of the item that will have its count decremented
    *** \param count The amount to decrease the object's count by (default value == 1)
    ***
    *** If the item does not exist in the inventory, this function will do nothing. If the count parameter
    *** is set to zero, no change will take place. If the count parameter is greater than or equal to the
    *** current count of the object, the object will be completely removed from the inventory.
    ***
    *** \note The callee can not assume that the function call succeeded, but rather has to check this themselves.
    **/
    void DecrementItemCount(uint32_t obj_id, uint32_t obj_count = 1);

    /** \brief Checks whether or a given object is currently stored in the inventory
    *** \param id The id of the object (item, weapon, armor, etc.) to check for
    *** \return True if the object was found in the inventor, or false if it was not found
    **/
    bool IsItemInInventory(uint32_t id) {
        return (_inventory.find(id) != _inventory.end());
    }

    /** \brief Gives how many of a given item is in the inventory
    *** \param id The id of the object (item, weapon, armor, etc.) to check for
    *** \return The number of the object found in the inventory
    **/
    uint32_t HowManyObjectsInInventory(uint32_t id) {
        return (_inventory.find(id) != _inventory.end()) ? _inventory.at(id)->GetCount() : 0;
    }

    void LoadInventory(vt_script::ReadScriptDescriptor& file);
    void SaveInventory(vt_script::WriteScriptDescriptor& file);

    std::map<uint32_t, std::shared_ptr<GlobalObject>>& GetInventory() {
        return _inventory;
    }

    std::vector<std::shared_ptr<GlobalItem>>& GetInventoryItems() {
        return _inventory_items;
    }

    std::vector<std::shared_ptr<GlobalWeapon>>& GetInventoryWeapons() {
        return _inventory_weapons;
    }

    //! \brief Returns the armor inventory depending on the item type.
    std::vector<std::shared_ptr<GlobalArmor>>& GetInventoryArmors(GLOBAL_OBJECT object_type);

    std::vector<std::shared_ptr<GlobalSpirit>>& GetInventorySpirits() {
        return _inventory_spirits;
    }

    std::vector<std::shared_ptr<GlobalObject>>& GetInventoryKeyItems() {
        return _inventory_key_items;
    }

    vt_script::ReadScriptDescriptor &GetItemsScript() {
        return _items_script;
    }

    vt_script::ReadScriptDescriptor &GetWeaponsScript() {
        return _weapons_script;
    }

    vt_script::ReadScriptDescriptor &GetHeadArmorScript() {
        return _head_armor_script;
    }

    vt_script::ReadScriptDescriptor &GetTorsoArmorScript() {
        return _torso_armor_script;
    }

    vt_script::ReadScriptDescriptor &GetArmArmorScript() {
        return _arm_armor_script;
    }

    vt_script::ReadScriptDescriptor &GetLegArmorScript() {
        return _leg_armor_script;
    }

    vt_script::ReadScriptDescriptor &GetSpiritsScript() {
        return _spirits_script;
    }

private:

    /** \brief Retains a list of all of the objects currently stored in the player's inventory
    *** This map is used to quickly check if an item is in the inventory or not. The key to the map is the object's
    *** identification number. When an object is added to the inventory, if it already exists then the object counter
    *** is simply increased instead of adding an entire new class object. When the object count becomes zero, the object
    *** is removed from the inventory. Duplicates of all objects are retained in the various inventory containers below.
    **/
    std::map<uint32_t, std::shared_ptr<GlobalObject>> _inventory;

    /** \brief Inventory containers
    *** These vectors contain the inventory of the entire party. The vectors are sorted according to the player's personal preferences.
    *** When a new object is added to the inventory, by default it will be placed at the end of the vector.
    **/
    //@{
    std::vector<std::shared_ptr<GlobalItem>>    _inventory_items;
    std::vector<std::shared_ptr<GlobalWeapon>>  _inventory_weapons;
    std::vector<std::shared_ptr<GlobalArmor>>   _inventory_head_armors;
    std::vector<std::shared_ptr<GlobalArmor>>   _inventory_torso_armors;
    std::vector<std::shared_ptr<GlobalArmor>>   _inventory_arm_armors;
    std::vector<std::shared_ptr<GlobalArmor>>   _inventory_leg_armors;
    std::vector<std::shared_ptr<GlobalSpirit>>  _inventory_spirits;

    //! \brief The key items can be any kind of items.
    std::vector<std::shared_ptr<GlobalObject>> _inventory_key_items;
    //@}

    //! \brief Contains data definitions for all items
    vt_script::ReadScriptDescriptor _items_script;

    //! \brief Contains data definitions for all weapons
    vt_script::ReadScriptDescriptor _weapons_script;

    //! \brief Contains data definitions for all armor that are equipped on the head
    vt_script::ReadScriptDescriptor _head_armor_script;

    //! \brief Contains data definitions for all armor that are equipped on the torso
    vt_script::ReadScriptDescriptor _torso_armor_script;

    //! \brief Contains data definitions for all armor that are equipped on the arms
    vt_script::ReadScriptDescriptor _arm_armor_script;

    //! \brief Contains data definitions for all armor that are equipped on the legs
    vt_script::ReadScriptDescriptor _leg_armor_script;

    //! \brief Contains data definitions for all spirits
    vt_script::ReadScriptDescriptor _spirits_script;

    /** \brief A helper template function that finds and removes an object from the inventory
    *** \param obj_id The ID of the object to remove from the inventory
    *** \param inv The vector container of the appropriate inventory type
    *** \return True if the object was successfully removed, or false if it was not
    **/
    template <class T> bool _RemoveFromInventory(uint32_t obj_id, std::vector<std::shared_ptr<T>>& inv);

    /** \brief A helper template function that finds and returns a copy of an object from the inventory
    *** \param obj_id The ID of the object to obtain from the inventory
    *** \param inv The vector container of the appropriate inventory type
    *** \return A pointer to the newly created copy of the object, or nullptr if the object could not be found
    **/
    template <class T> std::shared_ptr<T> _GetFromInventory(uint32_t obj_id, const std::vector<std::shared_ptr<T>>& inv);

    /** \brief A helper function to GameGlobal::SaveGame() that stores the contents of a type of inventory to the saved game file
    *** \param file A reference to the open and valid file where to write the inventory list
    *** \param category_name The name under which this set of inventory data should be categorized (ie "items", "weapons", etc)
    *** \param inv A reference to the inventory vector to store
    *** \note The class type T must be a derived class of GlobalObject
    **/
    template <class T> void _SaveInventory(vt_script::WriteScriptDescriptor& file,
                                           const std::string& category_name,
                                           const std::vector<std::shared_ptr<T>>& inv);

    /** \brief A helper function to GameGlobal::LoadGame() that restores the contents of the inventory from a saved game file
    *** \param file A reference to the open and valid file from where to read the inventory list
    *** \param category_name The name of the table in the file that should contain the inventory for a specific category
    **/
    void _LoadInventory(vt_script::ReadScriptDescriptor& file, const std::string& category_name);

};

template <class T> bool InventoryHandler::_RemoveFromInventory(uint32_t obj_id,
                                                               std::vector<std::shared_ptr<T>>& inv)
{
    for (auto i = inv.begin(); i != inv.end(); i++) {
        if ((*i)->GetID() == obj_id) {
            inv.erase(i);
            _inventory.erase(obj_id);
            return true;
        }
    }

    return false;
}

template <class T> std::shared_ptr<T> InventoryHandler::_GetFromInventory(uint32_t obj_id,
                                                                          const std::vector<std::shared_ptr<T>>& inv)
{
    for (auto it = inv.begin(); it != inv.end(); ++it) {

        if ((*it)->GetID() != obj_id)
            continue;

        auto return_object = std::make_shared<T>(**it);
        return_object->SetCount(1);
        return return_object;
    }

    return nullptr;
}

template <class T> void InventoryHandler::_SaveInventory(vt_script::WriteScriptDescriptor& file,
                                                         const std::string& category_name,
                                                         const std::vector<std::shared_ptr<T>>& inv)
{
    if (file.IsFileOpen() == false) {
        PRINT_WARNING << "failed because the argument file was not open" << std::endl;
        return;
    }

    file.InsertNewLine();
    file.WriteLine(category_name + " = {");

    for (uint32_t i = 0; i < inv.size(); i++) {
        // Don't save inventory items with 0 count
        if (inv[i]->GetCount() == 0)
            continue;

        if (i == 0)
            file.WriteLine("\t", false);
        else
            file.WriteLine(", ", false);

        // Add a new line every 10 entries for better readability and debugging
        if ((i > 0) && !(i % 10)) {
            file.InsertNewLine();
            file.WriteLine("\t", false);
        }

        file.WriteLine("[" + vt_utils::NumberToString(inv[i]->GetID()) + "] = " +
                       vt_utils::NumberToString(inv[i]->GetCount()), false);
    }

    file.InsertNewLine();
    file.WriteLine("},");
}

} // namespace vt_global

#endif // __GLOBAL_INVENTORY_HANDLER_HEADER__
