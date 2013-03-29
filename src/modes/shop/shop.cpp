///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    shop.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for shop mode interface
***
*** This code provides an interface for the user to purchase wares from a
*** merchant. This mode is usually entered from a map after discussing with a
*** shop keeper.
*** ***************************************************************************/

#include "shop.h"

#include "shop_root.h"
#include "shop_buy.h"
#include "shop_sell.h"
#include "shop_trade.h"

#include "utils.h"

#include "engine/audio/audio.h"
#include "engine/video/video.h"
#include "engine/input.h"
#include "engine/system.h"

#include "common/global/global.h"

#include "engine/mode_manager.h"
#include "modes/pause.h"

#include <iostream>

using namespace hoa_utils;
using namespace hoa_audio;
using namespace hoa_video;
using namespace hoa_gui;
using namespace hoa_input;
using namespace hoa_system;
using namespace hoa_global;
using namespace hoa_mode_manager;
using namespace hoa_shop::private_shop;
using namespace hoa_pause;

namespace hoa_shop
{

bool SHOP_DEBUG = false;
// Initialize static class variable
ShopMode *ShopMode::_current_instance = NULL;

namespace private_shop
{

// *****************************************************************************
// ***** ShopMedia class methods
// *****************************************************************************

ShopMedia::ShopMedia()
{
    _all_category_names.push_back(UTranslate("Items"));
    _all_category_names.push_back(UTranslate("Weapons"));
    _all_category_names.push_back(UTranslate("Head Armor"));
    _all_category_names.push_back(UTranslate("Torso Armor"));
    _all_category_names.push_back(UTranslate("Arm Armor"));
    _all_category_names.push_back(UTranslate("Leg Armor"));
    _all_category_names.push_back(UTranslate("Shards"));
    _all_category_names.push_back(UTranslate("Key Items"));
    _all_category_names.push_back(UTranslate("All Wares"));

    // Initialize the character's prites images.
    _InitializeCharacters();
}


void ShopMedia::_InitializeCharacters()
{
    // Grab the sprite frames for all characters in the active party
    std::vector<GlobalCharacter *>* characters = GlobalManager->GetOrderedCharacters();
    for(uint32 i = 0; i < characters->size(); ++i) {
        GlobalCharacter *character = characters->at(i);

        if(!character) {
            _character_sprites.push_back(new AnimatedImage());
            continue;
        }

        // Load a copy of the idle battle character sprite.
        AnimatedImage anim = *character->RetrieveBattleAnimation("idle");
        anim.SetDimensions(anim.GetWidth() / 2, anim.GetHeight() / 2);
        _character_sprites.push_back(anim);
    }
}


ustring *ShopMedia::GetCategoryName(GLOBAL_OBJECT object_type)
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
    case GLOBAL_OBJECT_SHARD:
        index = 6;
        break;
    case GLOBAL_OBJECT_TOTAL:
        index = 7;
        break;
    default:
        return NULL;
    }

    return &(_all_category_names[index]);
}

// *****************************************************************************
// ***** ShopObjectViewer class methods
// *****************************************************************************

ShopObjectViewer::ShopObjectViewer() :
    _view_mode(SHOP_VIEW_MODE_LIST),
    _selected_object(NULL),
    _object_type(SHOP_OBJECT_INVALID),
    _is_weapon(false),
    _map_usable(false),
    _battle_usable(false),
    _target_type_index(0),
    _shard_number(0)
{
    // Initialize all properties of class members that we can
    _object_name.SetStyle(TextStyle("title24"));

    // Position and dimensions for _description_text are set by _SetDescriptionText()
    _description_text.SetTextStyle(TextStyle("text20"));
    _description_text.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _description_text.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _description_text.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _SetDescriptionText(); // Will set the position and dimensions of _description_text

    // Position and dimensions for _hint_text are set by _SetHintText()
    _hint_text.SetTextStyle(TextStyle("text20"));
    _hint_text.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _hint_text.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _hint_text.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _hint_text.SetPosition(25.0f, 340.0f);
    _hint_text.SetDimensions(750.0f, 10.0f);

    // Position and dimensions for _count_text are set by _SetCountText()
    _count_text.SetTextStyle(TextStyle("text28"));
    _count_text.SetDisplayMode(VIDEO_TEXT_INSTANT);
    _count_text.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _count_text.SetTextAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _count_text.SetPosition(550.0f, 22.0f);
    _count_text.SetDimensions(325.0f, 10.0f);

    _field_use_header.SetStyle(TextStyle("text22"));
    _field_use_header.SetText(UTranslate("Field Use:"));
    _battle_use_header.SetStyle(TextStyle("text22"));
    _battle_use_header.SetText(UTranslate("Battle Use:"));
    _target_type_header.SetStyle(TextStyle("text22"));
    _target_type_header.SetText(UTranslate("Target:"));

    _target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_SELF_POINT), TextStyle("text22")));
    _target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_ALLY_POINT), TextStyle("text22")));
    _target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_FOE_POINT), TextStyle("text22")));
    _target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_SELF), TextStyle("text22")));
    _target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_ALLY), TextStyle("text22")));
    _target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_ALLY_EVEN_DEAD), TextStyle("text22")));
    _target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_FOE), TextStyle("text22")));
    _target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_ALL_ALLIES), TextStyle("text22")));
    _target_type_text.push_back(TextImage(GetTargetText(GLOBAL_TARGET_ALL_FOES), TextStyle("text22")));

    _phys_header.SetStyle(TextStyle("text18"));
    _phys_header.SetText(UTranslate("ATK:"));
    _mag_header.SetStyle(TextStyle("text18"));
    _mag_header.SetText(UTranslate("M.ATK:"));

    _phys_rating.SetStyle(TextStyle("text18"));
    _mag_rating.SetStyle(TextStyle("text18"));

    _conditions_title.SetStyle(TextStyle("text22"));
    _conditions_title.SetText(UTranslate("Conditions:"));

    _conditions_name.SetOwner(ShopMode::CurrentInstance()->GetMiddleWindow());
    _conditions_name.SetPosition(400.0f, 140.0f);
    _conditions_name.SetDimensions(600.0f, 120.0f, 1, 255, 1, 4);
    _conditions_name.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _conditions_name.SetTextStyle(TextStyle("text22"));
    _conditions_name.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);
    _conditions_name.SetCursorOffset(-40.0f, -20.0f);
    _conditions_name.SetHorizontalWrapMode(VIDEO_WRAP_MODE_NONE);
    _conditions_name.SetVerticalWrapMode(VIDEO_WRAP_MODE_NONE);

    _conditions_number.SetOwner(ShopMode::CurrentInstance()->GetMiddleWindow());
    _conditions_number.SetPosition(730.0f, 140.0f);
    _conditions_number.SetDimensions(50.0f, 120.0f, 1, 255, 1, 4);
    _conditions_number.SetOptionAlignment(VIDEO_X_LEFT, VIDEO_Y_CENTER);
    _conditions_number.SetTextStyle(TextStyle("text22"));
    _conditions_number.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    _conditions_number.SetHorizontalWrapMode(VIDEO_WRAP_MODE_NONE);
    _conditions_number.SetVerticalWrapMode(VIDEO_WRAP_MODE_NONE);

    GlobalMedia& media = GlobalManager->Media();
    _check_icon = media.GetCheckIcon();
    _x_icon = media.GetXIcon();
    _shard_slot_icon = media.GetShardSlotIcon();
    _equip_icon = media.GetEquipIcon();
    _key_item_icon = media.GetKeyItemIcon();

    _atk_icon = media.GetStatusIcon(GLOBAL_STATUS_STRENGTH_RAISE, GLOBAL_INTENSITY_NEUTRAL);
    _matk_icon = media.GetStatusIcon(GLOBAL_STATUS_VIGOR_RAISE, GLOBAL_INTENSITY_NEUTRAL);
    _def_icon = media.GetStatusIcon(GLOBAL_STATUS_FORTITUDE_RAISE, GLOBAL_INTENSITY_NEUTRAL);
    _mdef_icon = media.GetStatusIcon(GLOBAL_STATUS_PROTECTION_RAISE, GLOBAL_INTENSITY_NEUTRAL);
}


