/////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_mode.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for map mode interface.
***
*** This file contains the interface for map mode, active when the player is
*** exploring town or dungeon maps. The map environments are
*** quite extensive, thus this code is responsible for processing many things.
*** This includes handling all tile images, objects, sprites, map events,
*** dialogue, and more.
***
*** Each individual map is represented by it's own object of the MapMode class.
*** It is our intention in the future to retain more than one map in memory at
*** once to reduce or eliminate loading times when the player transitions
*** between maps.
*** ***************************************************************************/

#ifndef __MAP_HEADER__
#define __MAP_HEADER__

#include "engine/mode_manager.h"
#include "engine/system.h"

#include "map_utils.h"
#include "map_minimap.h"

#include "engine/audio/audio_descriptor.h"

namespace vt_defs {
void BindModeCode();
}

namespace vt_global {
class GlobalEnemy;
class GlobalObject;
}

//! All calls to map mode are wrapped in this namespace.
namespace vt_map
{

//! An internal namespace to be used only within the map code. Don't use this namespace anywhere else!
namespace private_map
{
class DialogueSupervisor;
class EventSupervisor;
class Light;
class MapObject;
class MapZone;
class Minimap;
class ObjectSupervisor;
class PhysicalObject;
class SoundObject;
class TileSupervisor;
class TreasureObject;
class TreasureSupervisor;
} // namespace private_map

/** ****************************************************************************
*** \brief Handles the game execution while the player is exploring maps.
***
*** This class contains all of the structures that together compose each map.
*** Each map has a Lua script file in which the map data is permanently retained
*** and various script subroutines exist that modify the map's behavior. Keep in
*** mind that this class alone does not represent all of the data nor all of the
*** code that is used in a particular map, as the map's Lua file likely retains
*** additional information about the map that is not represented in this class.
***
*** Maps are composed by a series of tiles and objects. Tiles are 32x32 pixel
*** squares that are adjacent to one another on a map, and together make up the
*** map's background environment. Objects are variable sized entities that are
*** usually living, animated creatures (sprites), but may be something static
*** such as a large tree. Tiles and objects are drawn in multiple interwieving
*** layers to emulate a 3D environment for the game. Additionally each map has
*** a collision grid composed of 16x16 pixel elements that determine which
*** quadrants of each tile may be occupied by sprites or other objects.
***
*** Another important concept to MapMode is that of contexts. Each map has at
*** least one context and can have up to a maximum of 32 contexts. Every context
*** has its own collision grid and its own set of tiles. Map objects and sprites
*** exist in one of these context and may change their context at any time.
*** Objects and sprites can not interact with one another when they are not
*** within the same context, and typically only objects that are in the same
*** context as the map camera are visible on the screen. You can think of each
*** context as essentially its own map, and the set of contexts as a set of maps
*** that work with one another to create a cohesive environment.
***
*** Because this game mode is so complex, the MapMode class shares its
*** responsibilities with several small singleton classes that manage a
*** particular area of map code, such as tiles or objects. These sub-manager
*** classes should be viewed as extensions of the MapMode class.
*** ***************************************************************************/
class MapMode : public vt_mode_manager::GameMode
{
    friend void vt_defs::BindModeCode();

public:
    //! \param data_filename The name of the Lua file that retains all data about the map to create
    //! \param script_filename The name of the Lua file that retains all data about script to load
    MapMode(const std::string &data_filename, const std::string& script_filename);

    ~MapMode();

    //! \brief Resets appropriate class members. Called whenever the MapMode object is made the active game mode.
    void Reset();

    //! \brief Called when a map mode is deactivated.
    //!  Note that this is called twice when doing a battle transition.
    void Deactivate();

    //! \brief Updates the game and calls various sub-update functions depending on the current state of map mode.
    void Update();

    //! \brief The highest level draw function that will call the appropriate lower-level draw functions
    void Draw();

    //! \brief The highest level draw function for stuff unaffected by light and fade effects.
    void DrawPostEffects();

    // The methods below this line are not intended to be used outside of the map code

    //! \brief Empties the state stack and places an invalid state on top
    void ResetState();

    /** \brief Pushes a state type to the top of the state stack, making it the active state
    *** \param state The state to push onto the stack
    **/
    void PushState(private_map::MAP_STATE state);

