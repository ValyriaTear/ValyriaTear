////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "shop_data_handler.h"

using namespace vt_utils;

namespace vt_global
{

ShopDataHandler::~ShopDataHandler()
{
    Clear();
}

void ShopDataHandler::Clear()
{
    _shop_data.clear();
}

const ShopData& ShopDataHandler::GetShopData(const std::string& shop_id) {
    if (_shop_data.find(shop_id) == _shop_data.end())
        return _shop_data[std::string()]; // Return default empty shop data
    return _shop_data.at(shop_id);
}

bool ShopDataHandler::HasShopData(const std::string& shop_id) const {
    return (_shop_data.find(shop_id) != _shop_data.end());
}

void ShopDataHandler::SetShopData(const std::string& shop_id, const ShopData& shop_data)
{
    _shop_data[shop_id] = shop_data;
}

void ShopDataHandler::LoadShopData(vt_script::ReadScriptDescriptor& file)
{
    if(file.IsFileOpen() == false) {
        PRINT_WARNING << "The file provided in the function argument was not open" << std::endl;
        return;
    }

    if (!file.OpenTable("shop_data")) {
        return;
    }

    std::vector<std::string> shop_ids;
    file.ReadTableKeys(shop_ids);

    for (size_t i = 0; i < shop_ids.size(); ++i) {
        // Open the Shop Id table
        if (!file.OpenTable(shop_ids[i]))
            continue;

        ShopData shop_data;
        if (file.OpenTable("available_buy")) {
            std::vector<std::string> item_ids;
            file.ReadTableKeys(item_ids);
            for (size_t j = 0; j < item_ids.size(); ++j) {
                uint32_t item_count = file.ReadUInt(item_ids[j]);
                shop_data._available_buy[std::stoi(item_ids[j].c_str())] = item_count;
            }
            file.CloseTable(); // available_buy
        }
        if (file.OpenTable("available_trade")) {
            std::vector<std::string> item_ids;
            file.ReadTableKeys(item_ids);
            for (size_t j = 0; j < item_ids.size(); ++j) {
                uint32_t item_count = file.ReadUInt(item_ids[j]);
                shop_data._available_trade[std::stoi(item_ids[j].c_str())] = item_count;
            }
            file.CloseTable(); // available_trade
        }
        _shop_data[ shop_ids[i] ] = shop_data;
        file.CloseTable(); // shop_id
    }
    file.CloseTable(); // shop_data
}

void ShopDataHandler::SaveShopData(vt_script::WriteScriptDescriptor& file)
{
    if(!file.IsFileOpen()) {
        PRINT_WARNING << "The file was not open: " << file.GetFilename() << std::endl;
        return;
    }

    file.WriteLine("shop_data = {");
    file.InsertNewLine();

    auto it = _shop_data.begin();
    auto it_end = _shop_data.end();
    for (; it != it_end; ++it) {
        std::string shop_id = it->first;
        const ShopData& shop_data = it->second;

        file.WriteLine("\t[\"" + shop_id + "\"] = {");

        file.WriteLine("\t\tavailable_buy = {");
        auto it2 = shop_data._available_buy.begin();
        auto it2_end = shop_data._available_buy.end();
        for(; it2 != it2_end; ++it2) {
            std::string item_id = NumberToString(it2->first);
            std::string count = NumberToString(it2->second);
            file.WriteLine("\t\t\t[\"" + item_id + "\"] = " + count + ",");
        }
        file.WriteLine("\t\t},");

        file.WriteLine("\t\tavailable_trade = {");
        auto it3 = shop_data._available_trade.begin();
        auto it3_end = shop_data._available_trade.end();
        for(; it3 != it3_end; ++it3) {
            std::string item_id = NumberToString(it3->first);
            std::string count = NumberToString(it3->second);
            file.WriteLine("\t\t\t[\"" + item_id + "\"] = " + count + ",");
        }
        file.WriteLine("\t\t}");

        file.WriteLine("\t},");
    }
    file.WriteLine("},"); // Close the shop_data table
    file.InsertNewLine();
}

} // namespace vt_global