void ShopObjectViewer::Initialize()
{
    // Can change over time.
    _description_text.SetOwner(ShopMode::CurrentInstance()->GetBottomWindow());

    _count_text.SetOwner(ShopMode::CurrentInstance()->GetMiddleWindow());
    _hint_text.SetOwner(ShopMode::CurrentInstance()->GetMiddleWindow());

    std::vector<hoa_video::AnimatedImage>* animations = ShopMode::CurrentInstance()->Media()->GetCharacterSprites();
    uint32 number_character = animations->size();

    for(uint32 i = 0; i < number_character; ++i) {
        _character_sprites.push_back(&animations->at(i));
        _character_equipped.push_back(false);
        _phys_change_text.push_back(TextImage());
        _mag_change_text.push_back(TextImage());
    }
}


void ShopObjectViewer::Update()
{
    // Update active character animations.
    std::vector<hoa_video::AnimatedImage *>::iterator it = _character_sprites.begin();
    for(; it != _character_sprites.end(); ++it) {
        if(!(*it)->IsGrayScale())
            (*it)->Update();
    }

    _description_text.Update();
    _hint_text.Update();
    _count_text.Update();

    if (ShopMode::CurrentInstance()->GetState() == SHOP_STATE_TRADE) {
        _conditions_name.Update();
        _conditions_number.Update();
    }
}



void ShopObjectViewer::Draw()
{
    if(_selected_object == NULL) {
        return;
    }

    // Set the initial draw cursor position to the top left corner of the proper window
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_CENTER, 0);
    if(_view_mode == SHOP_VIEW_MODE_LIST) {
        VideoManager->Move(135.0f, 580.0f);
    } else if(_view_mode == SHOP_VIEW_MODE_INFO) {
        VideoManager->Move(135.0f, 200.0f);
    } else { // An unknown/unsupported view mode is active, so draw nothing
        return;
    }

    // Object's name and icon are drawn in the same position for all objects
    _object_name.Draw();
    VideoManager->MoveRelative(0.0f, 55.0f);
    GlobalObject* object = _selected_object->GetObject();
    object->GetIconImage().Draw();
    if (object->IsKeyItem()) {
        VideoManager->MoveRelative(32.0f, 15.0f);
        _key_item_icon->Draw();
        VideoManager->MoveRelative(-32.0f, -15.0f);
    }

    switch(_object_type) {
    case SHOP_OBJECT_ITEM:
        _DrawItem();
        break;
    case SHOP_OBJECT_EQUIPMENT:
        _DrawEquipment();
        break;
    case SHOP_OBJECT_SHARD:
        _DrawShard();
        break;
    default: // unknown/unsupported object type, draw no further information
        break;
    }

    // In the info view mode, description text and lore text is always drawn near the bottom of the middle window
    if(_view_mode == SHOP_VIEW_MODE_INFO) {
        _description_text.Draw();
        _hint_text.Draw();
        _count_text.Draw();
        if(ShopMode::CurrentInstance()->GetState() == SHOP_STATE_TRADE) {
            VideoManager->Move(580.0f, 275.0f);
            _conditions_title.Draw();

            _conditions_name.Draw();
            _conditions_number.Draw();
        }
    }
}



void ShopObjectViewer::SetSelectedObject(ShopObject *object)
{
    if(object == NULL) {
        _selected_object = NULL;
        return;
    }

    if(_selected_object == object) {
        return;
    }

    _selected_object = object;
    _object_type = _selected_object->DetermineShopObjectType();

    // Get a pointer to the global object type of the new object selection
    switch(_object_type) {
    case SHOP_OBJECT_ITEM:
        _SetItemData();
        break;
    case SHOP_OBJECT_EQUIPMENT:
        _SetEquipmentData();
        break;
    case SHOP_OBJECT_SHARD:
        _SetShardData();
        break;
    default:
        IF_PRINT_WARNING(SHOP_DEBUG) << "invalid object type: " << _object_type << std::endl;
        break;
    }

    _object_name.SetText(_selected_object->GetObject()->GetName());
    _description_text.SetDisplayText(_selected_object->GetObject()->GetDescription());

    _SetHintText();
    UpdateCountText();
} // void ShopObjectViewer::SetSelectedObject(ShopObject* object)



void ShopObjectViewer::ChangeViewMode(SHOP_VIEW_MODE new_mode)
{
    if(_view_mode == new_mode)
        return;

    if(new_mode == SHOP_VIEW_MODE_LIST) {
        _view_mode = new_mode;
    } else if(new_mode == SHOP_VIEW_MODE_INFO) {
        _view_mode = new_mode;
        if(ShopMode::CurrentInstance()->GetState() == SHOP_STATE_TRADE)
            _UpdateTradeConditions();
    } else {
        IF_PRINT_WARNING(SHOP_DEBUG) << "unknown/unsupported view mode passed in function argument: " << new_mode << std::endl;
    }

    _SetDescriptionText(); // Necessary because description text must change its owner window
    _SetHintText();
    UpdateCountText();
}

void ShopObjectViewer::_UpdateTradeConditions()
{
    _conditions_name.ClearOptions();
    _conditions_number.ClearOptions();

    if (!_selected_object || !_selected_object->GetObject())
        return;

    // The option box current index
    uint32 j = 0;
    // The trade conditions
    const std::vector<std::pair<uint32, uint32> >& trade_cond = _selected_object->GetObject()->GetTradeConditions();

    for(uint32 i = 0; i < trade_cond.size(); ++i) {
        uint32 item_id = trade_cond[i].first;
        uint32 item_number = trade_cond[i].second;

        // Create a global object to get info from.
        GlobalObject* obj = GlobalCreateNewObject(item_id, 1);
        if (!obj)
            continue;

        _conditions_name.AddOption(MakeUnicodeString("<" + obj->GetIconImage().GetFilename() + "><30>")
                                 + obj->GetName());
        // Delete it once we're done with it.
        delete obj;

        StillImage *img = _conditions_name.GetEmbeddedImage(j);
        if (img)
            img->SetDimensions(30.0f, 30.0f);

        _conditions_number.AddOption(MakeUnicodeString("x" + NumberToString(item_number)));

        ++j;
    }

    // Hide the cursor if there is no scrolling to do on conditions
    if (_conditions_name.GetNumberOptions() < 5)
        _conditions_name.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    else
        _conditions_name.SetCursorState(VIDEO_CURSOR_STATE_VISIBLE);

    _conditions_name.SetSelection(0);
    _conditions_number.SetSelection(0);
    _conditions_name.ResetViewableOption();
    _conditions_number.ResetViewableOption();
}

void ShopObjectViewer::_SetItemData()
{
    if(_object_type != SHOP_OBJECT_ITEM) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "function invoked when selected object was not an item: " << _object_type << std::endl;
        return;
    }

    // Ensure that the position of description text is correct
    _SetDescriptionText();
    _SetHintText();
    UpdateCountText();

    // Set map/battle usability status
    GlobalItem *item = dynamic_cast<GlobalItem *>(_selected_object->GetObject());
    _map_usable = item->IsUsableInField();
    _battle_usable = item->IsUsableInBattle();

    // Determine the target type text to display for this item
    GLOBAL_TARGET target_type = item->GetTargetType();
    _target_type_index = static_cast<uint32>(target_type);
    if(_target_type_index >= _target_type_text.size()) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "unknown/invalid target type, defaulting to 'Self': " << target_type << std::endl;
        _target_type_index = 0;
    }
}



