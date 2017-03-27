///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_TREASURE_CONTENT_HEADER__
#define __MAP_TREASURE_CONTENT_HEADER__

#include "utils/utils_pch.h"

namespace vt_global {
class GlobalObject;
}

namespace vt_map
{

namespace private_map
{

class TreasureObject;

/** ****************************************************************************
*** \brief A container class for treasures procured by the player
***
*** Treasures may contain multiple quantities and types of items, weapons, armor,
*** or any other type of global object. They may additionally contain any amount
*** of drunes (money). As one would expect, the contents of a treasure should only
*** be retrieved by the player one time. This class holds a member for tracking whether
*** the treasure has been taken or not, but it is not responsible for determining
*** if a treasure was taken in the past (by a previous visit to the map or from the
*** saved game file).
*** ***************************************************************************/
class MapTreasureContent
{
    friend class TreasureSupervisor;

public:
    MapTreasureContent();

    //! \brief Sets the number of drunes present in the chest's contents.
    void SetDrunes(uint32_t amount) {
        _drunes = amount;
    }

    /** \brief Adds an item to the contents of the MapTreasure
    *** \param id The id of the GlobalObject to add
    *** \param quantity The number of the object to add (default == 1)
    *** \return True if the object was added successfully
    **/
    bool AddItem(uint32_t id, uint32_t quantity = 1);

    bool IsTaken() const {
        return _taken;
    }

    void SetTaken(bool taken) {
        _taken = taken;
    }

private:
    //! \brief Set to true when the contents of the treasure have been added to the player's inventory
    bool _taken;

    //! \brief The number of drunes contained in the chest
    uint32_t _drunes;

    //! \brief The list of objects given to the player upon opening the treasure
    std::vector<std::shared_ptr<vt_global::GlobalObject>> _items_list;
};

} // namespace private_map

} // namespace vt_map

#endif // __MAP_TREASURE_CONTENT_HEADER__
