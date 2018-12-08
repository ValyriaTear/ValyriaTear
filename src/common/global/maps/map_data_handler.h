////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_MAP_HEADER__
#define __GLOBAL_MAP_HEADER__

#include "utils/ustring.h"
#include "script/script_read.h"
#include "script/script_write.h"

#include "modes/map/map_location.h"
#include "engine/video/image.h"

//! \brief All calls to global code are wrapped inside this namespace.
namespace vt_global
{

//! \brief Stores data about the encountered maps during the game
class MapDataHandler
{
public:
    MapDataHandler();
    ~MapDataHandler();

    //! \brief Clears data about encountered maps
    void Clear();

    //! \brief Loads game map related data
    bool Load(vt_script::ReadScriptDescriptor& file);

    //! \brief Saves map related data in file
    bool Save(vt_script::WriteScriptDescriptor& file,
              uint32_t x_position,
              uint32_t y_position);

    //! \brief Tells whether the map mode minimap should be shown, if any.
    bool ShouldShowMinimap() const {
        return _show_minimap;
    }

    void ShowMinimap(bool show) {
        _show_minimap = show;
    }

    /** \brief Sets the name and graphic for the current location
    *** \param map_data_filename The string that contains the name of the current map data file.
    *** \param map_script_filename The string that contains the name of the current map script file.
    *** \param map_image_filename The filename of the image that presents this map
    *** \param map_hud_name The UTF16 map name shown at map intro time.
    **/
    void SetMap(const std::string& map_data_filename,
                const std::string& map_script_filename,
                const std::string& map_image_filename,
                const vt_utils::ustring& map_hud_name);

    const std::string& GetMapDataFilename() const {
        return _map_data_filename;
    }

    const std::string& GetMapScriptFilename() const {
        return _map_script_filename;
    }

    /** \brief Sets the active Map data filename (for game saves)
    *** \param location_name The string that contains the name of the current map data
    **/
    void SetMapDataFilename(const std::string& map_data_filename) {
        _map_data_filename = map_data_filename;
    }

    /** \brief Sets the active Map script filename (for game saves)
    *** \param location_name The string that contains the name of the current map script file
    **/
    void SetMapScriptFilename(const std::string& map_script_filename) {
        _map_script_filename = map_script_filename;
    }

    uint32_t GetSaveLocationX() const {
        return _x_save_map_position;
    }

    uint32_t GetSaveLocationY() const {
        return _y_save_map_position;
    }

    uint32_t GetSaveStamina() const {
        return _save_stamina;
    }

    void SetSaveStamina(uint32_t stamina) {
        _save_stamina = stamina;
    }

    const std::string& GetPreviousLocation() const {
        return _previous_location;
    }

    /** \brief Set up the previous map point the character is coming from.
    *** It is used to make the new map aware about where the character should appear.
    *** \param previous_location The string telling the location the character is coming from.
    **/
    void SetPreviousLocation(const std::string& previous_location) {
        _previous_location = previous_location;
    }

    /** Get the previous map hud name shown at intro time.
    *** Used to know whether the new hud name is the same in the map mode.
    **/
    bool ShouldDisplayHudNameOnMapIntro() const {
        return !_same_map_hud_name_as_previous;
    }

    //! \brief Set the new "home" map data
    void SetHomeMap(const std::string& map_data_filename,
                    const std::string& map_script_filename,
                    uint32_t x_pos, uint32_t y_pos) {
        _home_map = vt_map::MapLocation(map_data_filename,
                                        map_script_filename,
                                        x_pos, y_pos);
    }

    //! \brief Get the current "home" map data
    const vt_map::MapLocation& GetHomeMap() const {
        return _home_map;
    }

    //! \brief Clear Home map data, sometimes used by the game.
    void ClearHomeMap() {
        _home_map.Clear();
    }

    /** \brief Unset the save data once retreived at load time.
    *** It should be done in the map code once this data has been restored.
    **/
    void UnsetSaveData() {
        _x_save_map_position = 0;
        _y_save_map_position = 0;
        _save_stamina = 0;
    }

    vt_video::StillImage& GetMapImage() {
        return _map_image;
    }

    const vt_utils::ustring& GetMapHudName() const {
        return _map_hud_name;
    }

private:
    //! \brief The map data and script filename the current party is on.
    std::string _map_data_filename;
    std::string _map_script_filename;

    //! \brief last save point map tile location.
    uint32_t _x_save_map_position;
    uint32_t _y_save_map_position;

    //! \brief last save party stamina value.
    uint32_t _save_stamina;

    //! \brief The graphical image which represents the current location
    vt_video::StillImage _map_image;

    //! \brief Contains the previous "home" map location data.
    vt_map::MapLocation _home_map;

    //! \brief The map location the character is com from.
    //! Used to make the new map know where to make the character appear.
    std::string _previous_location;

    /** \brief Stores the previous and current map names appearing on screen at intro time.
    *** This is used to know whether we have to display it,
    *** as we won't when it's the same location name than the previous map.
    **/
    vt_utils::ustring _previous_map_hud_name;
    vt_utils::ustring _map_hud_name;
    bool _same_map_hud_name_as_previous;

    //! \brief Stores whether the map mode minimap should be shown.
    bool _show_minimap;
};

} // namespace vt_global

#endif // __GLOBAL_MAP_HEADER__
