///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2014 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_events.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for map mode events and event processing
***
*** Events occur on map mode to alter the state of the map, present a scene to the
*** player, or do any other custom task we require. Events may be "chained" together
*** so that one event begins as another ends. Many events are scripted, but this
*** file contains some C++ implementations of the most common types of events so
*** that these do not have to be continually re-implemented in every Lua map file.
*** ***************************************************************************/

#ifndef __MAP_EVENTS_HEADER__
#define __MAP_EVENTS_HEADER__

#include "modes/map/map_treasure.h"
#include "modes/shop/shop_utils.h"

#include "engine/audio/audio_descriptor.h"

#include "engine/script/script.h"

namespace vt_map
{

namespace private_map
{

class ContextZone;
class MapSprite;
class SpriteDialogue;
class VirtualSprite;

struct BattleEnemyInfo;

/** ****************************************************************************
*** \brief A container class representing a link between two map events
***
*** Map events may trigger additional events to occur alongside it or following
*** it. This class represents a "link" between two events and describes how the
*** two events are linked. In an event link there is a parent event and a child
*** event. The parent and child events may begin at the same time, or the child
*** event may occur after the parent event starts, but the child will never
*** preceed the parent's start. This class only stores the event_id of the child,
*** and the link object is added as a member onto the parent event's class. When
*** the parent event gets processed, all links are examined and the children events
*** are prepared appropriately.
***
*** We use two pieces of information to determine when to start a child event relevant
*** to its parent. The first is a boolean value that indicates whether the child's
*** start is relative to the parent's start or the parent's finish. The second is a
*** time value that indicates how long to wait (in milliseconds) from the parent's
*** start/finish before starting the child event.
*** ***************************************************************************/
class EventLink
{
public:
    EventLink(const std::string &child_id, bool start, uint32 time) :
        child_event_id(child_id), launch_at_start(start), launch_timer(time) {}

    ~EventLink()
    {}

    //! \brief The ID of the child event in this link
    std::string child_event_id;

    //! \brief The event will launch relative to the parent event's start if true, or its finish if false
    bool launch_at_start;

    //! \brief The amount of milliseconds to wait before launching the event (0 means launch instantly)
    uint32 launch_timer;
}; // class EventLink


/** ****************************************************************************
*** \brief An abstract class representing an event that occurs on a map
***
*** An event can be virtually anything from playing a sound to moving a sprite
*** to beginning a dialogue. Events do not necessarily inform the user (through
*** visual or audio means) that an event has occurred. They may be employed to
*** change the state of a map without the player's knowledge. This is an abstract
*** class because common types of events (such as beginning a dialogue) are implemented
*** in C++ code while Lua is used to represent not-so-common types of events.
***
*** All events have a unique, non empty, std::string value that serve to
*** distinguish the events from one another (an ID string). Events can also contain any
*** number of "links" to children events, which are events which launch simultaneously
*** with or some time after the parent event. Events are processed via two
*** functions. _Start() is called only one when the event begins. _Update() is called
*** once for every iteration of the main game loop until this function returns a true
*** value, indicating that the event is finished.
*** ***************************************************************************/
class MapEvent
{
    friend class EventSupervisor;
public:
    //! \param id The ID for the map event (an empty() value is invalid)
    MapEvent(const std::string &id, EVENT_TYPE type) :
        _event_id(id), _event_type(type) {}

    virtual ~MapEvent()
    {}

    const std::string &GetEventID() const {
        return _event_id;
    }

    EVENT_TYPE GetEventType() const {
        return _event_type;
    }

    /** \brief Declares a child event to be launched immediately at the start of this event
    *** \param child_event_id The event id of the child event
    **/
    void AddEventLinkAtStart(const std::string &child_event_id) {
        _AddEventLink(child_event_id, true, 0);
    }

    /** \brief Declares a child event to be launched after the start of this event
    *** \param child_event_id The event id of the child event
    *** \param launch_time The number of milliseconds to wait before launching the child event
    **/
    void AddEventLinkAtStart(const std::string &child_event_id, uint32 launch_time) {
        _AddEventLink(child_event_id, true, launch_time);
    }

    /** \brief Declares a child event to be launched immediately at the end of this event
    *** \param child_event_id The event id of the child event
    **/
    void AddEventLinkAtEnd(const std::string &child_event_id) {
        _AddEventLink(child_event_id, false, 0);
    }

    /** \brief Declares a child event to be launched after the end of this event
    *** \param child_event_id The event id of the child event
    *** \param launch_time The number of milliseconds to wait before launching the child event
    **/
    void AddEventLinkAtEnd(const std::string &child_event_id, uint32 launch_time) {
        _AddEventLink(child_event_id, false, launch_time);
    }

protected:
    /** \brief Starts the event
    *** This function is only called once per event execution
    **/
    virtual void _Start() = 0;

    /** \brief Updates the event progress and checks if the event has finished
    *** \return True if the event is finished
    *** This function is called as many times as needed until the event has finished. The contents
    *** of this function may do more than simply check if the event is finished. It may also execute
    *** code for the event with the goal of eventually brining the event to a finished state.
    **/
    virtual bool _Update() = 0;