void ShopObjectViewer::_SetEquipmentData()
{
    if(_object_type != SHOP_OBJECT_EQUIPMENT) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "function invoked when selected object was not a piece of equipment: " << _object_type << std::endl;
        return;
    }

    // ---------- (1): Determine whether the selected object is a weapon or piece of armor
    GlobalWeapon *selected_weapon = NULL;
    GlobalArmor *selected_armor = NULL;
    uint32 usable_status = 0; // This is a bit mask that will hold the selected object's usablility information
    uint32 armor_index = 0; // Will hold the correct index into a GlobalCharacter object's equipped armor container

    if(_selected_object->GetObject()->GetObjectType() == GLOBAL_OBJECT_WEAPON) {
        selected_weapon = dynamic_cast<GlobalWeapon *>(_selected_object->GetObject());
        usable_status = selected_weapon->GetUsableBy();
        _is_weapon = true;
    } else {
        selected_armor = dynamic_cast<GlobalArmor *>(_selected_object->GetObject());
        usable_status = selected_armor->GetUsableBy();
        _is_weapon = false;

        // Armor on GlobalCharacter objects are stored in 4-element vectors. The different armor type maps to one of these four elements
        switch(selected_armor->GetObjectType()) {
        case GLOBAL_OBJECT_HEAD_ARMOR:
            armor_index = 0;
            break;
        case GLOBAL_OBJECT_TORSO_ARMOR:
            armor_index = 1;
            break;
        case GLOBAL_OBJECT_ARM_ARMOR:
            armor_index = 2;
            break;
        case GLOBAL_OBJECT_LEG_ARMOR:
            armor_index = 3;
            break;
        default:
            IF_PRINT_WARNING(SHOP_DEBUG) << "object type was not armor: " << _selected_object->GetObject()->GetObjectType() << std::endl;
            return;
        }
    }

    // ---------- (2): Determine equipment's rating, socket, elemental effects, and status effects to report

    if(selected_weapon) {
        _phys_header.SetText(UTranslate("ATK:"));
        _mag_header.SetText(UTranslate("M.ATK:"));
        _phys_rating.SetText(NumberToString(selected_weapon->GetPhysicalAttack()));
        _mag_rating.SetText(NumberToString(selected_weapon->GetMagicalAttack()));
        _shard_number = selected_weapon->GetShardSlots().size();
        _SetElementalIcons(selected_weapon->GetElementalEffects());
        _SetStatusIcons(selected_weapon->GetStatusEffects());
    } else if(selected_armor) {
        _phys_header.SetText(UTranslate("DEF:"));
        _mag_header.SetText(UTranslate("M.DEF:"));
        _phys_rating.SetText(NumberToString(selected_armor->GetPhysicalDefense()));
        _mag_rating.SetText(NumberToString(selected_armor->GetMagicalDefense()));
        _shard_number = selected_armor->GetShardSlots().size();
        _SetElementalIcons(selected_armor->GetElementalEffects());
        _SetStatusIcons(selected_armor->GetStatusEffects());
    }

    // ---------- (3): For each character, determine if they already have the selection equipped or determine the change in pricing
    std::vector<GlobalCharacter *>* party = GlobalManager->GetOrderedCharacters();
    GlobalCharacter *character = NULL;
    GlobalWeapon *equipped_weapon = NULL;
    GlobalArmor *equipped_armor = NULL;
    int32 phys_diff = 0, mag_diff = 0; // Holds the difference in attack power from equipped weapon/armor to selected weapon/armor

    // NOTE: In this block of code, entries to the _phys_change_text and _mag_change_text members are only modified if that information is to be
    // displayed for the character (meaning that the character can use the weapon/armor and does not already have it equipped). This means
    // that these two container members may contain stale data from previous objects. This is acceptable, however, as the stale data should
    // never be drawn. The stale data is allowed to remain so that we do not waste time re-rendering text for which we will not display.
    if(selected_weapon != NULL) {
        for(uint32 i = 0; i < party->size(); ++i) {
            character = party->at(i);
            equipped_weapon = character->GetWeaponEquipped();

            // Initially assume that the character does not have this weapon equipped
            _character_equipped[i] = false;

            // Case 1: determine if the character can use the weapon and if not, move on to the next character
            // Toggle grayscale mode appropriately to indicate whether or not the character can equip this
            if(usable_status & (character->GetID())) {
                if(_character_sprites[i]->IsGrayScale())
                    _character_sprites[i]->DisableGrayScale();
            } else {
                if(!_character_sprites[i]->IsGrayScale())
                    _character_sprites[i]->EnableGrayScale();
                continue;
            }
            // Case 2: if the player does not have any weapon equipped, the stat diff is equal to the selected weapon's ratings
            if(equipped_weapon == NULL) {
                phys_diff = static_cast<int32>(selected_weapon->GetPhysicalAttack());
                mag_diff = static_cast<int32>(selected_weapon->GetMagicalAttack());
            }
            // Case 3: if the player already has this weapon equipped, indicate thus and move on to the next character
            else if(selected_weapon->GetID() == equipped_weapon->GetID()) {
                _character_equipped[i] = true;
                continue;
            }
            // Case 4: the player can use this weapon and does not already have it equipped
            else {
                phys_diff = static_cast<int32>(selected_weapon->GetPhysicalAttack()) -
                            static_cast<int32>(equipped_weapon->GetPhysicalAttack());
                mag_diff = static_cast<int32>(selected_weapon->GetMagicalAttack()) -
                            static_cast<int32>(equipped_weapon->GetMagicalAttack());
            }

            // If this line has been reached, either case (2) or case (4) were evaluated as true. Render the phys/meta stat variation text
            _SetChangeText(i, phys_diff, mag_diff);
        }
    } else { // (selected_armor != NULL)
        for(uint32 i = 0; i < party->size(); ++i) {
            character = party->at(i);
            equipped_armor = character->GetArmorEquipped().at(armor_index);

            // Initially assume that the character does not have this armor equipped
            _character_equipped[i] = false;

            // Case 1: determine if the character can use the armor and if not, move on to the next character
            // Toggle grayscale mode appropriately to indicate whether or not the character can equip this
            if(usable_status & (character->GetID())) {
                if(_character_sprites[i]->IsGrayScale())
                    _character_sprites[i]->DisableGrayScale();
            } else {
                if(!_character_sprites[i]->IsGrayScale())
                    _character_sprites[i]->EnableGrayScale();
                continue;
            }
            // Case 2: if the player does not have any armor equipped, the stat diff is equal to the selected armor's ratings
            if(equipped_armor == NULL) {
                phys_diff = static_cast<int32>(selected_armor->GetPhysicalDefense());
                mag_diff = static_cast<int32>(selected_armor->GetMagicalDefense());
            }
            // Case 3: if the player already has this armor equipped, indicate thus and move on to the next character
            else if(selected_armor->GetID() == equipped_armor->GetID()) {
                _character_equipped[i] = true;
                continue;
            }
            // Case 4: the player can use this armor and does not already have it equipped
            else {
                phys_diff = static_cast<int32>(selected_armor->GetPhysicalDefense()) -
                            static_cast<int32>(equipped_armor->GetPhysicalDefense());
                mag_diff = static_cast<int32>(selected_armor->GetMagicalDefense()) -
                            static_cast<int32>(equipped_armor->GetMagicalDefense());
            }

            // If this line has been reached, either case (2) or case (4) were evaluated as true. Render the phys/meta stat variation text
            _SetChangeText(i, phys_diff, mag_diff);
        }
    }
} // void ShopObjectViewer::_SetEquipmentData()



void ShopObjectViewer::_SetShardData()
{
    // TODO: implement when GlobalShard class is ready for use
}



