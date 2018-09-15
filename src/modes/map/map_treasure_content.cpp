///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#include "modes/map/map_treasure_content.h"

#include "common/global/objects/global_object.h"

#include "utils/utils_common.h"

namespace vt_map
{

namespace private_map
{

MapTreasureContent::MapTreasureContent() :
    _taken(false),
    _drunes(0)
{
}

bool MapTreasureContent::AddItem(uint32_t id, uint32_t quantity)
{
    std::shared_ptr<vt_global::GlobalObject> obj = vt_global::GlobalCreateNewObject(id, quantity);
    if (obj == nullptr) {
        PRINT_WARNING << "invalid object id argument passed to function: " << id << std::endl;
        return false;
    }

    _items_list.push_back(obj);
    return true;
}

} // namespace private_map

} // namespace vt_map