    /** \brief Declares a child event to be linked to this event
    *** \param child_event_id The event id of the child event
    *** \param launch_at_start The child starts relative to the start of the event if true, its finish if false
    *** \param launch_time The number of milliseconds to wait before launching the child event
    **/
    void _AddEventLink(const std::string &child_event_id, bool launch_at_start, uint32 launch_time) {
        _event_links.push_back(EventLink(child_event_id, launch_at_start, launch_time));
    }

private:
    //! \brief A unique ID string for the event. An empty value is invalid
    std::string _event_id;

    //! \brief Identifier for the class type of this event
    EVENT_TYPE _event_type;

    //! \brief All child events of this class, represented by EventLink objects
    std::vector<EventLink> _event_links;
}; // class MapEvent


/** ****************************************************************************
*** \brief An event which activates a dialogue on the map
***
*** Note that a dialogue may execute script actions, which would somewhat act
*** like events but technically are not events. Children events that are implemented
*** in Lua can take advantage of options selected by the player in these dialogues
*** to determine what events should follow down the event chain
***
*** Sometimes you may want a dialogue event to stop the camera from moving, especially
*** if it is the first event in an event chain. When this behavior is desired, call the
*** StopCameraMovement() method after creating the event object.
*** ***************************************************************************/
class DialogueEvent : public MapEvent
{
public:
    /** \param event_id The ID of this event
    *** \param dialogue_id The ID of the dialogue to execute through this event
    *** \param dialogue The SpriteDialogue* to execute through this event
    **/
    DialogueEvent(const std::string &event_id, uint32 dialogue_id);
    DialogueEvent(const std::string &event_id, SpriteDialogue *dialogue);

    ~DialogueEvent()
    {}

    //! \brief Toggles whether or not camera movement should be stopped when the dialogue begins
    void SetStopCameraMovement(bool stop) {
        _stop_camera_movement = stop;
    }

protected:
    //! \brief The ID of the dialogue to invoke
    uint32 _dialogue_id;

    //! \brief When true, any camera movement will be stopped when the event begins
    bool _stop_camera_movement;

    //! \brief Begins the dialogue
    void _Start();

    //! \brief Returns true when the last line of the dialogue has been read
    bool _Update();
}; // class DialogueEvent : public MapEvent


/** ****************************************************************************
*** \brief An event that creates an instance of ShopMode when started
*** ***************************************************************************/
class ShopEvent : public MapEvent
{
public:
    //! \param event_id The ID of this event
    ShopEvent(const std::string &event_id):
        MapEvent(event_id, SHOP_EVENT),
        _buy_level(vt_shop::SHOP_PRICE_STANDARD),
        _sell_level(vt_shop::SHOP_PRICE_STANDARD),
        _enable_sell_mode(true)
    {}

    ~ShopEvent()
    {}

    //! \brief Set the Shop name
    void SetShopName(const vt_utils::ustring& shop_name) {
        _shop_name = shop_name;
    }

    //! \brief Set the Shop greetings text
    void SetGreetingText(const vt_utils::ustring& greeting_text) {
        _greeting_text = greeting_text;
    }

    void SetSellModeEnabled(bool sell_mode_enabled) {
        _enable_sell_mode = sell_mode_enabled;
    }

    /** \brief Adds an object to the list of objects for sale.
    *** \param object_id The ID of the GlobalObject to make available for purchase.
    *** \param stock The amount of the object to make available for purchase.
    *** If set to 0, the number of objects is infinite.
    *** \note All wares must be added before the _Start() method is called to ensure
    *** that the wares actually appear in shop mode.
    **/
    void AddObject(uint32 object_id, uint32 stock) {
        _objects.insert(std::make_pair(object_id, stock));
    }

    /** \brief Adds an object to the list of objects for sale.
    *** \param object_id The ID of the GlobalObject to make available for purchase.
    *** \param stock The amount of the object to make available for purchase.
    *** If set to 0, the number of objects is infinite.
    *** \note All wares must be added before the _Start() method is called to ensure
    *** that the wares actually appear in shop mode.
    **/
    void AddTrade(uint32 object_id, uint32 stock) {
        _trades.insert(std::make_pair(object_id, stock));
    }

    //! \brief Set the shop quality levels which will handle pricing variations.
    void SetPriceLevels(vt_shop::SHOP_PRICE_LEVEL buy_level,
                        vt_shop::SHOP_PRICE_LEVEL sell_level) {
        _buy_level = buy_level;
        _sell_level = sell_level;
    }

protected:
    //! \brief The GlobalObject IDs and stock count of all objects to be sold in the shop
    std::set<std::pair<uint32, uint32> > _objects;

    //! \brief The GlobalObject IDs and stock count of all objects to be sold in the shop
    std::set<std::pair<uint32, uint32> > _trades;

    //! \brief The Shop quality levels. The more the level is, the worse it is for the player.
    vt_shop::SHOP_PRICE_LEVEL _buy_level;
    vt_shop::SHOP_PRICE_LEVEL _sell_level;

    //! \brief Optional custom shop name and greeting text.
    vt_utils::ustring _shop_name;
    vt_utils::ustring _greeting_text;