void ShopObjectViewer::_SetDescriptionText()
{
    if(_view_mode == SHOP_VIEW_MODE_LIST) {
        _description_text.SetOwner(ShopMode::CurrentInstance()->GetBottomWindow());
        // For key items, draw position is a little higher than other cases to center it in the blank area
        if(_selected_object && _selected_object->GetObject()->IsKeyItem()) {
            _description_text.SetPosition(102.0f, 64.0f);
        } else {
            _description_text.SetPosition(102.0f, 84.0f);
        }
        _description_text.SetDimensions(675.0f, 50.0f);
    }

    else if(_view_mode == SHOP_VIEW_MODE_INFO) {
        _description_text.SetOwner(ShopMode::CurrentInstance()->GetMiddleWindow());
        _description_text.SetPosition(25.0f, 260.0f);
        _description_text.SetDimensions(750.0f, 50.0f);
    }

    else {
        IF_PRINT_WARNING(SHOP_DEBUG) << "unknown/unsupported view mode was active: " << _view_mode << std::endl;
    }
}

void ShopObjectViewer::_SetHintText()
{
    if(_view_mode == SHOP_VIEW_MODE_LIST) {
        _hint_text.SetDisplayText("");
        return;
    }

    const std::string left_key = Translate(InputManager->GetLeftKeyName());
    const std::string right_key = Translate(InputManager->GetRightKeyName());

    switch (ShopMode::CurrentInstance()->GetState()) {
    case SHOP_STATE_BUY:
        _hint_text.SetDisplayText(VTranslate("Press %s to add items to buy and %s to remove items from your purchase.", right_key, left_key));
        break;
    case SHOP_STATE_SELL:
        _hint_text.SetDisplayText(VTranslate("Press %s to add items to sell and %s to remove items from your sale.", right_key, left_key));
        break;
    case SHOP_STATE_TRADE:
        _hint_text.SetDisplayText(VTranslate("Press %s to add items to trade and %s to remove them.", right_key, left_key));
        break;
    default:
        _hint_text.SetDisplayText(""); //Clear the text for everything else
        break;
    }
}

void ShopObjectViewer::UpdateCountText()
{
    if(!_selected_object || _view_mode == SHOP_VIEW_MODE_LIST) {
        _count_text.SetDisplayText("");
        return;
    }

    switch (ShopMode::CurrentInstance()->GetState()) {
    case SHOP_STATE_BUY:
        _count_text.SetDisplayText(Translate("Buy count: x ") + NumberToString(_selected_object->GetBuyCount()));
        break;
    case SHOP_STATE_SELL:
        _count_text.SetDisplayText(Translate("Sell count: x ") + NumberToString(_selected_object->GetSellCount()));
        break;
    case SHOP_STATE_TRADE:
        _count_text.SetDisplayText(Translate("Trade count: x ") + NumberToString(_selected_object->GetTradeCount()));
        break;
    default:
        _count_text.SetDisplayText(""); //Clear the text for everything else
        break;
    }
}

void ShopObjectViewer::ScrollUpTradeConditions()
{
    _conditions_name.InputUp();
    _conditions_number.InputUp();
}

void ShopObjectViewer::ScrollDownTradeConditions()
{
    _conditions_name.InputDown();
    _conditions_number.InputDown();
}

void ShopObjectViewer::_SetChangeText(uint32 index, int32 phys_diff, int32 mag_diff)
{
    if(index >= _character_sprites.size()) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "index argument was out of bounds: " << index << std::endl;
        return;
    }

    _phys_change_text[index].Clear();
    if(phys_diff > 0) {
        _phys_change_text[index].SetStyle(TextStyle("text18", Color::green));
        _phys_change_text[index].SetText("+" + NumberToString(phys_diff));
    } else if(phys_diff < 0) {
        _phys_change_text[index].SetStyle(TextStyle("text18", Color::red));
        _phys_change_text[index].SetText(NumberToString(phys_diff));
    } else { // (phys_diff == 0)
        _phys_change_text[index].SetStyle(TextStyle("text18", Color::white));
        _phys_change_text[index].SetText(NumberToString(phys_diff));
    }

    _mag_change_text[index].Clear();
    if(mag_diff > 0) {
        _mag_change_text[index].SetStyle(TextStyle("text18", Color::green));
        _mag_change_text[index].SetText("+" + NumberToString(mag_diff));
    } else if(mag_diff < 0) {
        _mag_change_text[index].SetStyle(TextStyle("text18", Color::red));
        _mag_change_text[index].SetText(NumberToString(mag_diff));
    } else { // (mag_diff == 0)
        _mag_change_text[index].SetStyle(TextStyle("text18", Color::white));
        _mag_change_text[index].SetText(NumberToString(mag_diff));
    }
}

void ShopObjectViewer::_SetElementalIcons(const std::vector<std::pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY> >& elemental_effects)
{
    _elemental_icons.clear();
    for(std::vector<std::pair<GLOBAL_ELEMENTAL, GLOBAL_INTENSITY> >::const_iterator it = elemental_effects.begin();
            it != elemental_effects.end(); ++it) {
        if(it->second != GLOBAL_INTENSITY_NEUTRAL)
            _elemental_icons.push_back(GlobalManager->Media().GetElementalIcon(it->first, it->second));
    }
}

void ShopObjectViewer::_SetStatusIcons(const std::vector<std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY> >& status_effects)
{
    _status_icons.clear();
    for(std::vector<std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY> >::const_iterator it = status_effects.begin();
            it != status_effects.end(); ++it) {
        if(it->second != GLOBAL_INTENSITY_NEUTRAL)
            _status_icons.push_back(GlobalManager->Media().GetStatusIcon(it->first, it->second));
    }
}

void ShopObjectViewer::_DrawItem()
{
    float move_offset = 0.0f; // Used to save image widths in determining relative movement
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_CENTER, 0);

    VideoManager->MoveRelative(80.0f, -15.0f);
    _field_use_header.Draw();
    move_offset = _field_use_header.GetWidth() + 5.0f; // 5.0f is a small buffer space between text and graphic
    VideoManager->MoveRelative(move_offset, 0.0f);
    if(_map_usable) {
        _check_icon->Draw();
    } else {
        _x_icon->Draw();
    }

    VideoManager->MoveRelative(175.0f - move_offset, 0.0f);
    _battle_use_header.Draw();
    move_offset = _battle_use_header.GetWidth() + 5.0f;
    VideoManager->MoveRelative(move_offset, 0.0f);
    if(_battle_usable) {
        _check_icon->Draw();
    } else {
        _x_icon->Draw();
    }

    VideoManager->MoveRelative(175.0f - move_offset, 0.0f);
    _target_type_header.Draw();
    move_offset = _target_type_header.GetWidth() + 5.0f;
    VideoManager->MoveRelative(move_offset, 0.0f);
    _target_type_text[_target_type_index].Draw();

    _description_text.Draw();
    _hint_text.Draw();
    _count_text.Draw();
}



