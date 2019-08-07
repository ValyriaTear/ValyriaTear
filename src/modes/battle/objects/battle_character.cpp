////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "battle_character.h"

#include "modes/battle/battle.h"
#include "modes/battle/actions/item_action.h"
#include "modes/battle/status_effects/status_effects_supervisor.h"
#include "modes/battle/command/command_supervisor.h"

#include "common/global/global.h"
#include "common/global/actors/global_character.h"
#include "common/global/objects/global_weapon.h"

#include "engine/video/text.h"

#include "utils/utils_random.h"
#include "utils/utils_strings.h"

using namespace vt_global;
using namespace vt_script;
using namespace vt_system;
using namespace vt_utils;
using namespace vt_video;

namespace vt_battle
{

namespace private_battle
{

BattleCharacter::BattleCharacter(GlobalCharacter *character) :
    BattleActor(character),
    _global_character(character),
    _last_rendered_hp(0),
    _last_rendered_sp(0),
    _sprite_animation_alias("idle")
{
    _last_rendered_hp = GetHitPoints();
    _last_rendered_sp = GetSkillPoints();

    _name_text.SetStyle(TextStyle("title22"));
    _name_text.SetText(GetName());
    _hit_points_text.SetStyle(TextStyle("text24", VIDEO_TEXT_SHADOW_BLACK));
    _hit_points_text.SetText(NumberToString(_last_rendered_hp));
    _skill_points_text.SetStyle(TextStyle("text24", VIDEO_TEXT_SHADOW_BLACK));
    _skill_points_text.SetText(NumberToString(_last_rendered_sp));

    _action_selection_text.SetStyle(TextStyle("text20"));
    _action_selection_text.SetText("");

    // Init the battle animation pointers
    _current_sprite_animation = _global_character->RetrieveBattleAnimation(_sprite_animation_alias);
    // Add custom weapon animation
    std::string weapon_animation;
    if (_global_character->GetEquippedWeapon())
            weapon_animation = _global_character->GetEquippedWeapon()->GetWeaponAnimationFile(_global_character->GetID(), _sprite_animation_alias);
    if (weapon_animation.empty() || !_current_weapon_animation.LoadFromAnimationScript(weapon_animation))
        _current_weapon_animation.Clear();

    // Load the potential the ammo image filename
    _ammo_animation_file = _global_character->GetEquippedWeapon() ?
                               _global_character->GetEquippedWeapon()->GetAmmoAnimationFile() : std::string();

    // Apply passive status effect from equipment
    const std::vector<GLOBAL_INTENSITY>& passive_effects = _global_character->GetEquipementStatusEffects();
    for (uint32_t i = 0; i < passive_effects.size(); ++i) {
        GLOBAL_INTENSITY intensity = passive_effects.at(i);

        if (intensity == GLOBAL_INTENSITY_NEUTRAL)
            continue;

        GLOBAL_STATUS status_effect = (GLOBAL_STATUS) i;
        _effects_supervisor->AddPassiveStatusEffect(status_effect, intensity);
    }

    // Apply currently active status effects
    const std::vector<ActiveStatusEffect>& active_effects = character->GetActiveStatusEffects();
    for(std::vector<ActiveStatusEffect>::const_iterator it = active_effects.begin();
            it != active_effects.end(); ++it) {
        const ActiveStatusEffect& effect = (*it);
        _effects_supervisor->ChangeActiveStatusEffect(effect.GetEffect(), effect.GetIntensity(),
                                                      effect.GetEffectTime(), effect.GetElapsedTime());
    }
}

BattleCharacter::~BattleCharacter()
{
    // If the character finished the battle alive, we set the active
    // status effects on its global alter ego.
    if (IsAlive())
        _effects_supervisor->SetActiveStatusEffects(_global_character);
    else // Otherwise, we just reset those.
        _global_character->ResetActiveStatusEffects();
}

void BattleCharacter::ChangeState(ACTOR_STATE new_state)
{
    BattleActor::ChangeState(new_state);

    switch(_state) {
    case ACTOR_STATE_COMMAND:
        // The battle action should pause whenever a character enters the command state in the WAIT battle type
        BattleMode::CurrentInstance()->SetActorStatePaused(true);
        break;
    case ACTOR_STATE_WARM_UP: {
        // BattleActor::Update() changes to the warm up state if the actor has an action set when the idle time is expired. However for characters, we do not
        // want to proceed forward in this case if the player is currently setting a different action for that same character. Instead we place the character
        // in the command state and wait until the player exits the command menu before moving on to the warm up state.
        if(BattleMode::CurrentInstance()->GetCommandSupervisor()->GetCommandCharacter() == this)
            ChangeState(ACTOR_STATE_COMMAND);

        std::string animation_name = _action->GetWarmupActionName();
        // Set the default animation name when it is empty.
        if (animation_name.empty()) {
            if(GetHitPoints() < (GetMaxHitPoints() / 4))
                animation_name = "poor";
            else
                animation_name = "idle";
        }

        ChangeSpriteAnimation(animation_name);
        break;
    }
    case ACTOR_STATE_SHOWNOTICE:
        if (_action && _action->ShouldShowSkillNotice()) {
            _state_timer.Initialize(1000);
            _state_timer.Run();

            // Determine the current weapon icon if existing...
            std::string icon_filename;
            if (_action->GetIconFilename() == "weapon") {
                std::shared_ptr<GlobalWeapon> wpn = _global_character->GetEquippedWeapon();
                if (wpn) {
                    icon_filename = _global_character->GetEquippedWeapon()->GetIconImage().GetFilename();
                    if (icon_filename.empty())
                        icon_filename = "data/gui/battle/default_weapon.png";
                }
                else {
                    icon_filename = "data/inventory/weapons/fist-human.png";
                }
            }
            else {
                icon_filename = _action->GetIconFilename();
            }
            BattleMode::CurrentInstance()->GetIndicatorSupervisor().AddShortNotice(_action->GetName(),
                                                                                   icon_filename,
                                                                                   _state_timer.GetDuration());
        }
        else {
            _state = ACTOR_STATE_NOTICEDONE;
        }
        break;
    case ACTOR_STATE_ACTING: {
        _action->Initialize();
        if(_action->IsScripted())
            return;

        // Trigger the action animation
        std::string animation_name = _action->GetActionName().empty() ? "idle" : _action->GetActionName();
        ChangeSpriteAnimation(animation_name);
        // Reset state timer
        _state_timer.Initialize(_current_sprite_animation->GetAnimationLength());
        _state_timer.Run();
        break;
    }
    case ACTOR_STATE_DYING:
        // Cancel possible previous actions in progress.
        if(_action)
            _action->Cancel();

        // use the default death sequence when there is no scripts.
        if (!_death_init.is_valid()) {
            ChangeSpriteAnimation("dying");
            _state_timer.Initialize(_current_sprite_animation->GetAnimationLength());
            _state_timer.Run();
        }
        break;
    case ACTOR_STATE_DEAD:
        ChangeSpriteAnimation("dead");
        break;
    case ACTOR_STATE_REVIVE:
        ChangeSpriteAnimation("revive");
        _state_timer.Initialize(_current_sprite_animation->GetAnimationLength());
        _state_timer.Run();
        break;
    default:
        break;
    }

    // The action/target text for the character is always updated when the character's state changes. Technically we do not need to update
    // this text display for every possible state change, but we do it anyway just to be safe and to not add unnecessary code complexity.
    ChangeActionText();
}

void BattleCharacter::Update()
{
    BattleActor::Update();

    _animation_timer.Update();

    // Update the active sprite animation
    _current_sprite_animation->Update();
    _current_weapon_animation.Update();

    // Update hit and skill points after drawing to reduce GPU stall.
    if(_last_rendered_hp != GetHitPoints()) {
        _last_rendered_hp = GetHitPoints();
        _hit_points_text.SetText(NumberToString(_last_rendered_hp));
    }
    if(_last_rendered_sp != GetSkillPoints()) {
        _last_rendered_sp = GetSkillPoints();
        _skill_points_text.SetText(NumberToString(_last_rendered_sp));
    }

    BattleMode* BM = BattleMode::CurrentInstance();

    // Avoid updating the battle logic when finishing.
    // This might break the character's animation.
    switch (BM->GetState()) {
    default:
        break;
    case BATTLE_STATE_VICTORY:
    case BATTLE_STATE_DEFEAT:
    case BATTLE_STATE_EXITING:
        return;
    }

    // In scene mode, only the animations are updated
    if (BM->IsInSceneMode())
        return;

    // Update potential scripted Battle action without hardcoded logic in that case
    if(_action && _action->IsScripted() && _state == ACTOR_STATE_ACTING) {
        if(!_action->Update())
            return;
        else
            ChangeState(ACTOR_STATE_COOL_DOWN);
    }

    // Only set the origin when actor are in normal battle mode,
    // Otherwise the battle sequence manager will take care of them.
    if(BM->GetState() == BATTLE_STATE_NORMAL) {
        _location = _origin;
    }

    if(_sprite_animation_alias == "idle") {
        // Check whether character HP are low
        if (_is_stunned || GetHitPoints() < (GetMaxHitPoints() / 4))
            ChangeSpriteAnimation("poor");
    } else if(_sprite_animation_alias == "run") {
        // no need to do anything
    } else if(_sprite_animation_alias == "run_after_victory") {
        // Returns now as the battle is ending to prevent the animation
        // timer to reset a potential previous battle animation
        // if finishing while the heroes are running.
        return;
    } else if(_sprite_animation_alias == "dying") {
        // no need to do anything, the change state will handle it
    } else if(_sprite_animation_alias == "poor") {
        // Check whether character HP are not low anymore
        if (!_is_stunned && GetHitPoints() > (GetMaxHitPoints() / 4))
            ChangeSpriteAnimation("idle");
    } else if(_sprite_animation_alias == "dead") {
        // no need to do anything
    } else if(_sprite_animation_alias == "revive") {
        // no need to do anything
    } else if(_sprite_animation_alias == "victory") {
        // no need to do anything
    } else if(_sprite_animation_alias == "magic_prepare") {
        // no need to do anything
    }
    // Makes the action listed below be set back to idle once done.
    else if(_animation_timer.IsFinished()) {
        if(_sprite_animation_alias == "hurt" || _sprite_animation_alias == "dodge")
            ChangeSpriteAnimation(_before_attack_sprite_animation);
        else
            ChangeSpriteAnimation("idle");
    } else if(_sprite_animation_alias == "attack") {
        uint32_t dist = _state_timer.GetDuration() > 0 ?
                      120 * _state_timer.GetTimeExpired() / _state_timer.GetDuration() :
                      0;
        _location.x = _origin.x + dist;
    } else if(_sprite_animation_alias == "dodge") {
        _location.x = _origin.x - 20.0f;
    }

    // Add a shake effect when the battle actor has received damages
    if(_hurt_timer.IsRunning())
        _location.x = _origin.x + RandomFloat(-6.0f, 6.0f);

    // If the character has finished to execute its battle action,
    if(_state == ACTOR_STATE_ACTING && _state_timer.IsFinished()) {
        // Triggers here the skill or item action
        // and set the actor to cool down mode.
        if(!_action->Execute())
            // Indicate the the skill execution failed to the user.
            RegisterMiss();

        // If it was an item action, show the item used.
        if(_action->IsItemAction()) {
            ItemAction* item_action = static_cast<ItemAction *>(_action);

            // Creates an item indicator
            float y_pos = GetYLocation() - GetSpriteHeight();
            vt_mode_manager::IndicatorSupervisor& indicator = BM->GetIndicatorSupervisor();
            indicator.AddItemIndicator(GetXLocation(), y_pos, item_action->GetBattleItem()->GetGlobalItem());
        }

        ChangeState(ACTOR_STATE_COOL_DOWN);
    }
}

void BattleCharacter::DrawSprite()
{
    VideoManager->Move(_location.x, _location.y);
    _current_sprite_animation->Draw(Color(1.0f, 1.0f, 1.0f, _sprite_alpha));
    _current_weapon_animation.Draw(Color(1.0f, 1.0f, 1.0f, _sprite_alpha));

    BattleMode *BM = BattleMode::CurrentInstance();

    //! Don't display effects on characters when the battle is over
    if (BM->GetState() != BATTLE_STATE_NORMAL && BM->GetState() != BATTLE_STATE_COMMAND)
        return;

    if(_state == ACTOR_STATE_DYING) {
        try {
            if (_death_draw_on_sprite.is_valid())
                luabind::call_function<void>(_death_draw_on_sprite);
        } catch(const luabind::error &e) {
            PRINT_ERROR << "Error while triggering DrawOnSprite() function of actor id: " << _global_actor->GetID() << std::endl;
            ScriptManager->HandleLuaError(e);
        } catch(const luabind::cast_failed &e) {
            PRINT_ERROR << "Error while triggering DrawOnSprite() function of actor id: " << _global_actor->GetID() << std::endl;
            ScriptManager->HandleCastError(e);
        }

    }
    else if(_is_stunned && (_state == ACTOR_STATE_COMMAND || _state == ACTOR_STATE_IDLE ||
                       _state == ACTOR_STATE_WARM_UP || _state == ACTOR_STATE_COOL_DOWN)) {
        VideoManager->MoveRelative(0, -GetSpriteHeight());
        GlobalManager->GetBattleMedia().GetStunnedIcon().Draw();
    }
}

void BattleCharacter::ChangeSpriteAnimation(const std::string &alias)
{
    // Retains the previous animation when being hurt or dodging.
    if((alias == "hurt" || alias == "dodge")
            && (_sprite_animation_alias != "hurt" && _sprite_animation_alias != "dodge"))
        _before_attack_sprite_animation = _sprite_animation_alias;

    _sprite_animation_alias = alias;
    _current_sprite_animation = _global_character->RetrieveBattleAnimation(_sprite_animation_alias);

    // Change the weapon animation as well
    // Add custom weapon animation
    std::string weapon_animation;
    if (_global_character->GetEquippedWeapon())
            weapon_animation = _global_character->GetEquippedWeapon()->GetWeaponAnimationFile(_global_character->GetID(), _sprite_animation_alias);
    if (weapon_animation.empty() || !_current_weapon_animation.LoadFromAnimationScript(weapon_animation))
        _current_weapon_animation.Clear();

    _current_sprite_animation->ResetAnimation();
    _current_weapon_animation.ResetAnimation();
    uint32_t timer_length = _current_sprite_animation->GetAnimationLength();

    _animation_timer.Reset();
    _animation_timer.SetDuration(timer_length);
    _animation_timer.Run();
}

void BattleCharacter::ChangeActionText()
{
    if(_action) {
        ustring action_text = _action->GetName() + MakeUnicodeString(" -> ") + _action->GetTarget().GetName();
        _action_selection_text.SetText(action_text);
        if (_action->GetIconFilename().empty()) {
            _action_selection_icon.Clear();
        }
        else {
            // Determine the weapon icon according to the current skill
            std::string icon_file = _action->GetIconFilename();
            if (icon_file == "weapon") { // Alias used to trigger the loading of the weapon icon.
                std::shared_ptr<GlobalWeapon> char_wpn = GetGlobalCharacter()->GetEquippedWeapon();
                icon_file = char_wpn ?
                            char_wpn->GetIconImage().GetFilename() :
                            "data/inventory/weapons/fist-human.png";
            }
            _action_selection_icon.Clear();
            _action_selection_icon.Load(icon_file, 24, 24);
        }
        return;
    }

    // If the character is able to have an action selected, notify the player
    if((_state == ACTOR_STATE_IDLE) || (_state == ACTOR_STATE_COMMAND))
        _action_selection_text.SetText(Translate("[Select Action]"));
    else
        _action_selection_text.Clear();

    _action_selection_icon.Clear();
}

void BattleCharacter::DrawPortrait()
{
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(5.0f, 762.0f);

    std::vector<StillImage>& portrait_frames = *(_global_character->GetBattlePortraits());
    float hp_percent =  static_cast<float>(GetHitPoints()) / static_cast<float>(GetMaxHitPoints());

    if(GetHitPoints() == GetMaxHitPoints()) {
        portrait_frames[0].Draw();
    } else if(GetHitPoints() == 0) {
        portrait_frames[4].Draw();
    } else if(hp_percent > 0.75f) {
        portrait_frames[0].Draw();
        float alpha = 1.0f - ((hp_percent - 0.75f) * 4.0f);
        portrait_frames[1].Draw(Color(1.0f, 1.0f, 1.0f, alpha));
    } else if(hp_percent > 0.50f) {
        portrait_frames[1].Draw();
        float alpha = 1.0f - ((hp_percent - 0.50f) * 4.0f);
        portrait_frames[2].Draw(Color(1.0f, 1.0f, 1.0f, alpha));
    } else if(hp_percent > 0.25f) {
        portrait_frames[2].Draw();
        float alpha = 1.0f - ((hp_percent - 0.25f) * 4.0f);
        portrait_frames[3].Draw(Color(1.0f, 1.0f, 1.0f, alpha));
    } else { // (hp_precent > 0.0f)
        portrait_frames[3].Draw();
        float alpha = 1.0f - (hp_percent * 4.0f);
        portrait_frames[4].Draw(Color(1.0f, 1.0f, 1.0f, alpha));
    }
}

void BattleCharacter::DrawStatus(uint32_t order, BattleCharacter* character_command)
{
    BattleMedia& battle_media = GlobalManager->GetBattleMedia();
    GlobalMedia& media = GlobalManager->Media();
    // Used to determine where to draw the character's status
    float y_offset = 0.0f;

    // Determine what vertical order the character is in and set the y_offset accordingly
    switch(order) {
    case 0:
        y_offset = 0.0f;
        break;
    case 1:
        y_offset = 25.0f;
        break;
    case 2:
        y_offset = 50.0f;
        break;
    case 3:
        y_offset = 75.0f;
        break;
    default:
        IF_PRINT_WARNING(BATTLE_DEBUG) << "invalid order argument: " << order << std::endl;
        y_offset = 0.0f;
    }

    // Draw the character's name
    VideoManager->SetDrawFlags(VIDEO_X_RIGHT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(280.0f, 686.0f + y_offset);
    _name_text.Draw();

    if (!character_command) {
        // Draw each characters active status effect.
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
        VideoManager->MoveRelative(-273.0f, 0.0f);
        _effects_supervisor->Draw();
    } else if (this == character_command) {
        // Draw the active character status effect at bottom.
        // Draw each characters active status effect.
        VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
        VideoManager->Move(7.0f, 678.0f);
        VideoManager->MoveRelative(0.0f, -25.0f * _effects_supervisor->GetDisplayedStatusEffectNumber());
        _effects_supervisor->DrawVertical();
    }

    // Draw the status, HP and SP bars (bars are 88 pixels wide and 6 pixels high).
    const float BAR_BASE_SIZE_X = 88.0f;
    const float BAR_BASE_SIZE_Y = 6.0f;
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_NO_BLEND, 0);

    // Draw the HP bar in green.
    float bar_size = static_cast<float>(BAR_BASE_SIZE_X * GetHitPoints()) / static_cast<float>(GetMaxHitPoints());
    VideoManager->Move(313.0f, 678.0f + y_offset);

    // Add HP Bar Shadow
    VideoManager->DrawRectangle(BAR_BASE_SIZE_X, BAR_BASE_SIZE_Y, Color::dark_green_hp);

    if (GetHitPoints() > 0) {
        if (bar_size < BAR_BASE_SIZE_X / 4.0f)
            VideoManager->DrawRectangle(bar_size, BAR_BASE_SIZE_Y, Color::orange);
        else
            VideoManager->DrawRectangle(bar_size, BAR_BASE_SIZE_Y, Color::green_hp);
    }

    // Draw the SP bar in blue.
    bar_size = static_cast<float>(BAR_BASE_SIZE_X * GetSkillPoints()) / static_cast<float>(GetMaxSkillPoints());
    VideoManager->Move(425.0f, 678.0f + y_offset);

    // Add SP bar shadow
    VideoManager->DrawRectangle(BAR_BASE_SIZE_X, BAR_BASE_SIZE_Y, Color::dark_blue_sp);

    if (GetSkillPoints() > 0)
        VideoManager->DrawRectangle(bar_size, BAR_BASE_SIZE_Y, Color::blue_sp);

    // Draw the cover image over the top of the bar.
    VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
    VideoManager->Move(289.0f, 684.0f + y_offset);
    media.GetStatusIcon(vt_global::GLOBAL_STATUS_HP, vt_global::GLOBAL_INTENSITY_NEUTRAL)->Draw();
    VideoManager->MoveRelative(114.0f, 0.0f);
    media.GetStatusIcon(vt_global::GLOBAL_STATUS_SP, vt_global::GLOBAL_INTENSITY_NEUTRAL)->Draw();

    VideoManager->SetDrawFlags(VIDEO_X_CENTER, 0);
    // Draw the character's current health on top of the middle of the HP bar.
    VideoManager->Move(356.0f, 687.0f + y_offset);
    _hit_points_text.Draw();

    // Draw the character's current skill points on top of the middle of the SP bar.
    VideoManager->MoveRelative(113.0f, 0.0f);
    _skill_points_text.Draw();

    // Note: if the command menu is visible, it will be drawn over all of the components that follow below. We still perform these draw calls
    // regardless because sometimes even if the battle is in the command state, the command menu may not be drawn if a dialogue is active or if
    // a scripted scene is taking place. Its easier (and not costly) to just always draw this information rather than check for all possible
    // conditions where the command menu is not drawn.
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_CENTER, VIDEO_BLEND, 0);

    // Move to the position wher command button icons are drawn
    VideoManager->Move(545.0f, 673.0f + y_offset);

    // If this character can be issued a command, draw the appropriate command button to indicate this. The type of button drawn depends on
    // whether or not the character already has an action set. Characters that can not be issued a command have no button drawn
    if(CanSelectCommand()) {
        uint32_t button_index = 0;
        if(IsActionSet() == false)
            button_index = 1;
        else
            button_index = 6;
        button_index += order;
        battle_media.GetCharacterActionButton(button_index)->Draw();
    }

    // Draw the action icon and text
    VideoManager->MoveRelative(40.0f, 0.0f);
    _action_selection_icon.Draw();
    VideoManager->MoveRelative(28.0f, 0.0f);
    _action_selection_text.Draw();
}

} // namespace private_battle

} // namespace vt_battle