    //! \brief Tells whether the sell mode can be enabled
    bool _enable_sell_mode;

    //! \brief Creates an instance of ShopMode and pushes it to the game mode stack
    void _Start();

    //! \brief Performs no operation (returns true)
    bool _Update() {
        return true;
    }
}; // class ShopEvent : public MapEvent


/** ****************************************************************************
*** \brief Plays a sound. The event finishes when the sound stops
***
*** The suggested usage for initializing an object of this class is the following:
*** -# Call the class constructor
*** -# Call the GetSound() function to retrieve the SoundDescriptor object
*** -# Call SoundDescriptor methods to set the desired properties of the sound (looping, attenuation, etc)
***
*** After these steps are performed the event is ready to launch. The default properties
*** of the sound are the same as are in the default constructor of the SoundDescriptor
*** class. This includes no looping and no distance attenuation. The event will finish when the
*** sound finishes playing (when the sound state is AUDIO_STATE_STOPPED). Note that if looping is set
*** to infinite, the sound will never enter this state. It is possible to prematurely terminate this
*** event by calling the GetSound() method and invoking Stop() on the SoundDescriptor object that is
*** returned.
***
*** \note The MapMode class has a container of SoundDescriptor objects which should include all of
*** the sounds that may be used on a given map. This means that when a SoundEvent is created, the
*** sound file data will already be loaded by the audio engine.
***
*** \todo Support sounds with a position that employ distance attenuation. Perhaps
*** another derived class would be ideal to implement this, since sounds could possibly
*** be mobile (attached to sprites).
*** ***************************************************************************/
class SoundEvent : public MapEvent
{
public:
    /** \param event_id The ID of this event
    *** \param sound_filename The name of the sound file to load
    **/
    SoundEvent(const std::string &event_id, const std::string &sound_filename);

    ~SoundEvent()
    { _sound.Stop(); }

    //! \brief Accessor which allows the properties of the sound to be customized
    vt_audio::SoundDescriptor &GetSound()
    { return _sound; }

protected:
    //! \brief Begins playback of the sound
    void _Start()
    { _sound.Play(); }

    //! \brief Returns true when the sound has finished playing, or finished looping
    bool _Update();

    //! \brief The sound that this event will play
    vt_audio::SoundDescriptor _sound;
}; // class SoundEvent : public MapEvent


/** ****************************************************************************
*** \brief Event for switching from one map to another, with fading.
***
***
*** ***************************************************************************/
class MapTransitionEvent : public MapEvent
{
public:
    /** \param event_id The ID of this event
    *** \param data_filename The name of the map data file to transition to
    *** \param data_filename The name of the map script file to use
    *** \param coming_from The transition origin.
    **/
    MapTransitionEvent(const std::string &event_id,
                       const std::string &data_filename,
                       const std::string &script_filename,
                       const std::string &coming_from);

    ~MapTransitionEvent() {};

protected:
    //! \brief Begins the transition process by fading out the screen and music
    void _Start();

    //! \brief Once the fading process completes, creates the new map mode to transition to
    bool _Update();

    //! \brief The data and script filenames of the map to transition to
    std::string _transition_map_data_filename;
    std::string _transition_map_script_filename;

    /** \brief a string telling where the map transition is coming from.
    *** useful when changing from a map to another to set up the camera position.
    **/
    std::string _transition_origin;

    //! \brief tells the update function to trigger the new map.
    bool _done;
}; // class MapTransitionEvent : public MapEvent


/** ****************************************************************************
*** \brief
***
***
*** ***************************************************************************/
class JoinPartyEvent : public MapEvent
{
public:
    /** \param event_id The ID of this event
    **/
    JoinPartyEvent(const std::string &event_id);

    ~JoinPartyEvent();

protected:
    //! \brief
    void _Start();

    //! \brief
    bool _Update();
}; // class JoinPartyEvent : public MapEvent


/** ****************************************************************************
*** \brief Instantly starts a battle.
***
***
*** ***************************************************************************/
class BattleEncounterEvent : public MapEvent
{
public:
    /** \param event_id The ID of this event
    **/
    BattleEncounterEvent(const std::string &event_id);

    ~BattleEncounterEvent()
    {}

    void SetMusic(const std::string &filename) {
        _battle_music = filename;
    }

    void SetBackground(const std::string &filename) {
        _battle_background = filename;
    }

    void SetBoss(bool is_boss) {
        _is_boss = is_boss;
    }

    void AddEnemy(uint32 enemy_id, float position_x, float position_y);

    void AddEnemy(uint32 enemy_id) {
        AddEnemy(enemy_id, 0, 0);
    }

    void AddScript(const std::string &filename) {
        _battle_scripts.push_back(filename);
    }

protected:
    //! \brief ID numbers for enemies to generate
    std::vector<BattleEnemyInfo> _enemies;

    //! \brief Filename for battle music
    std::string _battle_music;

    //! \brief Filename for battle background
    std::string _battle_background;

    //! \brief Filenames of the battle scripts
    std::vector<std::string> _battle_scripts;

    //! \brief Tells whether the battle is against a boss
    bool _is_boss;

    //! \brief Starts the battle
    void _Start();