void ShopObjectViewer::_DrawEquipment()
{
    VideoManager->MoveRelative(70.0f, -15.0f);
    if (_is_weapon)
        _atk_icon->Draw();
    else
        _def_icon->Draw();
    VideoManager->MoveRelative(25.0f, 0.0f);
    _phys_header.Draw();

    VideoManager->MoveRelative(-25.0f, 30.0f);
    if (_is_weapon)
        _matk_icon->Draw();
    else
        _mdef_icon->Draw();
    VideoManager->MoveRelative(25.0f, 0.0f);
    _mag_header.Draw();

    VideoManager->SetDrawFlags(VIDEO_X_RIGHT, 0);
    VideoManager->MoveRelative(110.0f, -30.0f);
    _phys_rating.Draw();
    VideoManager->MoveRelative(0.0f, 30.0f);
    _mag_rating.Draw();

    VideoManager->SetDrawFlags(VIDEO_X_LEFT, 0);
    VideoManager->MoveRelative(20.0f, 0.0f);
    for (uint32 i = 0; i < _shard_number; ++i) {
        _shard_slot_icon->Draw();
        VideoManager->MoveRelative(15.0f , 0.0f);
    }
    VideoManager->MoveRelative(-15.0f * (_shard_number > 5.0f ? 5.0f : _shard_number) , -35.0f);

    // Draw elemental effect icons
    uint32 element_size = _elemental_icons.size();
    VideoManager->MoveRelative((18.0f * element_size) - 12.0f, 0.0f);
    for(uint32 i = 0; i < element_size; ++i) {
        _elemental_icons[i]->Draw();
        VideoManager->MoveRelative(-18.0f, 0.0f);
    }
    VideoManager->MoveRelative(0.0f, -25.0f);

    // Draw status effects icons
    element_size = _status_icons.size() > 9 ? 9 : _status_icons.size();
    VideoManager->MoveRelative((18.0f * element_size), 0.0f);
    for(uint32 i = 0; i < element_size; ++i) {
        _status_icons[i]->Draw();
        VideoManager->MoveRelative(-18.0f, 0.0f);
    }

    if(_view_mode == SHOP_VIEW_MODE_LIST) {
        // In list view mode, draw the sprites to the right of the icons
        VideoManager->MoveRelative(210.0f, -20.0f);
    }
    else if(ShopMode::CurrentInstance()->GetState() == SHOP_STATE_TRADE) {
        // In info view mode, draw on the left side
        VideoManager->Move(170.0f, 295.0f);
    }
    else {
        // In info view mode, draw the sprites centered on the screen
        // in a row below the other equipment data
        VideoManager->Move(512.0f, 295.0f);
        float x_offset = -20.0f * _character_sprites.size();
        VideoManager->MoveRelative(x_offset, 0.0f);
    }

    VideoManager->MoveRelative(-45.0f, 78.0f);
    if (_is_weapon)
        _atk_icon->Draw();
    else
        _def_icon->Draw();
    VideoManager->MoveRelative(0.0f, 18.0f);
    if (_is_weapon)
        _matk_icon->Draw();
    else
        _mdef_icon->Draw();
    VideoManager->MoveRelative(45.0f, -96.0f);

    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_TOP, 0);

    uint32 max_characters = _character_sprites.size();
    // In list mode, there's only enough room to show 8 sprites
    if ((_view_mode == SHOP_VIEW_MODE_LIST) && (max_characters > 8))
        max_characters = 8;

    for(uint32 i = 0; i < max_characters; ++i) {
        _character_sprites[i]->Draw();

        // Case 1: Draw the equip icon below the character sprite
        if(_character_equipped[i]) {
            VideoManager->MoveRelative(0.0f, 78.0f);
            _equip_icon->Draw();
            VideoManager->MoveRelative(0.0f, -78.0f);
        }
        // Case 2: Draw the phys/mag change text below the sprite
        else if(!_character_sprites[i]->IsGrayScale()) {
            VideoManager->MoveRelative(0.0f, 65.0f);
            _phys_change_text[i].Draw();
            VideoManager->MoveRelative(0.0f, 20.0f);
            _mag_change_text[i].Draw();
            VideoManager->MoveRelative(0.0f, -85.0f);
        }
        // Case 3: Nothing needs to be drawn below the sprite
        VideoManager->MoveRelative(40.0f, 0.0f);
    }
} // void ShopObjectViewer::_DrawEquipment()

void ShopObjectViewer::_DrawShard()
{
    // TODO: implement when GlobalShard class is ready for use
}

} // namespace private_shop

// *****************************************************************************
// ***** ShopMode class methods
// *****************************************************************************

ShopMode::ShopMode() :
    _initialized(false),
    _state(SHOP_STATE_ROOT),
    _buy_price_level(SHOP_PRICE_STANDARD),
    _sell_price_level(SHOP_PRICE_STANDARD),
    _total_costs(0),
    _total_sales(0),
    _total_change_amount(0),
    _shop_media(NULL),
    _object_viewer(NULL),
    _root_interface(NULL),
    _buy_interface(NULL),
    _sell_interface(NULL),
    _trade_interface(NULL)
{
    mode_type = MODE_MANAGER_SHOP_MODE;
    _current_instance = this;

    // Create the menu windows and set their properties
    _top_window.Create(800.0f, 96.0f, ~VIDEO_MENU_EDGE_BOTTOM);
    _top_window.SetPosition(112.0f, 84.0f);
    _top_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _top_window.SetDisplayMode(VIDEO_MENU_INSTANT);
    _top_window.Show();

    _middle_window.Create(800.0f, 400.0f, VIDEO_MENU_EDGE_ALL, VIDEO_MENU_EDGE_TOP | VIDEO_MENU_EDGE_BOTTOM);
    _middle_window.SetPosition(112.0f, 164.0f);
    _middle_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _middle_window.SetDisplayMode(VIDEO_MENU_INSTANT);
    _middle_window.Show();

    _bottom_window.Create(800.0f, 140.0f, ~VIDEO_MENU_EDGE_TOP);
    _bottom_window.SetPosition(112.0f, 544.0f);
    _bottom_window.SetAlignment(VIDEO_X_LEFT, VIDEO_Y_TOP);
    _bottom_window.SetDisplayMode(VIDEO_MENU_INSTANT);
    _bottom_window.Show();

    // Create the list of shop actions
    _action_options.SetOwner(&_top_window);
    _action_options.SetPosition(80.0f, 6.0f);
    _action_options.SetDimensions(640.0f, 30.0f, 3, 1, 3, 1);
    _action_options.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_TOP);
    _action_options.SetTextStyle(TextStyle("title28"));
    _action_options.SetSelectMode(VIDEO_SELECT_SINGLE);
    _action_options.SetCursorOffset(-55.0f, -30.0f);
    _action_options.SetVerticalWrapMode(VIDEO_WRAP_MODE_STRAIGHT);

    std::vector<ustring> option_text;
    option_text.push_back(UTranslate("Buy"));
    option_text.push_back(UTranslate("Sell"));
    option_text.push_back(UTranslate("Trade"));
    _action_options.SetOptions(option_text);
    _action_options.SetSelection(0);
    _action_options.SetSkipDisabled(true);

    _action_titles.push_back(TextImage(option_text[0], TextStyle("title28")));
    _action_titles.push_back(TextImage(option_text[1], TextStyle("title28")));
    _action_titles.push_back(TextImage(option_text[2], TextStyle("title28")));

    // (3) Create the financial table text
    _finance_table.SetOwner(&_top_window);
    _finance_table.SetPosition(80.0f, 51.0f);
    _finance_table.SetDimensions(640.0f, 20.0f, 3, 1, 3, 1);
    _finance_table.SetOptionAlignment(VIDEO_X_CENTER, VIDEO_Y_CENTER);
    _finance_table.SetTextStyle(TextStyle("text22"));
    _finance_table.SetCursorState(VIDEO_CURSOR_STATE_HIDDEN);
    // Initialize all three options with an empty string that will be overwritten by the following method call
    for(uint32 i = 0; i < 3; i++)
        _finance_table.AddOption(ustring());
    UpdateFinances(0);

    _shop_media = new ShopMedia();
    _object_viewer = new ShopObjectViewer();
    _root_interface = new RootInterface();
    _buy_interface = new BuyInterface();
    _sell_interface = new SellInterface();
    _trade_interface = new TradeInterface();

    try {
        _screen_backdrop = VideoManager->CaptureScreen();
    } catch(const Exception& e) {
        IF_PRINT_WARNING(SHOP_DEBUG) << e.ToString() << std::endl;
    }
} // ShopMode::ShopMode()



