///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    defs.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Header file for forward declarations of classes and debug variables.
***
*** This file serves two purposes. The first purpose of this file is to forward
*** declare classes and shared variables in order to avoid problems with
*** recursive inclusion. The second purpose of this file is to declare
*** the function that contains all of the Allacrost Lua binding code. This makes
*** the C++ engine code available for use in Lua scripts.
***
*** \note Pretty much every header file in the source tree will need to include
*** this file, with a few exceptions (utils.h is one). The only source file
*** that should need to include this file is defs.cpp
***
*** \note The commenting for all namespaces, variables, and classes declared in
*** this file can be found in the respective header files for where these
*** structures reside in. There are no doxygen comments for the classes and
*** namespaces found here.
***
*** \note You should not need to use the hoa_defs namespace unless you are
*** making the call to bind the engine to Lua.
*** **************************************************************************/

#ifndef __DEFS_HEADER__
#define __DEFS_HEADER__

////////////////////////////////////////////////////////////////////////////////
// Game Engine Declarations
////////////////////////////////////////////////////////////////////////////////

// Audio declarations, see src/engine/audio/
namespace hoa_audio {
	extern bool AUDIO_DEBUG;
	class AudioEngine;

	class AudioDescriptor;
	class MusicDescriptor;
	class SoundDescriptor;

	namespace private_audio {
		class AudioCacheElement;

		class AudioBuffer;
		class AudioSource;
		class AudioStream;

		class AudioInput;
		class WavFile;
		class OggFile;
		class AudioMemory;

		class AudioEffect;
		class FadeInEffect;
		class FadeOutEffect;
	}
}

// Video declarations, see src/engine/video/
namespace hoa_video {
	extern bool VIDEO_DEBUG;
	class VideoEngine;

	class Color;
	class CoordSys;
	class ScreenRect;

	class FixedImageNode;
	class VariableImageNode;

	class ImageDescriptor;
	class StillImage;
	class AnimatedImage;
	class CompositeImage;

	class TextureController;

	class TextSupervisor;
	class FontGlyph;
	class FontProperties;
	class TextImage;

	class Interpolator;

	class ParticleEffect;
	class ParticleEffectDef;
	class ParticleEmitter;
	class EffectParameters;

	namespace private_video {
		class Context;

		class TexSheet;
		class FixedTexSheet;
		class VariableTexSheet;
		class FixedTexNode;
		class VariableTexNode;

		class ImageMemory;

		class BaseTexture;
		class ImageTexture;
		class TextTexture;
		class TextElement;
		class AnimationFrame;
		class ImageElement;

		class ParticleManager;
		class ParticleSystem;
		class ParticleSystemDef;
		class Particle;
		class ParticleVertex;
		class ParticleTexCoord;
		class ParticleKeyframe;

		class ScreenFader;
		class ShakeForce;
	}
}

// Script declarations, see src/engine/script/
namespace hoa_script {
	extern bool SCRIPT_DEBUG;
	class ScriptEngine;

	class ScriptDescriptor;
	class ReadScriptDescriptor;
	class WriteScriptDescriptor;
	class ModifyScriptDescriptor;
}

// Mode manager declarations, see src/engine/
namespace hoa_mode_manager {
	extern bool MODE_MANAGER_DEBUG;
	class ModeEngine;

	class GameMode;
}

// Input declarations, see src/engine/
namespace hoa_input {
	extern bool INPUT_DEBUG;
	class InputEngine;
}

// Settings declarations, see src/engine/
namespace hoa_system {
	extern bool SYSTEM_DEBUG;
	class SystemEngine;
	class Timer;
}

////////////////////////////////////////////////////////////////////////////////
// Common Code Declarations
////////////////////////////////////////////////////////////////////////////////

// Common declarations, see src/common
namespace hoa_common {
	extern bool COMMON_DEBUG;
	class CommonDialogue;
	class CommonDialogueOptions;
	class CommonDialogueWindow;
	class CommonDialogueSupervisor;
}

// Global declarations, see src/common/global/
namespace hoa_global {
	extern bool GLOBAL_DEBUG;
	class GameGlobal;
	class GlobalEventGroup;

	class GlobalObject;
	class GlobalItem;
	class GlobalWeapon;
	class GlobalArmor;
	class GlobalShard;
	class GlobalKeyItem;

	class GlobalStatusEffect;
	class GlobalElementalEffect;
	class GlobalSkill;

	class GlobalAttackPoint;
	class GlobalActor;
	class GlobalCharacter;
	class GlobalCharacterGrowth;
	class GlobalEnemy;
	class GlobalParty;
}

