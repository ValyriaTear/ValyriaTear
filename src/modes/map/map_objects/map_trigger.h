///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAP_TRIGGER_HEADER__
#define __MAP_TRIGGER_HEADER__

#include "modes/map/map_objects/map_physical_object.h"

namespace vt_map
{

namespace private_map
{

//! \brief Represents a flat object that can be visually triggered when "talking" or stepping on it.
//! The object will then trigger an event.
class TriggerObject : public PhysicalObject
{
    //! \brief Constants representing the three types of animations for the treasure
    enum {
        TRIGGER_OFF_ANIM  = 0,
        TRIGGER_ON_ANIM = 1
    };

public:
    /** \param trigger_name The name of the trigger. Used to store and load the trigger state.
    *** \param off_animation_file The animation file used to display the treasure when it is closed.
    *** \param on_animation_file The animation file used to display the treasure when it is open.
    *** \param off_event_id The event id to call when setting the trigger to off.
    *** \param on_event_id The event id to call when setting the trigger to on.
    **/
    TriggerObject(const std::string &trigger_name, MapObjectDrawLayer layer,
                  const std::string &off_animation_file, const std::string &on_animation_file,
                  const std::string& off_event_id, const std::string& on_event_id);

    ~TriggerObject()
    {}

    //! \brief A C++ wrapper made to create a new object from scripting,
    //! without letting Lua handling the object life-cycle.
    //! \note We don't permit luabind to use constructors here as it can't currently
    //! give the object ownership at construction time.
    static TriggerObject* Create(const std::string &trigger_name,
                                 MapObjectDrawLayer layer,
                                 const std::string &off_animation_file,
                                 const std::string &on_animation_file,
                                 const std::string& off_event_id,
                                 const std::string& on_event_id);

    //! \brief Changes the current animation if the character collides with the trigger.
    void Update();

    std::string GetTriggerName() const
    { return _trigger_name; }

    //! \brief Triggers the object from off to on, or the contrary, calling the on or off event.
    //! true == triggered/on.
    void SetState(bool state = true);

    bool GetState() const
    { return _trigger_state; }

    void ToggleState()
    { SetState(!_trigger_state); }

    //! \brief Set whether the trigger can be toggled by the character.
    void SetTriggerableByCharacter(bool triggerable)
    { _triggerable_by_character = triggerable; }

    //! \brief Set the new event name trigger when the trigger is pushed.
    //! if the event is empty, the trigger event is disabled.
    void SetOnEvent(const std::string& on_event)
    { _on_event = on_event; }

    //! \brief Set the new event name trigger when the trigger is set to not pushed.
    //! if the event is empty, the trigger event is disabled.
    void SetOffEvent(const std::string& off_event)
    { _off_event = off_event; }

private:
    //! \brief The treasure object name
    std::string _trigger_name;

    //! The trigger state (false == off)
    bool _trigger_state;

    //! \brief Tells whether the character can toggle the state by stepping on it.
    //! If not, only events can do that. (true by default)
    bool _triggerable_by_character;

    //! \brief Event triggered when the trigger is set to on.
    std::string _on_event;

    //! \brief Event triggered when the trigger is set to off.
    std::string _off_event;

    //! \brief Loads the state of the trigger from the global event corresponding to the current map
    //! It doesn't call the on/off events since this should be dealt with the trigger states at map load time.
    void _LoadState();
}; // class TreasureObject : public PhysicalObject

} // namespace private_map

} // namespace vt_map

#endif // __MAP_TRIGGER_HEADER__