ShopMode::~ShopMode()
{
    delete _shop_media;
    delete _object_viewer;
    delete _root_interface;
    delete _buy_interface;
    delete _sell_interface;
    delete _trade_interface;

    _top_window.Destroy();
    _middle_window.Destroy();
    _bottom_window.Destroy();

    if(_current_instance == this) {
        _current_instance = NULL;
    }
}



void ShopMode::Reset()
{
    VideoManager->SetStandardCoordSys();
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, 0);

    _current_instance = this;
    if(IsInitialized() == false)
        Initialize();
}



void ShopMode::Initialize()
{
    if(IsInitialized() == true) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "shop was already initialized previously" << std::endl;
        return;
    }

    _initialized = true;

    // Add objects from the player's inventory to the list of available to sell
    _UpdateAvailableObjectsToSell();
    _UpdateAvailableShopOptions();

    // Initialize pricing for all buy shop objects
    for(std::map<uint32, ShopObject *>::iterator it = _available_buy.begin(); it != _available_buy.end(); ++it) {
        it->second->SetPricing(_buy_price_level, _sell_price_level);
    }
    _object_viewer->Initialize();

    // Initialize all shop interfaces
    _root_interface->Reinitialize();
    _buy_interface->Reinitialize();
    _sell_interface->Reinitialize();
    _trade_interface->Reinitialize();
} // void ShopMode::Initialize()


void ShopMode::_UpdateAvailableObjectsToSell()
{
    // Reinit the data
    _available_sell.clear();

    std::map<uint32, GlobalObject *>* inventory = GlobalManager->GetInventory();
    for(std::map<uint32, GlobalObject *>::iterator it = inventory->begin(); it != inventory->end(); ++it) {
        // Don't consider 0 worth objects.
        if(it->second->GetPrice() == 0)
            continue;

        // Don't show key items either.
        if(it->second->IsKeyItem())
            continue;

        // Check if the object already exists in the shop list and if so, set its ownership count
        std::map<uint32, ShopObject *>::iterator shop_obj_iter = _available_sell.find(it->second->GetID());
        if(shop_obj_iter != _available_sell.end()) {
            shop_obj_iter->second->IncrementOwnCount(it->second->GetCount());
        }
        // Otherwise, add the shop object to the list
        else {
            ShopObject *new_shop_object = new ShopObject(it->second);
            new_shop_object->IncrementOwnCount(it->second->GetCount());
            new_shop_object->SetPricing(GetBuyPriceLevel(), GetSellPriceLevel());
            _available_sell.insert(std::make_pair(it->second->GetID(), new_shop_object));
        }
    }
}


void ShopMode::_UpdateAvailableShopOptions()
{

    // Test the available categories
    //Switch back to buy
    if(_available_buy.size() > 0)
        _action_options.EnableOption(0, true);
    else
        _action_options.EnableOption(0, false);

    if(_available_sell.size() > 0)
        _action_options.EnableOption(1, true);
    else
        _action_options.EnableOption(1, false);

    if(_available_trade.size() > 0)
        _action_options.EnableOption(2, true);
    else {
        _action_options.EnableOption(2, false);
    }

    if(!_action_options.IsOptionEnabled(0) && !_action_options.IsOptionEnabled(1) && !_action_options.IsOptionEnabled(2)) {
        // Put the cursor on leave.
        _action_options.SetDimensions(640.0f, 30.0f, 4, 1, 4, 1);
        std::vector<ustring> option_text;
        option_text.push_back(UTranslate("Buy"));
        option_text.push_back(UTranslate("Sell"));
        option_text.push_back(UTranslate("Trade"));
        option_text.push_back(UTranslate("Leave"));
        _action_options.SetOptions(option_text);
        _action_options.SetSelection(3);
        _action_options.SetSkipDisabled(true);

        _action_titles.push_back(TextImage(option_text[0], TextStyle("title28")));
        _action_titles.push_back(TextImage(option_text[1], TextStyle("title28")));
        _action_titles.push_back(TextImage(option_text[2], TextStyle("title28")));
        _action_titles.push_back(TextImage(option_text[3], TextStyle("title28")));

        _action_options.EnableOption(0, false);
        _action_options.EnableOption(1, false);
        _action_options.EnableOption(2, false);
        _action_options.EnableOption(3, true);
    } else if(!_action_options.IsOptionEnabled(0) && !_action_options.IsOptionEnabled(1)) {
        // Put the cursor on trade.
        _action_options.SetSelection(2);
    } else if(!_action_options.IsOptionEnabled(0)) {
        // Put the cursor on sell.
        _action_options.SetSelection(1);
    } else if(!_action_options.IsOptionEnabled(1) || !_action_options.IsOptionEnabled(2)) {
        // Put the cursor on buy.
        _action_options.SetSelection(0);
    }
}


void ShopMode::Update()
{
    // Pause and quit events have highest priority. If either type of event is detected, no other update processing will be done
    if(InputManager->QuitPress() == true) {
        ModeManager->Push(new PauseMode(true));
        return;
    } else if(InputManager->PausePress() == true) {
        ModeManager->Push(new PauseMode(false));
        return;
    }

    // When the state is at the root interface ,ShopMode needs to process user input and possibly change state
    if(_state == SHOP_STATE_ROOT) {
        if(InputManager->ConfirmPress()) {
            if(_action_options.GetSelection() < 0 || _action_options.GetSelection() > 3) {
                IF_PRINT_WARNING(SHOP_DEBUG) << "invalid selection in action window: " << _action_options.GetSelection() << std::endl;
                _action_options.SetSelection(0);
                return;
            }

            _action_options.InputConfirm();
            GlobalManager->Media().PlaySound("confirm");

            if(_action_options.GetSelection() == 0 && _action_options.IsOptionEnabled(0)) {  // Buy
                ChangeState(SHOP_STATE_BUY);
            } else if(_action_options.GetSelection() == 1 && _action_options.IsOptionEnabled(1)) { // Sell
                ChangeState(SHOP_STATE_SELL);
            } else if(_action_options.GetSelection() == 2 && _action_options.IsOptionEnabled(2)) { // Trade
                ChangeState(SHOP_STATE_TRADE);
            } else if(_action_options.GetSelection() == 3) {
                // Leave
                ModeManager->Pop();
            }
        } else if(InputManager->CancelPress()) {
            // Leave shop
            ModeManager->Pop();
        } else if(InputManager->LeftPress()) {
            _action_options.InputLeft();
        } else if(InputManager->RightPress()) {
            _action_options.InputRight();
        }
        _action_options.Update();

        _root_interface->Update();
    } // if (_state == SHOP_STATE_ROOT)
    else {
        // Update the active interface
        switch(_state) {
        case SHOP_STATE_BUY:
            _buy_interface->Update();
            break;
        case SHOP_STATE_SELL:
            _sell_interface->Update();
            break;
        case SHOP_STATE_TRADE:
            _trade_interface->Update();
            break;
        default:
            IF_PRINT_WARNING(SHOP_DEBUG) << "invalid shop state: " << _state << ", reseting to root state" << std::endl;
            _state = SHOP_STATE_ROOT;
            break;
        } // switch (_state)
    }
} // void ShopMode::Update()



