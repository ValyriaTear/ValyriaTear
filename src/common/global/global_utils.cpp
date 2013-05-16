////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_utils.cpp
*** \author  Tyler Olsen, rootslinux@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for global game utility code
*** ***************************************************************************/

#include "global_utils.h"
#include "global.h"

#include "engine/system.h"
#include "engine/audio/audio_descriptor.h"

using namespace vt_utils;
using namespace vt_system;
using namespace vt_global::private_global;

namespace vt_global
{

uint32 GetEquipmentPositionFromObjectType(GLOBAL_OBJECT object_type)
{
    switch (object_type) {
    default:
       break;
    case GLOBAL_OBJECT_HEAD_ARMOR:
        return GLOBAL_POSITION_HEAD;
    case GLOBAL_OBJECT_TORSO_ARMOR:
        return GLOBAL_POSITION_TORSO;
    case GLOBAL_OBJECT_ARM_ARMOR:
        return GLOBAL_POSITION_ARMS;
    case GLOBAL_OBJECT_LEG_ARMOR:
        return GLOBAL_POSITION_LEGS;
    }
     return GLOBAL_POSITION_INVALID;
}

std::string GetTargetText(GLOBAL_TARGET target)
{
    switch(target) {
    case GLOBAL_TARGET_SELF_POINT:
        return Translate("Self — Point");
    case GLOBAL_TARGET_ALLY_POINT:
        return Translate("Ally — Point");
    case GLOBAL_TARGET_FOE_POINT:
        return Translate("Foe — Point");
    case GLOBAL_TARGET_SELF:
        return Translate("Self");
    case GLOBAL_TARGET_ALLY:
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
        return Translate("Ally");
    case GLOBAL_TARGET_FOE:
        return Translate("Foe");
    case GLOBAL_TARGET_ALL_ALLIES:
        return Translate("All Allies");
    case GLOBAL_TARGET_ALL_FOES:
        return Translate("All Foes");
    default:
        return Translate("Invalid Target");
    }
}



bool IsTargetPoint(GLOBAL_TARGET target)
{
    if((target == GLOBAL_TARGET_SELF_POINT) || (target == GLOBAL_TARGET_ALLY_POINT) || (target == GLOBAL_TARGET_FOE_POINT))
        return true;
    else
        return false;
}



bool IsTargetActor(GLOBAL_TARGET target)
{
    if((target == GLOBAL_TARGET_SELF) || (target == GLOBAL_TARGET_ALLY) || (target == GLOBAL_TARGET_FOE)
            || (target == GLOBAL_TARGET_ALLY_EVEN_DEAD))
        return true;
    else
        return false;
}



bool IsTargetParty(GLOBAL_TARGET target)
{
    if((target == GLOBAL_TARGET_ALL_ALLIES) || (target == GLOBAL_TARGET_ALL_FOES))
        return true;
    else
        return false;
}



bool IsTargetSelf(GLOBAL_TARGET target)
{
    if((target == GLOBAL_TARGET_SELF_POINT) || (target == GLOBAL_TARGET_SELF))
        return true;
    else
        return false;
}



bool IsTargetAlly(GLOBAL_TARGET target)
{
    if((target == GLOBAL_TARGET_ALLY_POINT) || (target == GLOBAL_TARGET_ALLY) || (target == GLOBAL_TARGET_ALL_ALLIES)
            || (target == GLOBAL_TARGET_ALLY_EVEN_DEAD))
        return true;
    else
        return false;
}



bool IsTargetFoe(GLOBAL_TARGET target)
{
    if((target == GLOBAL_TARGET_FOE_POINT) || (target == GLOBAL_TARGET_FOE) || (target == GLOBAL_TARGET_ALL_FOES))
        return true;
    else
        return false;
}



GlobalObject *GlobalCreateNewObject(uint32 id, uint32 count)
{
    GlobalObject *new_object = NULL;

    if((id > 0 && id <= MAX_ITEM_ID)
        || (id > MAX_SPIRIT_ID && id <= MAX_KEY_ITEM_ID))
        new_object = new GlobalItem(id, count);
    else if((id > MAX_ITEM_ID) && (id <= MAX_WEAPON_ID))
        new_object = new GlobalWeapon(id, count);
    else if((id > MAX_WEAPON_ID) && (id <= MAX_LEG_ARMOR_ID))
        new_object = new GlobalArmor(id, count);
    else if((id > MAX_LEG_ARMOR_ID) && (id <= MAX_SPIRIT_ID))
        new_object = new GlobalSpirit(id, count);
    else
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "function received an invalid id argument: " << id << std::endl;

    // If an object was created but its ID was set to NULL, this indicates that the object is invalid
    if((new_object != NULL) && (new_object->GetID() == 0)) {
        delete new_object;
        new_object = NULL;
    }

    return new_object;
}



bool IncrementIntensity(GLOBAL_INTENSITY &intensity, uint8 amount)
{
    if(amount == 0)
        return false;
    if((intensity <= GLOBAL_INTENSITY_INVALID) || (intensity >= GLOBAL_INTENSITY_POS_EXTREME))
        return false;

    // This check protects against overflow conditions
    if(amount > (GLOBAL_INTENSITY_TOTAL * 2)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to increment intensity by an excessive amount: " << amount << std::endl;
        if(intensity == GLOBAL_INTENSITY_POS_EXTREME) {
            return false;
        } else {
            intensity = GLOBAL_INTENSITY_POS_EXTREME;
            return true;
        }
    }

    intensity = GLOBAL_INTENSITY(intensity + amount);
    if(intensity >= GLOBAL_INTENSITY_TOTAL)
        intensity = GLOBAL_INTENSITY_POS_EXTREME;
    return true;
}



bool DecrementIntensity(GLOBAL_INTENSITY &intensity, uint8 amount)
{
    if(amount == 0)
        return false;
    if((intensity <= GLOBAL_INTENSITY_NEG_EXTREME) || (intensity >= GLOBAL_INTENSITY_TOTAL))
        return false;

    // This check protects against overflow conditions
    if(amount > (GLOBAL_INTENSITY_TOTAL * 2)) {
        IF_PRINT_WARNING(GLOBAL_DEBUG) << "attempted to decrement intensity by an excessive amount: " << amount << std::endl;
        if(intensity == GLOBAL_INTENSITY_NEG_EXTREME) {
            return false;
        } else {
            intensity = GLOBAL_INTENSITY_NEG_EXTREME;
            return true;
        }
    }

    intensity = GLOBAL_INTENSITY(intensity - amount);
    if(intensity <= GLOBAL_INTENSITY_INVALID)
        intensity = GLOBAL_INTENSITY_NEG_EXTREME;
    return true;
}

GLOBAL_STATUS GetOppositeStatusEffect(GLOBAL_STATUS status_effect)
{
    switch (status_effect) {
    default:
        return GLOBAL_STATUS_INVALID;

    case GLOBAL_STATUS_STRENGTH_RAISE:
        return GLOBAL_STATUS_STRENGTH_LOWER;
    case GLOBAL_STATUS_STRENGTH_LOWER:
        return GLOBAL_STATUS_STRENGTH_RAISE;

    case GLOBAL_STATUS_VIGOR_RAISE:
        return GLOBAL_STATUS_VIGOR_LOWER;
    case GLOBAL_STATUS_VIGOR_LOWER:
        return GLOBAL_STATUS_VIGOR_RAISE;

    case GLOBAL_STATUS_FORTITUDE_RAISE:
        return GLOBAL_STATUS_FORTITUDE_LOWER;
    case GLOBAL_STATUS_FORTITUDE_LOWER:
        return GLOBAL_STATUS_FORTITUDE_RAISE;

    case GLOBAL_STATUS_PROTECTION_RAISE:
        return GLOBAL_STATUS_PROTECTION_LOWER;
    case GLOBAL_STATUS_PROTECTION_LOWER:
        return GLOBAL_STATUS_PROTECTION_RAISE;

    case GLOBAL_STATUS_AGILITY_RAISE:
        return GLOBAL_STATUS_AGILITY_LOWER;
    case GLOBAL_STATUS_AGILITY_LOWER:
        return GLOBAL_STATUS_AGILITY_RAISE;

    case GLOBAL_STATUS_EVADE_RAISE:
        return GLOBAL_STATUS_EVADE_LOWER;
    case GLOBAL_STATUS_EVADE_LOWER:
        return GLOBAL_STATUS_EVADE_RAISE;

    case GLOBAL_STATUS_HP_REGEN:
        return GLOBAL_STATUS_HP_DRAIN;
    case GLOBAL_STATUS_HP_DRAIN:
        return GLOBAL_STATUS_HP_REGEN;

    case GLOBAL_STATUS_SP_REGEN:
        return GLOBAL_STATUS_SP_DRAIN;
    case GLOBAL_STATUS_SP_DRAIN:
        return GLOBAL_STATUS_SP_REGEN;
    }
}

// GlobalMedia functions

void GlobalMedia::Initialize()
{
    // Load common images
    if (!_drunes_icon.Load("img/icons/drunes.png"))
        PRINT_WARNING << "Failed to load drunes icon image" << std::endl;

    if (!_star_icon.Load("img/menus/star.png"))
        PRINT_WARNING << "Failed to load star icon image" << std::endl;

    if (!_check_icon.Load("img/menus/green_check.png"))
        PRINT_WARNING << "Failed to load check icon image" << std::endl;

    if (!_x_icon.Load("img/menus/red_x.png"))
        PRINT_WARNING << "Failed to load x icon image" << std::endl;

    if (!_spirit_slot_icon.Load("img/menus/spirit.png"))
        PRINT_WARNING << "Failed to load spirit icon image" << std::endl;

    if (!_equip_icon.Load("img/menus/equip.png"))
        PRINT_WARNING << "Failed to load equip icon image" << std::endl;

    if (!_key_item_icon.Load("img/menus/key.png"))
        PRINT_WARNING << "Failed to load key item icon image" << std::endl;

    if(!vt_video::ImageDescriptor::LoadMultiImageFromElementGrid(_elemental_icons, "img/icons/effects/elemental.png", 7, 9))
        PRINT_WARNING << "Failed to load elemental icon images" << std::endl;

    if(!vt_video::ImageDescriptor::LoadMultiImageFromElementSize(_status_icons, "img/icons/effects/status.png", 25, 25))
        PRINT_WARNING << "Failed to load status icon images" << std::endl;

    if(!vt_video::ImageDescriptor::LoadMultiImageFromElementGrid(_all_category_icons, "img/icons/object_category_icons.png", 3, 3))
        PRINT_WARNING << "Failed to load object category icon images" << std::endl;

    // Load common sounds
    _LoadSoundFile("confirm", "snd/confirm.wav");
    _LoadSoundFile("cancel", "snd/cancel.wav");
    _LoadSoundFile("coins", "snd/coins.wav");
    _LoadSoundFile("bump", "snd/bump.wav");
    _LoadSoundFile("volume_test", "snd/volume_test.wav");
    _LoadSoundFile("item_pickup", "snd/itempick2_michel_baradari_oga.wav");
}

GlobalMedia::~GlobalMedia()
{
    // Clear up sounds
    for(std::map<std::string, vt_audio::SoundDescriptor *>::iterator it = _sounds.begin(); it != _sounds.end(); ++it)
        delete it->second;
    _sounds.clear();
}

vt_video::StillImage* GlobalMedia::GetElementalIcon(GLOBAL_ELEMENTAL element_type, GLOBAL_INTENSITY intensity)
{
    const uint32 NUMBER_INTENSTIY_LEVELS = 9;

    // Row/col coordinates for where the specific icon can be found in the multi image array
    uint32 row = 0, col = 0;

    // Elemental type determines the icon's row
    switch(element_type) {
    case GLOBAL_ELEMENTAL_FIRE:
        row = 0;
        break;
    case GLOBAL_ELEMENTAL_WATER:
        row = 1;
        break;
    case GLOBAL_ELEMENTAL_VOLT:
        row = 2;
        break;
    case GLOBAL_ELEMENTAL_EARTH:
        row = 3;
        break;
    case GLOBAL_ELEMENTAL_LIFE:
        row = 4;
        break;
    case GLOBAL_ELEMENTAL_DEATH:
        row = 5;
        break;
    case GLOBAL_ELEMENTAL_NEUTRAL:
        row = 6;
        break;
    default:
        PRINT_WARNING << "Invalid elemental type: " << element_type << std::endl;
        return NULL;
    }

    // Intensity determines the icon's column
    switch(intensity) {
    case GLOBAL_INTENSITY_POS_EXTREME:
        col = 0;
        break;
    case GLOBAL_INTENSITY_POS_GREATER:
        col = 1;
        break;
    case GLOBAL_INTENSITY_POS_MODERATE:
        col = 2;
        break;
    case GLOBAL_INTENSITY_POS_LESSER:
        col = 3;
        break;
    case GLOBAL_INTENSITY_NEUTRAL:
        col = 4;
        break;
    case GLOBAL_INTENSITY_NEG_LESSER:
        col = 5;
        break;
    case GLOBAL_INTENSITY_NEG_MODERATE:
        col = 6;
        break;
    case GLOBAL_INTENSITY_NEG_GREATER:
        col = 7;
        break;
    case GLOBAL_INTENSITY_NEG_EXTREME:
        col = 8;
        break;
    default:
        PRINT_WARNING << "Invalid intensity level: " << intensity << std::endl;
        return NULL;
    }

    return &(_elemental_icons[(row * NUMBER_INTENSTIY_LEVELS) + col]);
}

vt_video::StillImage* GlobalMedia::GetItemCategoryIcon(GLOBAL_OBJECT object_type)
{
    uint32 index = 0;

    switch(object_type) {
    case GLOBAL_OBJECT_ITEM:
        index = 0;
        break;
    case GLOBAL_OBJECT_WEAPON:
        index = 1;
        break;
    case GLOBAL_OBJECT_HEAD_ARMOR:
        index = 2;
        break;
    case GLOBAL_OBJECT_TORSO_ARMOR:
        index = 3;
        break;
    case GLOBAL_OBJECT_ARM_ARMOR:
        index = 4;
        break;
    case GLOBAL_OBJECT_LEG_ARMOR:
        index = 5;
        break;
    case GLOBAL_OBJECT_SPIRIT:
        index = 6;
        break;
    case GLOBAL_OBJECT_TOTAL:
        index = 7;
        break;
    default:
        return NULL;
    }

    return &(_all_category_icons[index]);
}

vt_video::StillImage* GlobalMedia::GetStatusIcon(GLOBAL_STATUS type, GLOBAL_INTENSITY intensity)
{
    if((type <= GLOBAL_STATUS_INVALID) || (type >= GLOBAL_STATUS_TOTAL)) {
        PRINT_WARNING << "Invalid type: " << type << std::endl;
        return NULL;
    }

    if((intensity < GLOBAL_INTENSITY_NEUTRAL) || (intensity >= GLOBAL_INTENSITY_TOTAL)) {
        PRINT_WARNING << "Invalid intensity: " << intensity << std::endl;
        return NULL;
    }

    const uint32 IMAGE_ROWS = 5;
    return &(_status_icons[(type * IMAGE_ROWS) + intensity]);
}

void GlobalMedia::PlaySound(const std::string &identifier)
{
    std::map<std::string, vt_audio::SoundDescriptor *>::iterator sound = _sounds.find(identifier);
    if((sound != _sounds.end()) && sound->second)
         sound->second->Play();
}

void GlobalMedia::_LoadSoundFile(const std::string& sound_name, const std::string& filename)
{
    if (filename.empty() || sound_name.empty())
        return;

    _sounds[sound_name] = new vt_audio::SoundDescriptor();
    if(!_sounds[sound_name]->LoadAudio(filename))
        PRINT_WARNING << "Failed to load '" << filename << "' needed by shop mode" << std::endl;
}

} // namespace vt_global
