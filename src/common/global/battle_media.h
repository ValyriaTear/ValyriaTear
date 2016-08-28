////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_utils.h
*** \author  Tyler Olsen, rootslinux@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for global game utility code
***
*** This file contains several common constants, enums, and functions that are
*** used by various global classes.
*** ***************************************************************************/

#ifndef __BATTLE_MEDIA_HEADER__
#define __BATTLE_MEDIA_HEADER__

#include "global_utils.h"

#include "engine/video/image.h"
#include "engine/video/text.h"

#include "engine/audio/audio_descriptor.h"

namespace vt_global {

/** ****************************************************************************
*** \brief A specialized class to BattleMode that holds various related  multimedia data
***
*** Many of the battle mode interfaces require access to a common set of media data.
*** This class retains all of this common media data and makes it available for these
*** classes to utilize. It also serves to reduce the number of methods and members of
*** the BattleMode class.
***
*** \note Although most of the images and audio data here are public, you should take
*** extreme care when modifying any of the properties of this data, such as loading out
*** a different image or changing its size, as this could have implications for other
*** battle classes that also use this data.
*** ***************************************************************************/
class BattleMedia
{
public:
    BattleMedia() :
        _auto_battle_activated(nullptr)
    {
    }

    ~BattleMedia();

    //! \brief Loads all the battle media files.
    //! Should be called after the final intialization of the VideoManager as
    //! the texture manager is ready only afterward.
    void Initialize();

    ///! \brief Updates the different animations and media
    void Update();

    /** \brief Sets the background image for the battle
    *** \param filename The filename of the new background image to load
    **/
    void SetBackgroundImage(const std::string& filename);

    /** \brief Sets the battle music to use
    *** \param filename The full filename of the music to play
    **/
    void SetBattleMusic(const std::string& filename);

    /** \brief Retrieves a specific button icon for character action
    *** \param index The index of the button to retrieve
    *** \return A pointer to the appropriate button image, or nullptr if the index argument was out of bounds
    **/
    vt_video::StillImage* GetCharacterActionButton(uint32_t index);

    /** \brief Retrieves the appropriate icon image given a valid target type
    *** \param target_type The enumerated value that represents the type of target
    *** \return A pointer to the appropriate icon image, or nullptr if the target type was invalid
    **/
    vt_video::StillImage* GetTargetTypeIcon(vt_global::GLOBAL_TARGET target_type);

    inline const vt_video::StillImage& GetStunnedIcon() const {
        return _stunned_icon;
    }

    inline const vt_video::StillImage& GetAutoBattleIcon() const {
        return _auto_battle_icon;
    }

    inline const vt_video::TextImage* GetAutoBattleActiveText() const {
        return _auto_battle_activated;
    }

    inline const vt_video::StillImage& GetEscapeIcon() const {
        return _escape_icon;
    }

    // ---------- Public members

    //! \brief The static background image to be used for the battle
    vt_video::StillImage background_image;

    //! \brief The static image that is drawn for the bottom menus
    vt_video::StillImage bottom_menu_image;

    /** \brief An image that indicates that a particular actor has been selected
    *** This image best suites character sprites and enemy sprites of similar size. It does not work
    *** well with larger or smaller sprites.
    **/
    vt_video::StillImage actor_selection_image;

    /** \brief An image that points out the location of specific attack points on an actor
    *** This image may be used for both character and enemy actors. It is used to indicate an actively selected
    *** attack point, <b>not</b> just any attack points present.
    **/
    vt_video::AnimatedImage attack_point_indicator;

    //! \brief Used to provide a background highlight for a selected character
    vt_video::StillImage character_selected_highlight;

    //! \brief Used to provide a background highlight for a character that needs a command set
    vt_video::StillImage character_command_highlight;

    /** \brief The universal stamina bar that is used to represent the state of battle actors
    *** All battle actors have a portrait that moves along this meter to signify their
    *** turn in the rotation.  The meter and corresponding portraits must be drawn after the
    *** character sprites.
    **/
    vt_video::StillImage stamina_meter;

    //! \brief The image used to highlight stamina icons for selected actors
    vt_video::StillImage stamina_icon_selected;

    /** \brief Small button icons used to indicate when a player can select an action for their characters
    *** These buttons are used to indicate to the player what button to press to bring up a character's command
    *** menu. This vector is built from a 2-row, 5-column multi-image. The rows represent the buttons for when
    *** the character can be given a command (first row) versus when they may not (second row). The first element
    *** in each row is a "blank" button that is not used. The next four elements correspond to the characters on
    *** the screen, from top to bottom.
    **/
    std::vector<vt_video::StillImage> character_action_buttons;

    //! \brief The music filename played during the battle.
    //! We only keep a string because this music is handled by the audio manager
    //! for better cross game modes support.
    std::string battle_music_filename;

    //! \brief The music filename played after the player has won the battle.
    //! We only keep a string because this music is handled by the audio manager
    //! for better cross game modes support.
    std::string victory_music_filename;

    //! \brief The music filename played after the player has lost the battle. Don't delete it.
    //! We only keep a string because this music is handled by the audio manager
    //! for better cross game modes support.
    std::string defeat_music_filename;
    //@}

private:
    /** \brief Holds icon images that represent the different types of targets
    *** Target types include attack points, ally/enemy, and different parties.
    **/
    std::vector<vt_video::StillImage> _target_type_icons;

    //! \brief An icon displayed above the character's head when it is stunned.
    vt_video::StillImage _stunned_icon;

    //! \brief The auto battle icon.
    vt_video::StillImage _auto_battle_icon;

    //! \brief The auto-battle activated text.
    vt_video::TextImage* _auto_battle_activated;

    //! \brief The escape icon.
    vt_video::StillImage _escape_icon;
}; // class BattleMedia

} // namespace vt_global

#endif // __BATTLE_MEDIA_HEADER__