void ShopMode::Draw()
{
    // Draw the background image. Set the system coordinates to the size of the window (same as the screen backdrop)
    VideoManager->SetCoordSys(0.0f, static_cast<float>(VideoManager->GetScreenWidth()),
                              static_cast<float>(VideoManager->GetScreenHeight()), 0.0f);
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_TOP, VIDEO_BLEND, 0);
    VideoManager->Move(0.0f, 0.0f);
    _screen_backdrop.Draw();

    // Draw all menu windows
    // Restore the standard shop coordinate system before drawing the shop windows
    VideoManager->SetStandardCoordSys();
    _top_window.Draw();
    _bottom_window.Draw();
    _middle_window.Draw(); // Drawn last because the middle window has the middle upper and lower window borders attached

    // Draw the contents of the top window
    StillImage *drunes_icon = GlobalManager->Media().GetDrunesIcon();
    VideoManager->Move(130.0f, 103.0f);
    drunes_icon->Draw();
    VideoManager->MoveRelative(705.0f, 0.0f);
    drunes_icon->Draw();

    VideoManager->SetDrawFlags(VIDEO_X_CENTER, VIDEO_Y_CENTER, 0);
    VideoManager->Move(512.0f, 111.0f);
    switch(_state) {
    case SHOP_STATE_ROOT:
        _action_options.Draw();
        break;
    case SHOP_STATE_BUY:
        _action_titles[0].Draw();
        break;
    case SHOP_STATE_SELL:
        _action_titles[1].Draw();
        break;
    case SHOP_STATE_TRADE:
        _action_titles[2].Draw();
        break;
    default:
        IF_PRINT_WARNING(SHOP_DEBUG) << "invalid shop state: " << _state << std::endl;
        break;
    }

    // Note that X and Y are centered at that moment, explaining the relative coordinates used below.
    VideoManager->DrawLine(-315.0f, 20.0f, 315.0f, 20.0f, 1.0f, Color::white);

    _finance_table.Draw();

    // ---------- (4): Call the draw function on the active interface to fill the contents of the other two windows
    switch(_state) {
    case SHOP_STATE_ROOT:
        _root_interface->Draw();
        break;
    case SHOP_STATE_BUY:
        _buy_interface->Draw();
        break;
    case SHOP_STATE_SELL:
        _sell_interface->Draw();
        break;
    case SHOP_STATE_TRADE:
        _trade_interface->Draw();
        break;
    default:
        IF_PRINT_WARNING(SHOP_DEBUG) << "invalid shop state: " << _state << std::endl;
        break;
    }
} // void ShopMode::Draw()



void ShopMode::AddObjectToBuyList(ShopObject *object)
{
    if(object == NULL) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "function was passed a NULL argument" << std::endl;
        return;
    }

    if(object->GetBuyCount() == 0) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object to be added had a buy count of zero" << std::endl;
    }

    uint32 object_id = object->GetObject()->GetID();
    std::pair<std::map<uint32, ShopObject *>::iterator, bool> ret_val;
    ret_val = _buy_list.insert(std::make_pair(object_id, object));
    if(ret_val.second == false) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object to be added already existed in buy list" << std::endl;
    }
}



void ShopMode::RemoveObjectFromBuyList(ShopObject *object)
{
    if(object == NULL) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "function was passed a NULL argument" << std::endl;
        return;
    }

    if(object->GetBuyCount() > 0) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object to be removed had a buy count that was non-zero" << std::endl;
    }

    uint32 object_id = object->GetObject()->GetID();
    std::map<uint32, ShopObject *>::iterator object_entry = _buy_list.find(object_id);
    if(object_entry == _buy_list.end()) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object to be removed did not exist on the buy list" << std::endl;
    } else {
        _buy_list.erase(object_entry);
    }
}



void ShopMode::AddObjectToSellList(ShopObject *object)
{
    if(object == NULL) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "function was passed a NULL argument" << std::endl;
        return;
    }

    if(object->GetSellCount() == 0) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object to be added had a sell count of zero" << std::endl;
    }

    uint32 object_id = object->GetObject()->GetID();
    std::pair<std::map<uint32, ShopObject *>::iterator, bool> ret_val;
    ret_val = _sell_list.insert(std::make_pair(object_id, object));
    if(ret_val.second == false) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object to be added already existed in sell list" << std::endl;
    }
}



void ShopMode::RemoveObjectFromSellList(ShopObject *object)
{
    if(object == NULL) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "function was passed a NULL argument" << std::endl;
        return;
    }

    if(object->GetSellCount() > 0) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object to be removed had a sell count that was non-zero" << std::endl;
    }

    uint32 object_id = object->GetObject()->GetID();
    std::map<uint32, ShopObject *>::iterator object_entry = _sell_list.find(object_id);
    if(object_entry == _sell_list.end()) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object to be removed did not exist on the sell list" << std::endl;
    } else {
        _sell_list.erase(object_entry);
    }
}


void ShopMode::AddObjectToTradeList(ShopObject *object)
{
    if(object == NULL) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "function was passed a NULL argument" << std::endl;
        return;
    }

    if(object->GetTradeCount() == 0) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object to be added had a buy count of zero" << std::endl;
    }

    uint32 object_id = object->GetObject()->GetID();
    std::pair<std::map<uint32, ShopObject *>::iterator, bool> ret_val;
    ret_val = _trade_list.insert(std::make_pair(object_id, object));
    if(ret_val.second == false) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object to be added already existed in buy list" << std::endl;
    }
}



void ShopMode::RemoveObjectFromTradeList(ShopObject *object)
{
    if(object == NULL) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "function was passed a NULL argument" << std::endl;
        return;
    }

    if(object->GetTradeCount() > 0) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object to be removed had a buy count that was non-zero" << std::endl;
    }

    uint32 object_id = object->GetObject()->GetID();
    std::map<uint32, ShopObject *>::iterator object_entry = _trade_list.find(object_id);
    if(object_entry == _trade_list.end()) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object to be removed did not exist on the buy list" << std::endl;
    } else {
        _trade_list.erase(object_entry);
    }
}



void ShopMode::ClearOrder()
{
    for(std::map<uint32, ShopObject *>::iterator i = _buy_list.begin(); i != _buy_list.end(); i++)
        i->second->ResetBuyCount();
    for(std::map<uint32, ShopObject *>::iterator i = _sell_list.begin(); i != _sell_list.end(); i++)
        i->second->ResetSellCount();
    for(std::map<uint32, ShopObject *>::iterator i = _trade_list.begin(); i != _trade_list.end(); i++)
        i->second->ResetTradeCount();

    _buy_list.clear();
    _sell_list.clear();
    _trade_list.clear();

    _total_costs = 0;
    _total_sales = 0;
    _total_change_amount = 0;
    UpdateFinances(0);
}



void ShopMode::CompleteTransaction()
{
    uint32 count = 0;
    uint32 id = 0;

    // Add all objects on the buy list to inventory and update shop object status
    for(std::map<uint32, ShopObject *>::iterator it = _buy_list.begin(); it != _buy_list.end(); ++it) {
        count = it->second->GetBuyCount();
        id = it->second->GetObject()->GetID();

        // The player may have reduced the buy count to zero in the confirm interface before completing the transaction
        // We simply ignore any objects on the buy list with this condition
        if(count == 0)
            continue;

        it->second->ResetBuyCount();
        it->second->IncrementOwnCount(count);
        it->second->DecrementStockCount(count);
        GlobalManager->AddToInventory(id, count);

        if(it->second->GetStockCount() == 0) {
            RemoveObjectToBuy(id);
        }
    }
    _buy_list.clear();

    // Remove all objects on the sell list from the inventory and update shop object status
    for(std::map<uint32, ShopObject *>::iterator it = _sell_list.begin(); it != _sell_list.end(); ++it) {
        count = it->second->GetSellCount();
        id = it->second->GetObject()->GetID();

        if(count == 0)
            continue;

        it->second->ResetSellCount();
        it->second->DecrementOwnCount(count);
        GlobalManager->DecrementObjectCount(id, count);

        // When all owned instances of this object have been sold off, the object is automatically removed
        // from the player's inventory. If the object is not sold in the shop, this means it must be removed
        // from all shop object containers as the object data (GlobalObject pointer) is now invalid.
        if(it->second->GetOwnCount() == 0) {
            RemoveObjectToSell(id);
        }
    }
    _sell_list.clear();

    // Remove all objects on the trade list from the inventory and update shop object status
    for(std::map<uint32, ShopObject *>::iterator it = _trade_list.begin(); it != _trade_list.end(); ++it) {
        count = it->second->GetTradeCount();
        id = it->second->GetObject()->GetID();

        if(count == 0)
            continue;

        it->second->ResetTradeCount();
        it->second->IncrementOwnCount(count);
        it->second->DecrementStockCount(count);
        GlobalManager->AddToInventory(id, count);

        //Remove trade condition items from inventory and possibly call RemoveObjectToSell
        for(uint32 i = 0; i < it->second->GetObject()->GetTradeConditions().size(); ++i) {
            GlobalManager->DecrementObjectCount(it->second->GetObject()->GetTradeConditions()[i].first,
                                                it->second->GetObject()->GetTradeConditions()[i].second * count);
        }

        if(it->second->GetStockCount() == 0) {
            RemoveObjectToTrade(id);
        }

    }
    _trade_list.clear();

    // Update the player's drune count by subtracting costs and adding revenue and update the shop's financial display
    if(_total_change_amount >= 0) {
        GlobalManager->AddDrunes(_total_change_amount);
    } else {
        GlobalManager->SubtractDrunes(-_total_change_amount);
    }
    _total_costs = 0;
    _total_sales = 0;
    _total_change_amount = 0;
    UpdateFinances(0);

    // Notify all interfaces that a transaction has just been completed
    _root_interface->TransactionNotification();
    _buy_interface->TransactionNotification();
    _trade_interface->TransactionNotification();

    // Update the available shop options and place the cursor accordingly.
    _UpdateAvailableObjectsToSell();
    _UpdateAvailableShopOptions();

    // Update the sell list last to avoid a corruption when trading.
    _sell_interface->TransactionNotification();

} // void ShopMode::CompleteTransaction()