// GUI declarations, see src/common/gui
namespace hoa_gui {
	class GUISystem;
	class MenuWindow;
	class TextBox;
	class OptionBox;

	namespace private_gui {
		class GUIElement;
		class GUIControl;
		class MenuSkin;

		class Option;
		class OptionElement;
		class OptionCellBounds;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Game Mode Declarations
////////////////////////////////////////////////////////////////////////////////

// Battle mode declarations, see src/modes/battle/
namespace hoa_battle {
	extern bool BATTLE_DEBUG;
	class BattleMode;

	namespace private_battle {
		class BattleMedia;

		class SequenceSupervisor;

		class BattleActor;
		class BattleCharacter;
		class BattleEnemy;

		class BattleAction;
		class SkillAction;
		class ItemAction;

		class BattleTimer;
		class BattleTarget;
		class BattleItem;

		class BattleSpeaker;
		class BattleDialogue;
		class DialogueSupervisor;

		class BattleStatusEffect;
		class EffectsSupervisor;

		class IndicatorElement;
		class IndicatorText;
		class IndicatorImage;
		class IndicatorSupervisor;

		class ItemCommand;
		class SkillCommand;
		class CharacterCommand;
		class CommandSupervisor;

		class FinishDefeatAssistant;
		class FinishVictoryAssistant;
		class FinishSupervisor;
	}
}

// Boot mode declarations, see src/modes/boot/
namespace hoa_boot {
	extern bool BOOT_DEBUG;
	class BootMode;

	namespace private_boot {
		class BootMenu;
		class CreditsWindow;
		class WelcomeWindow;
	}
}

// Map mode declarations, see src/modes/map/
namespace hoa_map {
	extern bool MAP_DEBUG;
	class MapMode;

	namespace private_map {
		class TileSupervisor;
		class MapTile;

		class MapRectangle;
		class MapFrame;
		class PathNode;

		class ObjectSupervisor;
		class MapObject;
		class PhysicalObject;
		class TreasureObject;

		class VirtualSprite;
		class MapSprite;
		class EnemySprite;

		class DialogueSupervisor;
		class SpriteDialogue;
		class MapDialogueOptions;

		class EventSupervisor;
		class MapEvent;
		class DialogueEvent;
		class ScriptedEvent;
		class ShopEvent;
		class SoundEvent;
		class MapTransitionEvent;
		class JoinPartyEvent;
		class BattleEncounterEvent;
		class SpriteEvent;
		class ScriptedSpriteEvent;
		class ChangeDirectionSpriteEvent;
		class PathMoveSpriteEvent;
		class RandomMoveSpriteEvent;
		class AnimateSpriteEvent;

		class TreasureSupervisor;
		class MapTreasure;

		class ZoneSection;
		class MapZone;
		class ResidentZone;
		class EnemyZone;
		class ContextZone;
	}
}

// Menu mode declarations, see src/modes/menu/
namespace hoa_menu {
	extern bool MENU_DEBUG;
	class MenuMode;
}

// Pause mode declarations, see src/modes/
namespace hoa_pause {
	extern bool PAUSE_DEBUG;
	class PauseMode;
}

// Scene mode declarations, see src/modes/
namespace hoa_scene {
	extern bool SCENE_DEBUG;
	class SceneMode;
}

// Shop mode declarations, see src/modes/shop/
namespace hoa_shop {
	extern bool SHOP_DEBUG;
	class ShopMode;

	namespace private_shop {
		class ShopMedia;
		class ShopInterface;
		class ShopObject;
		class ShopObjectViewer;
		class ObjectListDisplay;

		class RootInterface;
		class CategoryDrawData;

		class BuyInterface;
		class BuyListDisplay;

		class SellInterface;
		class SellListDisplay;

		class TradeInterface;

		class ConfirmInterface;

		class LeaveInterface;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Miscellaneous Declarations
////////////////////////////////////////////////////////////////////////////////

// Utils declarations, see src/utils.h
namespace hoa_utils {
	extern bool UTILS_DEBUG;
	class ustring;
	class Exception;
	extern float RandomFloat();
}

////////////////////////////////////////////////////////////////////////////////
// Binding Declarations
////////////////////////////////////////////////////////////////////////////////

//! \brief Namespace which contains all binding functions
namespace hoa_defs {

/** \brief Contains the binding code which makes the C++ engine available to Lua
*** This method should <b>only be called once</b>. It must be called after the
*** ScriptEngine is initialized, otherwise the application will crash.
**/
void BindEngineCode();
void BindCommonCode();
void BindModeCode();

} // namespace hoa_defs

#endif // __DEFS_HEADER__
