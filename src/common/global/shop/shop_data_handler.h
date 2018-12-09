////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_SHOP_DATA_HANDLER_HEADER__
#define __GLOBAL_SHOP_DATA_HANDLER_HEADER__

#include "shop_data.h"

#include "script/script_read.h"
#include "script/script_write.h"

#include <string>
#include <map>

namespace vt_global
{

class ShopDataHandler
{

public:
    ShopDataHandler() {}
    ~ShopDataHandler();

    //! \brief Clear all shop data
    void Clear();

    //! \brief Gives the shop data corresponding to the current shop id.
    //! Used to sync a given shop or save games
    const ShopData& GetShopData(const std::string& shop_id);

    bool HasShopData(const std::string& shop_id) const;

    //! \brief Sets the current shop data to global manager.
    void SetShopData(const std::string& shop_id, const ShopData& shop_data);

    /** \brief Load shop data from the save game
    *** \param file Reference to an open file for reading save game data
    **/
    void LoadShopData(vt_script::ReadScriptDescriptor& file);

    /** \brief saves the shop data information. this is called from SaveGame()
    *** \param file Reference to open and valid file for writting the data
    **/
    void SaveShopData(vt_script::WriteScriptDescriptor& file);

private:
    //! \brief A map of the curent shop data.
    //! shop_id, corresponding shop data
    std::map<std::string, ShopData> _shop_data;
};

} // namespace vt_global

#endif // __GLOBAL_SHOP_DATA_HANDLER_HEADER__