    /** \brief Currently does nothing, since the battle transition management
    *** is done through games modes.
    **/
    bool _Update() {
        return true;
    }
}; // class BattleEncounterEvent : public MapEvent

/** ****************************************************************************
*** \brief An event thats starts an event or another based on a check function.
*** ***************************************************************************/
class IfEvent : public MapEvent
{
public:
    /** \param event_id The ID of this event
    *** \param check_function the map file's function to call to make the check requested
    *** \param on_true_event the map to call when the check function returns true
    *** \param on_false_event the map to call when the check function returns false
    ***
    *** \note An empty value for either the true or false event id arguments
    *** will result in no event in this case
    **/
    IfEvent(const std::string& event_id, const std::string& check_function,
            const std::string& on_true_event, const std::string& on_false_event);

    ~IfEvent()
    {}

protected:
    //! \brief A pointer to the Lua function that starts the event
    ScriptObject _check_function;

    std::string _true_event_id;
    std::string _false_event_id;

    //! \brief Calls the Lua _start_function, if one was defined
    void _Start();

    //! \brief Calls the Lua _update_function. If no update function was defined, does nothing and returns true
    bool _Update()
    { return true; }
}; // class IfEvent : public MapEvent

/** ****************************************************************************
*** \brief An event with its _Start and _Update functions implemented in Lua.
***
*** All events that do not fall into the other categories of events will be
*** implemented here. This event uses Lua functions to implement the _Start()
*** and _Update() functions (all these C++ functions do is call the
*** corresponding Lua functions). Note that any type of event can be implemented
*** in Lua, including alternative implementations of the other C++ event types.
*** You should only use this event type if there is no way to implement your
*** event in the other event types provided.
*** ***************************************************************************/
class ScriptedEvent : public MapEvent
{
public:
    /** \param event_id The ID of this event
    *** \param start_function the map file's function start function name to call
    *** \param update_function the map file's function update function name to call
    ***
    *** \note An empty value for either the start or update arguments will result in no start or
    *** update function being defined. If no update function is defined, the call to _Update() will always
    *** return true, meaning that this event will end immediately after it starts.
    **/
    ScriptedEvent(const std::string &event_id, const std::string &start_function,
                  const std::string &update_function);

    ~ScriptedEvent()
    {}

protected:
    //! \brief A pointer to the Lua function that starts the event
    ScriptObject _start_function;

    //! \brief A pointer to the Lua function that returns a boolean value if the event is finished
    ScriptObject _update_function;

    //! \brief Calls the Lua _start_function, if one was defined
    void _Start();

    //! \brief Calls the Lua _update_function. If no update function was defined, does nothing and returns true
    bool _Update();
}; // class ScriptedEvent : public MapEvent


/** ****************************************************************************
*** \brief An abstract event class that represents an event controlling a sprite
***
*** Sprite events are special types of events that control a sprite (of any type)
*** on a map. Technically they are more like controllers than events, in that they
*** take control of a sprite and direct how its state should change, whether that
*** be their direction, movement, and/or display. All sprite events are connected
*** to one (and only one) sprite. When the event takes control over the sprite,
*** it notifies the sprite object which grabs a pointer to the SpriteEvent.
***
*** For a deriving class to be implemented properly, it must do two things.
*** # In the _Start method, call SpriteEvent::_Start() before any other code
*** # Before returning true in the _Update() method, call _sprite->ReleaseControl(this)
***
*** \note It is important to keep in mind that all map sprites have their update
*** function called before map events are updated. This can have implications for
*** changing some members of the sprite object inside the _Start() and _Update() methods
*** as these methods are called <i>after</i> the sprite's own Update() method. Keep
*** this property in mind when designing a derived sprite event class.
*** ***************************************************************************/
class SpriteEvent : public MapEvent
{
public:
    /** \param event_id The ID of this event
    *** \param event_type The type of this event
    *** \param sprite_id The id of the sprite that this event will control
    **/
    SpriteEvent(const std::string &event_id, EVENT_TYPE event_type, uint16 sprite_id);

    /** \param event_id The ID of this event
    *** \param event_type The type of this event
    *** \param sprite A pointer to the sprite that this event will control
    **/
    SpriteEvent(const std::string &event_id, EVENT_TYPE event_type, VirtualSprite *sprite);

    ~SpriteEvent()
    {}

    VirtualSprite *GetSprite() const {
        return _sprite;
    }

    //! \brief Frees the sprite from the control_event
    virtual void Terminate();

protected:
    //! \brief A pointer to the map sprite that the event controls
    VirtualSprite *_sprite;

    /** \brief Starts a sprite event.
    ***
    *** This method will make sure no other sprite event is operating the current sprite
    *** and will potentially end the previous event before
    *** Acquiring control of the sprite.
    **/
    virtual void _Start();

