////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __SHOP_DATA_HEADER__
#define __SHOP_DATA_HEADER__

#include <map>

namespace vt_global {

//! \brief Shop save data.
//! This structure stores data from shops state to permit saving those states,
//! but also to sync the shop content when loading the game.
struct ShopData {
    ShopData() {}
    ~ShopData() {}

    //! \brief The list of items in the shop and their number, or 0 if infinite.
    //! \NOTE The item is not listed there if the count is 0.
    //! item id, count
    std::map<uint32_t, uint32_t> _available_buy;
    std::map<uint32_t, uint32_t> _available_trade;
};

} //namespace vt_global

#endif // __SHOP_DATA_HEADER__