void ShopMode::UpdateFinances(int32 change_amount)
{
    int32 updated_change_amount = _total_change_amount + change_amount;

    if((static_cast<int32>(GlobalManager->GetDrunes()) + updated_change_amount) < 0) {
        PRINT_WARNING << "updated costs and sales values cause negative balance" << std::endl;
        return;
    }

    _total_change_amount = updated_change_amount;

    _finance_table.SetOptionText(0, UTranslate("Funds: ") + MakeUnicodeString(NumberToString(GlobalManager->GetDrunes())));
    if(_total_change_amount < 0) {
        _finance_table.SetOptionText(1, UTranslate("Purchases: ") + MakeUnicodeString(NumberToString(_total_change_amount)));
        _finance_table.SetOptionText(2, UTranslate("Total: ") + MakeUnicodeString(NumberToString(GetTotalRemaining())));
    } else if(_total_change_amount > 0) {
        _finance_table.SetOptionText(1, UTranslate("Sales: +") + MakeUnicodeString(NumberToString(_total_change_amount)));
        _finance_table.SetOptionText(2, UTranslate("Total: ") + MakeUnicodeString(NumberToString(GetTotalRemaining())));
    } else {
        _finance_table.SetOptionText(1, hoa_utils::ustring());
        _finance_table.SetOptionText(2, hoa_utils::ustring());
    }
}



void ShopMode::ChangeState(SHOP_STATE new_state)
{
    if(_state == new_state) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "shop was already in the state to change to: " << _state << std::endl;
        return;
    }

    _state = new_state;

    switch(_state) {
    case SHOP_STATE_ROOT:
        _root_interface->MakeActive();
        break;
    case SHOP_STATE_BUY:
        _buy_interface->MakeActive();
        break;
    case SHOP_STATE_SELL:
        _sell_interface->MakeActive();
        break;
    case SHOP_STATE_TRADE:
        _trade_interface->MakeActive();
        break;
    default:
        IF_PRINT_WARNING(SHOP_DEBUG) << "invalid shop state: " << _state << std::endl;
        break;
    }
}



void ShopMode::SetShopName(const ustring& name)
{
    if(IsInitialized() == true) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "function called after shop was already initialized" << std::endl;
        return;
    }

    _root_interface->SetShopName(name);
}



void ShopMode::SetGreetingText(const ustring& greeting)
{
    if(IsInitialized() == true) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "function called after shop was already initialized" << std::endl;
        return;
    }

    _root_interface->SetGreetingText(greeting);
}



void ShopMode::SetPriceLevels(SHOP_PRICE_LEVEL buy_level, SHOP_PRICE_LEVEL sell_level)
{
    if(IsInitialized() == true) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "function called after shop was already initialized" << std::endl;
        return;
    }

    _buy_price_level = buy_level;
    _sell_price_level = sell_level;
}



void ShopMode::AddObject(uint32 object_id, uint32 stock)
{
    if(IsInitialized() == true) {
        PRINT_WARNING << "function called after shop was already initialized" << std::endl;
        return;
    }

    if(object_id == private_global::OBJECT_ID_INVALID || object_id >= private_global::OBJECT_ID_EXCEEDS) {
        PRINT_WARNING << "attempted to add object with invalid id: " << object_id << std::endl;
        return;
    }

    if(_available_buy.find(object_id) != _available_buy.end()) {
        PRINT_WARNING << "attempted to add object that already existed: " << object_id << std::endl;
        return;
    }

    GlobalObject *new_object = GlobalCreateNewObject(object_id, 1);
    if(new_object != NULL) {
        ShopObject *new_shop_object = new ShopObject(new_object);
        new_shop_object->IncrementStockCount(stock);
        _available_buy.insert(std::make_pair(object_id, new_shop_object));
    }
}



void ShopMode::AddTrade(uint32 object_id, uint32 stock)
{
    if(IsInitialized() == true) {
        PRINT_WARNING << "function called after shop was already initialized" << std::endl;
        return;
    }

    if(object_id == private_global::OBJECT_ID_INVALID || object_id >= private_global::OBJECT_ID_EXCEEDS) {
        PRINT_WARNING << "attempted to add object with invalid id: " << object_id << std::endl;
        return;
    }

    if(_available_trade.find(object_id) != _available_trade.end()) {
        PRINT_WARNING << "attempted to add object that already existed: " << object_id << std::endl;
        return;
    }

    GlobalObject *new_object = GlobalCreateNewObject(object_id, 1);
    if(new_object != NULL) {
        ShopObject *new_shop_object = new ShopObject(new_object);
        new_shop_object->IncrementStockCount(stock);
        _available_trade.insert(std::make_pair(object_id, new_shop_object));
    }
}


void ShopMode::RemoveObjectToBuy(uint32 object_id)
{
    std::map<uint32, ShopObject *>::iterator shop_iter = _available_buy.find(object_id);
    if(shop_iter == _available_buy.end()) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "attempted to remove object that did not exist: " << object_id << std::endl;
        return;
    }

    if(shop_iter->second->GetStockCount() != 0) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object's ownership count was non-zero: " << object_id << std::endl;
        return;
    }

    _available_buy.erase(shop_iter);
}

void ShopMode::RemoveObjectToSell(uint32 object_id)
{
    std::map<uint32, ShopObject *>::iterator shop_iter = _available_sell.find(object_id);
    if(shop_iter == _available_sell.end()) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "attempted to remove object that did not exist: " << object_id << std::endl;
        return;
    }

    if(shop_iter->second->GetOwnCount() != 0) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object's ownership count was non-zero: " << object_id << std::endl;
        return;
    }

    _available_sell.erase(shop_iter);
}

void ShopMode::RemoveObjectToTrade(uint32 object_id)
{
    std::map<uint32, ShopObject *>::iterator shop_iter = _available_trade.find(object_id);
    if(shop_iter == _available_trade.end()) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "attempted to remove object that did not exist: " << object_id << std::endl;
        return;
    }

    if(shop_iter->second->GetStockCount() != 0) {
        IF_PRINT_WARNING(SHOP_DEBUG) << "object's ownership count was non-zero: " << object_id << std::endl;
        return;
    }

    _available_trade.erase(shop_iter);
}

} // namespace hoa_shop