    //! \brief Removes the highest item in the state stack
    void PopState();

    /** \brief Retrieves the current map state
    *** \return The top-most item on the map state stack
    **/
    private_map::MAP_STATE CurrentState();

    //! \brief Adds a new object to the layer before ground object layer
    void AddFlatGroundObject(private_map::MapObject *obj);

    //! \brief Adds a new object to the ground object layer
    void AddGroundObject(private_map::MapObject *obj);

    //! \brief Adds a new object to the pass object layer
    void AddPassObject(private_map::MapObject *obj);

    //! \brief Adds a new object to the sky object layer
    void AddSkyObject(private_map::MapObject *obj);

    //! \brief Adds a new ambient sound object
    void AddAmbientSoundObject(private_map::SoundObject *obj);

    //! \brief Adds a new zone to the map
    void AddZone(private_map::MapZone *zone);

    //! \brief Adds a save point (Map Object) at the given coordinates.
    void AddSavePoint(float x, float y);

    //! \brief Adds a halo light (as Map Object) at the given coordinates.
    void AddHalo(const std::string &filename, float x, float y, const vt_video::Color &color);

    //! \brief Add a light source at the given coordinates.
    void AddLight(const std::string &main_flare_filename,
                  const std::string &secondary_flare_filename,
                  float x, float y,
                  const vt_video::Color &main_color,
                  const vt_video::Color &secondary_color);

    //! \brief Add a light object, often created through scripting
    void AddLight(private_map::Light *light);

    //! \brief Vectors containing the save points animations (when the character is in or not).
    std::vector<vt_video::AnimatedImage> active_save_point_animations;
    std::vector<vt_video::AnimatedImage> inactive_save_point_animations;

    //! \brief Class member accessor functions
    //@{
    static MapMode *CurrentInstance() {
        return _current_instance;
    }

    const vt_utils::ustring &GetMapHudName() const {
        return _map_hud_name;
    }

    uint32 GetStamina(){
        return _run_stamina;
    }

    void SetStamina(uint32 new_stamina){
        _run_stamina = new_stamina;
    }

    // Note: The map script is only valid while in loading the map file.
    // The file is closed afterward to permit the save menu to open it, for instance.
    vt_script::ReadScriptDescriptor &GetMapScript() {
        return _map_script;
    }

    /** \brief Opens the map tablespace of the map script
    *** \param use_global Has the same effect as in ReadScriptDescriptor#OpenTable
    *** \returns whether the tablespace opening was successful.
    **/
    bool OpenMapTablespace(bool use_global = false) {
        return _map_script.OpenTable(_map_script_tablespace, use_global);
    }

    const std::string &GetMapScriptFilename() const {
        return _map_script_filename;
    }

    private_map::TileSupervisor *GetTileSupervisor() const {
        return _tile_supervisor;
    }

    private_map::ObjectSupervisor *GetObjectSupervisor() const {
        return _object_supervisor;
    }

    private_map::EventSupervisor *GetEventSupervisor() const {
        return _event_supervisor;
    }

    private_map::DialogueSupervisor *GetDialogueSupervisor() const {
        return _dialogue_supervisor;
    }

    private_map::TreasureSupervisor *GetTreasureSupervisor() const {
        return _treasure_supervisor;
    }

    const private_map::MapFrame &GetMapFrame() const {
        return _map_frame;
    }

    private_map::VirtualSprite *GetCamera() const {
        return _camera;
    }

    void SetCamera(private_map::VirtualSprite *sprite) {
        _camera = sprite;
    }

    void SetCamera(private_map::VirtualSprite *sprite, uint32 duration);

    void MoveVirtualFocus(float loc_x, float loc_y);;

    void MoveVirtualFocus(float loc_x, float loc_y, uint32 duration);

    bool IsCameraOnVirtualFocus();

    bool IsCameraMoving() {
        return _camera_timer.IsRunning();
    }

    bool IsCameraXAxisInMapCorner() const {
        return _camera_x_in_map_corner;
    }

    bool IsCameraYAxisInMapCorner() const {
        return _camera_y_in_map_corner;
    }

    bool IsShowGUI() const {
        return _show_gui;
    }

    void SetShowGUI(bool state) {
        _show_gui = state;
    }