    //! \brief Updates the state of the sprite and returns true if the event is finished
    virtual bool _Update() = 0;
}; // class SpriteEvent : public MapEvent


/** ****************************************************************************
*** \brief A scripted event which operates on a sprite
***
*** This class is a cross between a SpriteEvent and ScriptedEvent class. The class
*** itself inherits from SpriteEvent (it does not also inherit from ScriptedEvent).
*** The key feature of this class is that it passes a pointer to a VirtualSprite
*** object in the argument list when it makes its Lua function calls. The Lua functions
*** are then able to take any allowable action on the sprite object. Otherwise, this
*** class behaves just like a standard ScriptedEvent class.
*** ***************************************************************************/
class ScriptedSpriteEvent : public SpriteEvent
{
public:
    /** \param event_id The ID of this event
    *** \param sprite_id The id of the sprite that will be passed to the Lua script functions
    *** \param start_function the map file's function start function name to call
    *** \param update_function the map file's function update function name to call
    ***
    *** \note A value of zero for either the start or update index arguments will result in no start or
    *** update function being defined. If no update function is defined, the call to _Update() will always
    *** return true, meaning that this event will end immediately after it starts.
    **/
    ScriptedSpriteEvent(const std::string &event_id, uint16 sprite_id,
                        const std::string &start_function, const std::string &update_function);

    /** \param event_id The ID of this event
    *** \param sprite A pointer to the sprite that will be passed to the Lua script functions
    *** \param start_function the map file's function start function name to call
    *** \param update_function the map file's function update function name to call
    ***
    *** \note A value of zero for either the start or update index arguments will result in no start or
    *** update function being defined. If no update function is defined, the call to _Update() will always
    *** return true, meaning that this event will end immediately after it starts.
    **/
    ScriptedSpriteEvent(const std::string &event_id, VirtualSprite *sprite,
                        const std::string &start_function, const std::string &update_function);

    ~ScriptedSpriteEvent()
    {}

protected:
    //! \brief A pointer to the Lua function that starts the event
    ScriptObject _start_function;

    //! \brief A pointer to the Lua function that returns a boolean value if the event is finished
    ScriptObject _update_function;

    //! \brief Calls the Lua _start_function, if one was defined
    void _Start();

    //! \brief Calls the Lua _update_function. If no update function was defined, does nothing and returns true
    bool _Update();
}; // class ScriptedSpriteEvent : public SpriteEvent


/** ****************************************************************************
*** \brief A simple event used to set the direction of a sprite
***
*** This event finishes immediately after it starts, as all that it performs is
*** to set the direction of a sprite in a particular orientation. Normally such
*** a minor event would be better suited as a ScriptedEvent with no update function,
*** but because a set direction operation is so common, it makes sense to create a
*** specific event for it for convenience.
***
*** \note The only directions you should set in the class constructor are: NORTH,
*** SOUTH, EAST, and WEST. This event is used when a sprite is stationary, so
*** the other types of directions (which also infer movement) are unnecessary.
*** Using a direction other than these four will result in a warning being printed.
*** ***************************************************************************/
class ChangeDirectionSpriteEvent : public SpriteEvent
{
public:
    /** \param event_id The ID of this event
    *** \param sprite _id The ID of the sprite to change the direction of
    *** \param direction The direction to face the sprite
    **/
    ChangeDirectionSpriteEvent(const std::string &event_id, uint16 sprite_id, uint16 direction);

    /** \param event_id The ID of this event
    *** \param sprite A pointer to the sprite that this event will effect
    *** \param direction The direction to face the sprite
    **/
    ChangeDirectionSpriteEvent(const std::string &event_id, VirtualSprite *sprite, uint16 direction);

    ~ChangeDirectionSpriteEvent()
    {}

protected:
    //! \brief Retains the direction to move the sprite when the event starts
    uint16 _direction;

    //! \brief Immediately changes the sprite's direction
    void _Start();

    //! \brief Always returns true immediately, terminating the event
    bool _Update();
}; // class ChangeDirectionSpriteEvent : public SpriteEvent

/** ****************************************************************************
*** \brief A simple event used to set the direction of a sprite to make it look
*** at something or another sprite.
***
*** This event finishes immediately after it starts, as all that it performs is
*** to set the direction of a sprite in a particular orientation.
***
*** \note The only directions you should set in the class constructor are: NORTH,
*** SOUTH, EAST, and WEST. This event is used when a sprite is stationary, so
*** the other types of directions (which also infer movement) are unnecessary.
*** Using a direction other than these four will result in a warning being printed.
*** ***************************************************************************/
class LookAtSpriteEvent : public SpriteEvent
{
public:
    /** \param event_id The ID of this event
    *** \param sprite_id The ID of the sprite to change the direction of
    *** \param second_sprite_id The ID of the sprite to make the first look at
    **/
    LookAtSpriteEvent(const std::string &event_id, uint16 sprite_id, uint16 second_sprite_id);

    /** \param event_id The ID of this event
    *** \param sprite A pointer to the sprite that this event will effect
    *** \param sprite A pointer to the sprite to look at.
    **/
    LookAtSpriteEvent(const std::string &event_id, VirtualSprite *sprite, VirtualSprite *other_sprite);

    /** \param event_id The ID of this event
    *** \param sprite A pointer to the sprite that this event will effect
    *** \param x, y map coodinates to look at.
    **/
    LookAtSpriteEvent(const std::string &event_id, VirtualSprite *sprite, float x, float y);

