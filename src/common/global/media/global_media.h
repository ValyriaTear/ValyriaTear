////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_MEDIA_HEADER__
#define __GLOBAL_MEDIA_HEADER__

#include "common/global/objects/global_object.h"
#include "common/global/objects/global_item.h"

#include "engine/video/image.h"

namespace vt_audio {
class SoundDescriptor;
}

namespace vt_global
{

/** \brief A simple class used to store commonly used media files.
*** It is used as a member of the game global class.
**/
class GlobalMedia {
public:
    GlobalMedia() {}

    ~GlobalMedia();

    //! \brief Loads all the media files.
    //! Should be called after the final intialization of the VideoManager as
    //! the texture manager is ready only afterward.
    void Initialize();

    vt_video::StillImage* GetDrunesIcon() {
        return &_drunes_icon;
    }

    vt_video::StillImage* GetStarIcon() {
        return &_star_icon;
    }

    vt_video::StillImage* GetCheckIcon() {
        return &_check_icon;
    }

    vt_video::StillImage* GetXIcon() {
        return &_x_icon;
    }

    vt_video::StillImage* GetSpiritSlotIcon() {
        return &_spirit_slot_icon;
    }

    vt_video::StillImage* GetEquipIcon() {
        return &_equip_icon;
    }

    vt_video::StillImage* GetKeyItemIcon() {
        return &_key_item_icon;
    }

    vt_video::StillImage* GetClockIcon() {
        return &_clock_icon;
    }

    vt_video::StillImage* GetBottomMenuImage() {
        return &_bottom_menu_image;
    }

    vt_video::StillImage* GetStaminaBarBackgroundImage() {
        return &_stamina_bar_background;
    }

    vt_video::StillImage* GetStaminaBarImage() {
        return &_stamina_bar;
    }

    vt_video::StillImage* GetStaminaInfiniteImage() {
        return &_stamina_bar_infinite_overlay;
    }

    std::vector<vt_video::StillImage>* GetAllItemCategoryIcons() {
        return &_all_category_icons;
    }
    std::vector<vt_video::StillImage>* GetAllSmallItemCategoryIcons() {
        return &_small_category_icons;
    }

    /** \brief Retrieves the category icon image that represents the specified object type
    *** \param object_type The type of the global object to retrieve the icon for
    *** \return A pointer to the image holding the category's icon. nullptr if the argument was invalid.
    *** \note GLOBAL_OBJECT_TOTAL will return the icon for "all wares"
    **/
    vt_video::StillImage* GetItemCategoryIcon(GLOBAL_OBJECT object_type);

    /** \brief Retrieves the category icon image that represents the specified object type
    *** \param object_type The type of the global object to retrieve the icon for
    *** \return A pointer to the image holding the category's icon. nullptr if the argument was invalid.
    *** \note GLOBAL_OBJECT_TOTAL will return the icon for "all wares"
    **/
    vt_video::StillImage* GetSmallItemCategoryIcon(ITEM_CATEGORY object_category);

    /** \brief Retrieves a specific elemental icon with the proper type and intensity
    *** \param element_type The type of element the user is trying to retrieve the icon for
    *** \param intensity The intensity level of the icon to retrieve
    *** \return The icon representation of the element type and intensity
    **/
    vt_video::StillImage* GetElementalIcon(GLOBAL_ELEMENTAL element_type, GLOBAL_INTENSITY intensity);

    /** \brief Retrieves a specific status icon with the proper type and intensity
    *** \param status_type The type of status the user is trying to retrieve the icon for
    *** \param intensity The intensity level of the icon to retrieve
    *** \return The icon representation of the status type and intensity
    **/
    vt_video::StillImage* GetStatusIcon(GLOBAL_STATUS status_type, GLOBAL_INTENSITY intensity);

    /** \brief Plays a sound object previously loaded
    *** \param identifier The string identifier for the sound to play
    **/
    void PlaySound(const std::string& identifier);

private:
    //! \brief Retains icon images for all possible object categories, including "all wares"
    std::vector<vt_video::StillImage> _all_category_icons;

    //! \brief Category icons - in a smaller size for the party menu.
    std::vector<vt_video::StillImage> _small_category_icons;

    //! \brief Image icon representing drunes (currency)
    vt_video::StillImage _drunes_icon;

    //! \brief Image icon of a single yellow/gold star
    vt_video::StillImage _star_icon;

    //! \brief Image icon of a green check mark
    vt_video::StillImage _check_icon;

    //! \brief Image icon of a red x
    vt_video::StillImage _x_icon;

    //! \brief Image icon representing open spirit slots available on weapons and armors
    vt_video::StillImage _spirit_slot_icon;

    //! \brief Image icon that represents when a character has a weapon or armor equipped
    vt_video::StillImage _equip_icon;

    //! \brief The Key item icon
    vt_video::StillImage _key_item_icon;

    //! \brief The clock icon
    vt_video::StillImage _clock_icon;

    //! \brief Image which underlays the stamina bar for running
    vt_video::StillImage _stamina_bar_background;

    //! \brief The stamina bar representing the current stamina
    vt_video::StillImage _stamina_bar;

    //! \brief Image which overlays the stamina bar to show that the player has unlimited running
    vt_video::StillImage _stamina_bar_infinite_overlay;

    //! \brief Retains all icon images that represent the game's status effects
    std::vector<vt_video::StillImage> _status_icons;

    //! \brief The battle and boot bottom image
    vt_video::StillImage _bottom_menu_image;

    //! \brief A map of the sounds used in different game modes
    std::map<std::string, vt_audio::SoundDescriptor*> _sounds;

    //! \brief Loads a sound file and add it to the sound map
    void _LoadSoundFile(const std::string& sound_name, const std::string& filename);
};

} // namespace vt_global

#endif // __GLOBAL_MEDIA_HEADER__