    const vt_video::AnimatedImage &GetDialogueIcon() const {
        return _dialogue_icon;
    }

    const vt_video::StillImage &GetMapImage() const {
        return _map_image;
    }

    //! \brief Tells whether a battle can start
    bool AttackAllowed();

    /**
     * \brief Since the map coords are non standard, this function
     * permits to quickly adapt the images to the map scale.
     */
    static void ScaleToMapCoords(vt_video::ImageDescriptor &img) {
        img.SetDimensions(img.GetWidth() / (private_map::GRID_LENGTH / 2),
                          img.GetHeight() / (private_map::GRID_LENGTH / 2));
    }

    //! \brief Returns in standard screen coordinates (1024x768),
    //! the x position of a tile position on the X axis.
    float GetScreenXCoordinate(float tile_position_x) const;

    //! \brief Returns in standard screen coordinates (1024x768),
    //! the x position of a tile position on the Y axis.
    float GetScreenYCoordinate(float tile_position_y) const;

    //! \brief toggles visibility of the minimap
    //! \param the new state of the minimap visibility
    //! \return the previous state of visibility before changing
    bool ShowMinimap(bool visibility) {
        bool previous_visibility = _show_minimap;
        _show_minimap = visibility;
        return previous_visibility;
    }

    //! \brief Tells the system to load a custom minimap image.
    void SetMinimapImage(const std::string& filename) {
        _minimap_custom_image_file = filename;
        // This also implies the minimap will be shown
        ShowMinimap(true);
    }

    //@}

private:
    // ----- Members : Names and Identifiers -----

    /** \brief A reference to the current instance of MapMode
    *** This is used by other map clases to be able to refer to the map that they exist in.
    **/
    static MapMode *_current_instance;

    //! Tells whether the mode is activated. It is true by calling Reset(),
    //! and false when calling Deactivate(). This member exists to prevent
    //! the triggering of deactivate more than once.
    bool _activated;

    //! \brief The name of the Lua file that contains the map data
    std::string _map_data_filename;

    /** \brief The map's data unique name as it is used to identify a Lua namespace table
    *** To avoid Lua naming conflicts between multiple scripting files, all map data is encompassed within
    *** a namespace (a Lua table) that is unique to each map.
    **/
    std::string _map_data_tablespace;

    //! \brief The name of the Lua file that contains the map script
    std::string _map_script_filename;

    //! \brief The map's script unique name as it is used to identify a Lua namespace table
    std::string _map_script_tablespace;

    //! \brief The name of the map, as it will be read by the player in the game.
    vt_utils::ustring _map_hud_name;

    //! \brief The map sub-part hud name. Shown in every cases.
    vt_utils::ustring _map_hud_subname;

    /** \brief The interface to the file which contains all the map's stored data and subroutines.
    *** This class generally performs a large amount of communication with this script continuously.
    *** The script remains open for as long as the MapMode object exists.
    **/
    vt_script::ReadScriptDescriptor _map_script;

    // ----- Members : Supervisor Class Objects and Script Functions -----

    //! \brief Instance of helper class to map mode. Responsible for tile related operations.
    private_map::TileSupervisor *_tile_supervisor;

    //! \brief Instance of helper class to map mode. Responsible for object and sprite related operations.
    private_map::ObjectSupervisor *_object_supervisor;

    //! \brief Instance of helper class to map mode. Responsible for updating and managing active map events.
    private_map::EventSupervisor *_event_supervisor;

    //! \brief Instance of helper class to map mode. Responsible for dialogue execution and display operations.
    private_map::DialogueSupervisor *_dialogue_supervisor;

    //! \brief Instance of helper class to map mode. Responsible for processing all information related to treasure discovery.
    private_map::TreasureSupervisor *_treasure_supervisor;

    /** \brief A script function which assists with the MapMode#Update method
    *** This function implements any custom update code that the specific map needs to be performed.
    *** The most common operation that this script function performs is to check for trigger conditions
    *** that cause map events to occur
    **/
    ScriptObject _update_function;

    // ----- Members : Properties and State -----

    //! \brief Retains information needed to correctly draw the next map frame
    private_map::MapFrame _map_frame;