    ~LookAtSpriteEvent()
    {}

protected:
    //! \brief Retains the position to look at when the event starts.
    float _x, _y;

    /** \brief Retains the position to look at when the even starts.
    *** \note The event will take the sprite coord only at _Start() call, since
    *** the position may have changed between the event declaration (map load time)
    *** and its start.
    **/
    VirtualSprite *_target_sprite;

    //! \brief Immediately changes the sprite's direction
    void _Start();

    //! \brief Always returns true immediately, terminating the event
    bool _Update();
}; // class LookAtSpriteEvent : public SpriteEvent

/** ****************************************************************************
*** \brief An event which moves a single sprite to a destination
***
*** This class allows for both absolute and relative destinations. A relative
*** destination
***
*** Using event linking, it is very simple to have a single event represent
*** a sprite traveling to multiple destinations, or multiple sprites travel to
*** multiple destinations.
*** ***************************************************************************/
class PathMoveSpriteEvent : public SpriteEvent
{
    friend class VirtualSprite;

public:
    /** \param event_id The ID of this event
    *** \param sprite_id The ID of the sprite that is to be moved
    *** \param x_coord The X coordinate to move the sprite to
    *** \param y_coord The Y coordinate to move the sprite to
    *** \param run whether the character has to go there by walking or running
    **/
    PathMoveSpriteEvent(const std::string &event_id, uint16 sprite_id, float x_coord, float y_coord, bool run);

    /** \param event_id The ID of this event
    *** \param sprite A pointer to the sprite to move
    *** \param x_coord The X coordinate to move the sprite to
    *** \param y_coord The Y coordinate to move the sprite to
    *** \param run whether the character has to go there by walking or running
    **/
    PathMoveSpriteEvent(const std::string &event_id, VirtualSprite *sprite, float x_coord, float y_coord, bool run);

    /** \param event_id The ID of this event
    *** \param sprite A pointer to the sprite to move
    *** \param target_sprite The target sprite to move the sprite to
    *** \param run whether the character has to go there by walking or running
    **/
    PathMoveSpriteEvent(const std::string &event_id, VirtualSprite *sprite, VirtualSprite *target_sprite, bool run);

    ~PathMoveSpriteEvent()
    {}

    /** \brief Used to change the destination coordinates after the class object has been constructed
    *** \param x_coord The X coordinate to move the sprite to
    *** \param y_coord The Y coordinate to move the sprite to
    *** or:
    *** \param target_sprite The target sprite to move the sprite to
    *** \param run whether the character has to go there by walking or running
    *** \note Any previous existing paths are cleared when this function is called. If this function is called when
    *** the event is active, no change will take place.
    *** This function is especially useful when trying to path move according to another NPC position, wichi may have
    *** changed between the event declaration (at map load time) and the event actual start.
    **/
    void SetDestination(float x_coord, float y_coord, bool run);
    void SetDestination(VirtualSprite *target_sprite, bool run);

    Path GetPath() const {
        return _path;
    }

    //! \brief Stops and frees the sprite from the control_event
    void Terminate();

protected:
    //! \brief Stores the destination coordinates for the path movement. These may be either absolute or relative coordinates.
    float _destination_x, _destination_y;

    //! \brief The destination target, useful when willing to reach a moving point.
    VirtualSprite *_target_sprite;

    //! \brief Used to store the previous coordinates of the sprite during path movement, so as to set the proper direction of the sprite as it moves
    float _last_x_position, _last_y_position;

    //! \brief Used to store the current node collision position (with offset)
    float _current_node_x, _current_node_y;

    //! \brief An index to the path vector containing the node that the sprite currently occupies
    uint32 _current_node;

    //! \brief Holds the path needed to traverse from source to destination
    Path _path;

    //! \brief Tells whether the sprite should use the walk or run animation
    bool _run;

    //! \brief Calculates a path for the sprite to move to the destination
    void _Start();

    //! \brief Returns true when the sprite has reached the destination
    bool _Update();

    //! \brief Sets the correct direction for the sprite to move to the next node in the path
    void _SetSpriteDirection();
}; // class PathMoveSpriteEvent : public SpriteEvent


/** ****************************************************************************
*** \brief An event which randomizes movement of a sprite
*** ***************************************************************************/
class RandomMoveSpriteEvent : public SpriteEvent
{
    friend class VirtualSprite;

public:
    /** \param event_id The ID of this event
    *** \param sprite A pointer to the sprite to move
    *** \param move_time The total amount of time that this event should take
    *** \param direction_time The amount of time to wait before changing the sprite's direction randomly
    **/
    RandomMoveSpriteEvent(const std::string &event_id, VirtualSprite *sprite,
                          uint32 move_time = 10000, uint32 direction_time = 2000);

    ~RandomMoveSpriteEvent();

    //! \brief Stops and frees the sprite from the control_event
    void Terminate();

protected:
    /** \brief The amount of time (in milliseconds) to perform random movement before ending this action
    *** Set this member to vt_system::INFINITE_TIME in order to continue the random movement
    *** forever. The default value of this member will be set to 10 seconds if it is not specified.
    **/
    uint32 _total_movement_time;

