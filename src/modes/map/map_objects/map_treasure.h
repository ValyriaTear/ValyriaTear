///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_TREASURE_HEADER__
#define __MAP_TREASURE_HEADER__

#include "modes/map/map_objects/map_physical_object.h"

#include "modes/map/map_treasure_content.h"

namespace vt_map
{

namespace private_map
{

/** ****************************************************************************
*** \brief Represents an obtainable treasure on the map which the player may access
***
*** A treasure is a specific type of physical object, usually in the form of a
*** treasure chest. When the player accesses these treasures, the chest animates as
*** it is being opened and the treasure supervisor is initialized once the opening
*** animation is complete. Each treasure object on a map has a global event associated
*** with it to determine whether the treasure contents have already been retrieved by
*** the player.
***
*** Image files for treasures are single row multi images where the frame ordering
*** goes from closed, to opening, to open. This means each map treasure has exactly
*** three animations. The closed and open animations are usually single frame images.
***
*** To add contents to the treasure for this object, you will need to retreive the
*** pointer to the MapTreasure object via the GetTreasure() method, then add drunes
*** and/or objects (items/equipment/etc) to the MapTreasure.
***
*** \todo Add support for more treasure features, such as locked chests, chests which
*** trigger a battle, etc.
*** ***************************************************************************/
class TreasureObject : public PhysicalObject
{
    //! \brief Constants representing the three types of animations for the treasure
    enum {
        TREASURE_CLOSED_ANIM   = 0,
        TREASURE_OPENING_ANIM  = 1,
        TREASURE_OPEN_ANIM     = 2
    };

public:
    /** \param treasure_name The name of the treasure. Used to store and load the treasure state.
    *** \param treasure The treasure object holding its content.
    *** \param closed_animation_file The animation file used to display the treasure when it is closed.
    *** \param opening_animation_file The animation file used to display the treasure when it is opening.
    *** \param open_animation_file The animation file used to display the treasure when it is open.
    **/
    TreasureObject(const std::string &treasure_name,
                   MapObjectDrawLayer layer,
                   const std::string &closed_animation_file,
                   const std::string &opening_animation_file,
                   const std::string &open_animation_file);

    ~TreasureObject() {
        delete _treasure;
    }

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static TreasureObject* Create(const std::string &treasure_name,
                                  MapObjectDrawLayer layer,
                                  const std::string &closed_animation_file,
                                  const std::string &opening_animation_file,
                                  const std::string &open_animation_file);

    std::string GetTreasureName() const {
        return _treasure_name;
    }

    //! \brief Opens the treasure, which changes the active animation and initializes the treasure supervisor when the opening animation finishes.
    void Open();

    //! \brief Changes the current animation if it has finished looping
    void Update();

    //! \brief Retrieves a pointer to the MapTreasure object holding the treasure.
    MapTreasureContent* GetTreasure() {
        return _treasure;
    }

    //! \brief Sets the number of drunes present in the chest's contents.
    void SetDrunes(uint32_t amount) {
        _treasure->SetDrunes(amount);
    }

    /** \brief Adds an item to the contents of the TreasureObject
    *** \param id The id of the GlobalObject to add
    *** \param quantity The number of the object to add (default == 1)
    *** \return True if the object was added successfully
    **/
    bool AddItem(uint32_t id, uint32_t quantity = 1);

    /** \brief Adds an event triggered at start of the treasure event.
    *** \param event_id The id of the event to add
    **/
    void AddEvent(const std::string& event_id);

private:
    //! \brief Stores the contents of the treasure which will be processed by the treasure supervisor
    MapTreasureContent* _treasure;

    //! \brief The treasure object name
    std::string _treasure_name;

    //! \brief Events triggered at the start of the treasure event.
    std::vector<std::string> _events;

    //! \brief Tells whether the events have been started. So we can keep track of
    //! whether they've finished before opening the treasure supervisor.
    bool _events_triggered;

    //! \brief Tells whether the treasure is being opened.
    bool _is_opening;

    //! \brief Loads the state of the chest from the global event corresponding to the current map
    void _LoadState();
};

} // namespace private_map

} // namespace vt_map

#endif // __MAP_TREASURE_HEADER__