    /** \brief Indicates whether we are in a map corner for one of the other axis,
    *** thus disabling any sprite shifting parallax for this axis
    *** since the map view won't move in that direction.
    **/
    bool _camera_x_in_map_corner;
    bool _camera_y_in_map_corner;

    //! \brief A pointer to the map sprite that the map camera will focus on
    private_map::VirtualSprite *_camera;

    //! \brief The way in x-direction, the camera will move
    float _delta_x;

    //! \brief The way in y-direction, the camera will move
    float _delta_y;

    //! \brief A time for camera movement
    vt_system::SystemTimer _camera_timer;

    //! \brief The number of contexts that this map uses (at least 1, at most 32)
    uint8 _num_map_contexts;

    //! \brief If true, the player is not allowed to run.
    bool _running_disabled;

    //! \brief If true, the player's stamina will not drain for actions and the stamina bar will not be shown
    bool _unlimited_stamina;

    //! \brief When false, dialogue icons, stamina bar, and other GUI elements will not be drawn to the screen
    bool _show_gui;

    /** \brief A counter for the player's stamina
    *** This value ranges from STAMINA_EMPTY to STAMINA_FULL. It takes twice as long to regenerate stamina as
    *** it does to consume it when running.
    **/
    uint32 _run_stamina;

    /** \brief the alpha value used to create fade in/out of the map GUI when switching in
    *** or out of the STATE_SCENE or DIALOGUE state.
    **/
    float _gui_alpha;

    /** \brief Maintains a stack state for the different modes of operation that the map may be in
    *** The top (back) of the stack is the active mode
    **/
    std::vector<private_map::MAP_STATE> _state_stack;

    // ----- Members : Timing and Graphics -----

    /** \brief A timer used for when the player first enters the map
    *** This timer is set to 7000 ms (7 seconds) and is used to display the map's location graphic
    *** and name at the top center of the screen. The graphic and text are faded in for the first
    *** two seconds, drawn opaquely for the next three seconds, and faded out in the final two seconds.
    **/
    vt_system::SystemTimer _intro_timer;

    //! \brief Freestyle art image of the current map
    vt_video::StillImage _map_image;

    //! \brief An icon graphic which appears over the heads of NPCs who have dialogue that has not been read by the player
    vt_video::AnimatedImage _dialogue_icon;

    //! \brief Image which underlays the stamina bar for running
    vt_video::StillImage _stamina_bar_background;

    //! \brief Image which overlays the stamina bar to show that the player has unlimited running
    vt_video::StillImage _stamina_bar_infinite_overlay;

    // ----- Members : Containers -----
    /** \brief A container for the various foes which may appear on this map
    *** These enemies are kept at their initial stats. When they are passed to battle mode,
    *** a copy of each enemy is made and initialized there.
    **/
    std::vector<vt_global::GlobalEnemy *> _enemies;

    /**
    *** Stores the first music filename loaded by the map.
    *** Used to auto-play it at map startup.
    *** NOTE: Other audio handling will have to be used through scripting.
    **/
    std::string _music_filename;
    vt_audio::AUDIO_STATE _audio_state;

    //! \brief the minimap for the current map instance
    private_map::Minimap *_minimap;

    //! \brief flag that enables minimap rendering or not
    bool _show_minimap;

    //! \brief Stores the potential custom minimap image filename
    std::string _minimap_custom_image_file;

    // ----- Methods -----

    //! \brief Loads all map data contained in the Lua file that defines the map
    bool _Load();

    /** Triggers the minimap creation either by trying to load the minimap file given.
    *** Or by creating a minimap procedurally.
    **/
    bool _CreateMinimap();

    //! \brief A helper function to Update() that is called only when the map is in the explore state
    void _UpdateExplore();

    //! \brief Update the map frame coordinates
    void _UpdateMapFrame();

    //! \brief Draws all visible map tiles and sprites to the screen
    void _DrawMapLayers();

    //! \brief Draws all GUI visuals, such as dialogue icons and the stamina bar
    void _DrawGUI();

    //! \brief Draws the stamina bar, part of DrawGUI()
    void _DrawStaminaBar(const vt_video::Color &blending = vt_video::Color::white);

    //! \brief Draws the map layer tile and collision grid.
    void _DrawDebugGrid();
}; // class MapMode

} // namespace vt_map;

#endif