    /** \brief The amount of time (in milliseconds) that the sprite should continue moving in its current direction
    *** The default value for this timer is 1.5 seconds (1500ms).
    **/
    uint32 _total_direction_time;

    //! \brief A timer which keeps track of how long the sprite has been in random movement
    uint32 _movement_timer;

    //! \brief A timer which keeps track of how long the sprite has been moving around since the last change in direction.
    uint32 _direction_timer;

    //! \brief Calculates a path for the sprite to move to the destination
    void _Start();

    //! \brief Returns true when the sprite has reached the destination
    bool _Update();
}; // class RandomMoveSpriteEvent : public SpriteEvent


/** ****************************************************************************
*** \brief Displays specific sprite frames for a certain period of time
***
*** This event displays a certain animation of a sprite for a specified amount of time.
*** Its primary purpose is to allow complete control over how a sprite appears to the
*** player and to show the sprite interacting with its surroundings, such as flipping
*** through a book taken from a bookshelf. Looping of these animations is also supported.
***
*** \note You <b>must</b> add at least one frame to this object
***
*** \note These actions can not be used with VirtualSprite objects, since this
*** class explicitly needs animation images to work and virtual sprites have no
*** images.
*** ***************************************************************************/
class AnimateSpriteEvent : public SpriteEvent
{
public:
    /** \param event_id The ID of this event
    *** \param sprite A pointer to the sprite to move
    *** \param animation_name The name of the custom animation to play
    *** \param animation_time The custom animation time, 0 if infinite, and -1 is default time used.
    **/
    AnimateSpriteEvent(const std::string &event_id, VirtualSprite *sprite,
                       const std::string &animation_name, int32 animation_time);

    ~AnimateSpriteEvent()
    {}

    //! \brief Stops the custom animation and frees the sprite from the control_event
    void Terminate();

protected:
    //! The custom animation name
    std::string _animation_name;

    //! The custom animation time.
    int32 _animation_time;

    //! A reference to the map sprite object
    MapSprite *_map_sprite;

    //! \brief Triggers the custom animation for the given time
    void _Start();

    //! \brief Returns true when the sprite has finished to display a custom animation.
    bool _Update();
}; // class AnimateSpriteEvent : public SpriteEvent

/** ****************************************************************************
*** \brief An event permitting to trigger a treasure opening dialog with given content.
***
*** This event does basically the same as opening a treasure chest. @See TreasureObject.
*** ***************************************************************************/
class TreasureEvent : public MapEvent
{
public:
    /** \param event_id The ID of this event
    **/
    TreasureEvent(const std::string &event_id);

    ~TreasureEvent() {
        delete _treasure;
    }

    MapTreasure *GetTreasure() const {
        return _treasure;
    }

    //! \brief Sets the number of drunes present in the chest's contents.
    void SetDrunes(uint32 amount) {
        _treasure->SetDrunes(amount);
    }

    /** \brief Adds an object to the contents of the TreasureEvent
    *** \param id The id of the GlobalObject to add
    *** \param quantity The number of the object to add (default == 1)
    *** \return True if the object was added successfully
    **/
    bool AddObject(uint32 id, uint32 quantity = 1);

    /** \brief Adds an event triggered at start of the treasure event.
    *** \param event_id The id of the event to add
    **/
    void AddEvent(const std::string& event_id);

protected:
    //! \brief A pointer to the treasure content, used by the TreasureSupervisor.
    MapTreasure *_treasure;

    //! \brief Events triggered at the start of the treasure event.
    std::vector<std::string> _events;

    /** \brief Starts a sprite event.
    ***
    *** This method will open the treasure dialog with the treasure content set for this event.
    **/
    void _Start();

    //! \brief Returns true once the treasure dialog is closed, and false otherwise.
    bool _Update();
}; // class TreasureEvent : public MapEvent

/** ****************************************************************************
*** \brief Manages, processes, and launches map events
***
*** The EventSupervisor serves as an assistant to the MapMode class, much like the
*** other map supervisor classes. As such, this class is only created as a member
*** of the MapMode class. The first responsibility of the EventSupervisor is to
*** retain all of the MapEvent objects that have been created. The second responsibility
*** of this class is to initialize and begin the first event in a n-length chain
*** of events, where n can be equal to one or any higher interger value.
***
*** When an event chain begins, the first (base) event of the chain is started.
*** Immediately after starting the first event, the supervisor will examine its event
*** links to determine which, if any, children events begin relative to the start of
*** the base event. If they are to start a certain time after the start of the parent
*** event, they are placed in a container and their countdown timers are initialized.
*** These timers will count down on every update call to the event manager and after
*** the timers expire, these events will be launched. When an active event ends, again
*** its event links are examined to determine if any children events exist that start
*** relative to the end of the parent event.
***
*** \todo What about the case when the same event is begun when the event is already
*** active? Should we prevent the case where an event is activated twice, print a
*** warning, or allow this situation and hope the programmer knows what they are doing?
*** ***************************************************************************/
class EventSupervisor
{
public:
    EventSupervisor():
        _is_updating(false)
    {}

    ~EventSupervisor();

    /** \brief Registers a map event object with the event supervisor
    *** \param new_event A pointer to the new event
    *** \note This function should be called for all events that are created
    **/
    void RegisterEvent(MapEvent *new_event);

    /** \brief Marks a specified event as active and starts the event
    *** \param event_id The ID of the event to activate
    *** \param launch_time The time to wait before launching the event.
    *** The specified event to start may be linked to several children, grandchildren, etc. events.
    *** If the event has no children, it will activate only the single event requested. Otherwise
    *** all events in the chain will become activated at the appropriate time.
    **/
    void StartEvent(const std::string &event_id);
    void StartEvent(const std::string &event_id, uint32 launch_time);

    /** \brief Marks a specified event as active and starts the event
    *** \param event A pointer to the event to begin
    *** \param launch_time The time to wait before launching the event.
    *** The specified event to start may be linked to several children, grandchildren, etc. events.
    *** If the event has no children, it will activate only the single event requested. Otherwise
    *** all events in the chain will become activated at the appropriate time.
    **/
    void StartEvent(MapEvent *event);
    void StartEvent(MapEvent *event, uint32 launch_time);

    /** \brief Pauses the active events by preventing them from updating
    *** \param event_id The ID of the active event(s) to pause
    *** If the event corresponding to the ID is not active, a warning will be issued and no change
    *** will occur.
    **/
    void PauseEvents(const std::string &event_id);

    /** \brief Pauses the given sprite events
    *** \param sprite The sprite to pause the sprite events from
    **/
    void PauseAllEvents(VirtualSprite *sprite);

    /** \brief Resumes a paused event
    *** \param event_id The ID of the active event(s) to resume
    *** If the event corresponding to the ID is not paused, a warning will be issued and no change
    *** will occur.
    **/
    void ResumeEvents(const std::string &event_id);

    /** \brief Resumes the given sprite events
    *** \param sprite The sprite to resume the sprite events from
    **/
    void ResumeAllEvents(VirtualSprite *sprite);

    /** \brief Terminates an event if it is active
    *** \param event_id The ID of the event(s) to terminate
    *** \param event Mapevent(s) to terminate
    *** \param trigger_event_links Tells whether the launching of any of the events' children should occur, true by default.
    *** \note If there is no active event that corresponds to the event ID, the function will do nothing.
    **/
    void TerminateEvents(const std::string &event_id, bool trigger_event_links = true);
    void TerminateEvents(MapEvent *event, bool trigger_event_links = true);

    /** \brief Terminates all the SpriteEvents (active, paused, or incoming) for the given sprite.
    ***
    *** This is very useful when wanting to break an active event chain controlling a sprite
    *** and liberate it for something else.
    *** Note that you should start the new sprite event chain *after* this call.
    **/
    void TerminateAllEvents(VirtualSprite *sprite);


    //! \brief Updates the state of all active and launch events
    void Update();

    /** \brief Determines if a chosen event is active
    *** \param event_id The ID of the event to check
    *** \return True if the event is active, false if it is not or the event could not be found
    **/
    bool IsEventActive(const std::string &event_id) const;

    //! \brief Returns true if any events are active
    bool HasActiveEvent() const {
        return !_active_events.empty();
    }

    //! \brief Returns true if any events are being prepared to be launched after their timers expire
    bool HasActiveDelayedEvent() const {
        return !_active_delayed_events.empty();
    }

    /** \brief Returns a pointer to a specified event stored by this class
    *** \param event_id The ID of the event to retrieve
    *** \return A MapEvent pointer (which may need to be casted to the proper event type), or NULL if no event was found
    **/
    MapEvent *GetEvent(const std::string &event_id) const;

    bool DoesEventExist(const std::string &event_id) const
    { return !(GetEvent(event_id) == NULL); }

private:
    //! \brief A container for all map events, where the event's ID serves as the key to the std::map
    std::map<std::string, MapEvent *> _all_events;

    //! \brief A list of all events which have started but are not yet finished
    std::vector<MapEvent *> _active_events;

    //! \brief A list of all events which have been paused
    std::vector<MapEvent *> _paused_events;

    /** \brief A list of all events that are waiting on their launch timers to expire before being started
    *** The interger part of this std::pair is the countdown timer for this event to be launched
    **/
    std::vector<std::pair<int32, MapEvent *> > _active_delayed_events;

    /** \brief A list of all events that are waiting on their launch timers to expire before being started
    *** The interger part of this std::pair is the countdown timer for this event to be launched
    *** Those ones are put on hold by PauseAllEvents() and PauseEvents();
    **/
    std::vector<std::pair<int32, MapEvent *> > _paused_delayed_events;

    /** States whether the event supervisor is parsing the active events queue, thus any modifications
    *** there on active events should be avoided.
    **/
    volatile bool _is_updating;

    /** \brief A function that is called whenever an event starts or finishes to examine that event's links
    *** \param parent_event The event that has just started or finished
    *** \param event_start The event has just started if this member is true, or if it just finished it will be false
    **/
    void _ExamineEventLinks(MapEvent *parent_event, bool event_start);
}; // class EventSupervisor

} // namespace private_map

} // namespace vt_map

#endif // __MAP_EVENTS_HEADER__
