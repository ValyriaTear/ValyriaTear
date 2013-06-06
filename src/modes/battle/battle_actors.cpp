////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2013 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    battle_actors.cpp
*** \author  Viljami Korhonen, mindflayer@allacrost.org
*** \author  Corey Hoffstein, visage@allacrost.org
*** \author  Andy Gardner, chopperdave@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Source file for actors present in battles.
*** ***************************************************************************/

#include "modes/battle/battle_actors.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actions.h"
#include "modes/battle/battle_command.h"
#include "modes/battle/battle_effects.h"
#include "modes/battle/battle_indicators.h"
#include "modes/battle/battle_utils.h"

#include "engine/input.h"
#include "engine/script/script.h"

using namespace vt_utils;
using namespace vt_audio;
using namespace vt_video;
using namespace vt_input;
using namespace vt_system;
using namespace vt_global;
using namespace vt_script;

namespace vt_battle
{

namespace private_battle
{

// Battle Particle effect class
BattleParticleEffect::BattleParticleEffect(const std::string &effect_filename):
    BattleObject()
{
    if(!_effect.LoadEffect(effect_filename))
        PRINT_WARNING << "Invalid battle particle effect file requested: "
                      << effect_filename << std::endl;
}

void BattleParticleEffect::DrawSprite()
{
    if(!_effect.IsAlive())
        return;

    _effect.Move(GetXLocation(), GetYLocation());
    _effect.Draw();
}

// BattleAmmo class
void BattleAmmo::DrawSprite()
{
    // Draw potential sprite ammo
    if(!_shown)
        return;

    VideoManager->Move(GetXLocation(), GetYLocation());
    _ammo_image.Draw(Color(0.0f, 0.0f, 0.0f, 0.6f));
    VideoManager->MoveRelative(0.0f, -_flying_height);
    _ammo_image.Draw();
}

////////////////////////////////////////////////////////////////////////////////
// BattleActor class
////////////////////////////////////////////////////////////////////////////////

BattleActor::BattleActor(GlobalActor *actor) :
    GlobalActor(*actor),
    BattleObject(),
    _state(ACTOR_STATE_INVALID),
    _global_actor(actor),
    _action(NULL),
    _execution_finished(false),
    _idle_state_time(0),
    _hurt_timer(0),
    _is_stunned(false),
    _animation_timer(0),
    _x_stamina_location(0.0f),
    _y_stamina_location(0.0f),
    _effects_supervisor(new EffectsSupervisor(this)),
    _indicator_supervisor(new IndicatorSupervisor(this))
{
    if(actor == NULL) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received NULL argument" << std::endl;
        return;
    }
}

BattleActor::~BattleActor()
{
    // If the actor did not get a chance to execute their action, delete it
    if(_action != NULL) {
        delete _action;
        _action = NULL;
    }

    delete _effects_supervisor;
    delete _indicator_supervisor;
}

void BattleActor::ResetActor()
{
    _effects_supervisor->RemoveAllStatus();

    ResetHitPoints();
    ResetSkillPoints();
    ResetStrength();
    ResetVigor();
    ResetFortitude();
    ResetProtection();
    ResetAgility();
    ResetEvade();

    // If the actor did not get a chance to execute their action, delete it
    if(_action != NULL) {
        delete _action;
        _action = NULL;
    }

    // Invalidate the actor state to force the reinit of the idle or dead state
    _state = ACTOR_STATE_INVALID;

    if(GetHitPoints() > 0)
        ChangeState(ACTOR_STATE_IDLE);
    else
        ChangeState(ACTOR_STATE_DEAD);
}

void BattleActor::ChangeState(ACTOR_STATE new_state)
{
    if(_state == new_state) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "actor was already in new state: " << new_state << std::endl;
        return;
    }

    _state = new_state;
    _state_timer.Reset();
    switch(_state) {
    case ACTOR_STATE_IDLE:
        if(_action != NULL) {
            delete _action;
            _action = NULL;
        }
        _state_timer.Initialize(_idle_state_time);
        _state_timer.Run();
        break;
    case ACTOR_STATE_WARM_UP:
        if(_action == NULL) {
            IF_PRINT_WARNING(BATTLE_DEBUG) << "no action available during state change: " << _state << std::endl;
        } else {
            _state_timer.Initialize(_action->GetWarmUpTime());
            _state_timer.Run();
        }
        break;
    case ACTOR_STATE_READY:
        if(_action == NULL) {
            IF_PRINT_WARNING(BATTLE_DEBUG) << "no action available during state change: " << _state << std::endl;
        } else {
            BattleMode::CurrentInstance()->NotifyActorReady(this);
        }
        break;
    case ACTOR_STATE_COOL_DOWN:
    {
        _execution_finished = false;
        uint32 cool_down_time = 1000; // Default value, overriden by valid actions
        if(_action)
            cool_down_time = _action->GetCoolDownTime();

        _state_timer.Initialize(cool_down_time);
        _state_timer.Run();
        break;
    }
    case ACTOR_STATE_DYING:
        ChangeSpriteAnimation("dying");
        // Note that the state timer is initiliazed in Battle Character
        // or In BattleEnemy

        // Make the battle engine aware of the actor death
        _effects_supervisor->RemoveAllStatus();
        BattleMode::CurrentInstance()->NotifyActorDeath(this);
        break;
    default:
        break;
    }
}

void BattleActor::RegisterDamage(uint32 amount)
{
    RegisterDamage(amount, NULL);
}

void BattleActor::RegisterDamage(uint32 amount, BattleTarget *target)
{
    if(amount == 0) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function called with a zero value argument" << std::endl;
        RegisterMiss(true);
        return;
    }
    if(_state == ACTOR_STATE_DYING || _state == ACTOR_STATE_DEAD) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function called when actor state was dead" << std::endl;
        RegisterMiss();
        return;
    }

    SubtractHitPoints(amount);
    _indicator_supervisor->AddDamageIndicator(amount);

    if(GetHitPoints() == 0) {
        ChangeState(ACTOR_STATE_DYING);
        return;
    }

    ChangeSpriteAnimation("hurt");

    // Apply a stun to the actor timer depending on the amount of damage dealt
    float damage_percent = static_cast<float>(amount) / static_cast<float>(GetMaxHitPoints());
    uint32 hurt_time = 0;
    if(damage_percent < 0.10f)
        hurt_time = 250;
    else if(damage_percent < 0.25f)
        hurt_time = 500;
    else if(damage_percent < 0.50f)
        hurt_time = 750;
    else // (damage_percent >= 0.50f)
        hurt_time = 1000;

    // Make the stun effect disappear faster depending on the battle type,
    // to not advantage the attacker.
    hurt_time /= BattleMode::CurrentInstance()->GetBattleTypeTimeFactor();

    // Run a shake effect for the same time.
    _hurt_timer.Initialize(hurt_time);
    _hurt_timer.Run();

    // If the damage dealt was to a point target type, check for and apply any status effects triggered by this point hit
    if((target != NULL) && (IsTargetPoint(target->GetType()) == true)) {
        GlobalAttackPoint *damaged_point = _global_actor->GetAttackPoint(target->GetPoint());
        if(damaged_point == NULL) {
            IF_PRINT_WARNING(BATTLE_DEBUG) << "target argument contained an invalid point index: " << target->GetPoint() << std::endl;
        } else {
            std::vector<std::pair<GLOBAL_STATUS, float> > status_effects = damaged_point->GetStatusEffects();
            for(std::vector<std::pair<GLOBAL_STATUS, float> >::const_iterator i = status_effects.begin(); i != status_effects.end(); i++) {
                if(RandomFloat(0.0f, 100.0f) <= i->second) {
                    RegisterStatusChange(i->first, GLOBAL_INTENSITY_POS_LESSER);
                }
            }
        }
    }
}

void BattleActor::RegisterHealing(uint32 amount, bool hit_points)
{
    if(amount == 0) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function called with a zero value argument" << std::endl;
        RegisterMiss();
        return;
    }
    if(_state == ACTOR_STATE_DYING || _state == ACTOR_STATE_DEAD) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function called when actor state was dead" << std::endl;
        RegisterMiss();
        return;
    }

    if(hit_points)
        AddHitPoints(amount);
    else
        AddSkillPoints(amount);
    _indicator_supervisor->AddHealingIndicator(amount, hit_points);
}

void BattleActor::RegisterRevive(uint32 amount)
{
    if(amount == 0) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function called with a zero value argument" << std::endl;
        RegisterMiss();
        return;
    }
    if(_state != ACTOR_STATE_DYING && _state != ACTOR_STATE_DEAD) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function called when actor state wasn't dead" << std::endl;
        RegisterMiss();
        return;
    }

    AddHitPoints(amount);
    _indicator_supervisor->AddHealingIndicator(amount, true);

    // Reset the stamina icon position and battle state time to the minimum
    SetIdleStateTime(MIN_IDLE_WAIT_TIME);

    ChangeState(ACTOR_STATE_REVIVE);
}

void BattleActor::RegisterMiss(bool was_attacked)
{
    _indicator_supervisor->AddMissIndicator();

    if(was_attacked && !IsEnemy())
        ChangeSpriteAnimation("dodge");
}

void BattleActor::RegisterStatusChange(GLOBAL_STATUS status, GLOBAL_INTENSITY intensity, uint32 duration)
{
    _effects_supervisor->ChangeStatus(status, intensity, duration);
}

void BattleActor::Update()
{
    // Don't update the state timer when the battle tells is to pause
    // when in idle state.
    // Also don't elapse the status effect time when paused.
    BattleMode *BM = BattleMode::CurrentInstance();
    if (!BM->AreActorStatesPaused() && !BM->IsInSceneMode()) {
        // Don't update the state_timer if the character is hurt.
        if (!_hurt_timer.IsRunning()) {

            // Check the stun effect only when in idle, warm up or cool down state
            if (!_is_stunned || (_state != ACTOR_STATE_IDLE && _state != ACTOR_STATE_WARM_UP && _state != ACTOR_STATE_COOL_DOWN))
                _state_timer.Update();
        }

        if (IsAlive())
            _effects_supervisor->Update();
    }
    else if (_state == ACTOR_STATE_DYING) {
        // Permits the actor to die even in pause mode,
        // so that the sprite fade out is properly done.
        _state_timer.Update();
    }

    // Ths shaking updates even in pause mode, so that the shaking
    // doesn't last indefinitely in that state.
    _hurt_timer.Update();

    _UpdateStaminaIconPosition();

    _indicator_supervisor->Update();

    if (_state_timer.IsFinished()) {
        if (_state == ACTOR_STATE_IDLE) {
            // If an action is already set for the actor, skip the command state and immediately begin the warm up state
            if (_action == NULL)
                ChangeState(ACTOR_STATE_COMMAND);
            else
                ChangeState(ACTOR_STATE_WARM_UP);
        } else if (_state == ACTOR_STATE_WARM_UP) {
            ChangeState(ACTOR_STATE_READY);
        } else if (_state == ACTOR_STATE_COOL_DOWN) {
            ChangeState(ACTOR_STATE_IDLE);
        } else if (_state == ACTOR_STATE_DYING) {
            ChangeState(ACTOR_STATE_DEAD);
        } else if (_state == ACTOR_STATE_REVIVE) {
            ChangeState(ACTOR_STATE_IDLE);
        }
    }
}

void BattleActor::_UpdateStaminaIconPosition()
{
    float x_pos = _x_stamina_location;
    float y_pos = _y_stamina_location;

    if(IsValid()) {
        if(IsEnemy())
            x_pos = STAMINA_BAR_POSITION_X + 25.0f;
        else
            x_pos = STAMINA_BAR_POSITION_X - 25.0f;
    }

    switch(_state) {
    case ACTOR_STATE_IDLE:
        y_pos = STAMINA_LOCATION_BOTTOM + ((STAMINA_LOCATION_COMMAND - STAMINA_LOCATION_BOTTOM) *
                                           _state_timer.PercentComplete());
        break;
    case ACTOR_STATE_COMMAND:
        y_pos = STAMINA_LOCATION_COMMAND;
        break;
    case ACTOR_STATE_WARM_UP:
        y_pos = STAMINA_LOCATION_COMMAND + ((STAMINA_LOCATION_TOP - STAMINA_LOCATION_COMMAND) *
                                            _state_timer.PercentComplete());
        break;
    case ACTOR_STATE_READY:
        y_pos = STAMINA_LOCATION_TOP;
        break;
    case ACTOR_STATE_ACTING:
        y_pos = STAMINA_LOCATION_TOP - 25.0f;
        break;
    case ACTOR_STATE_COOL_DOWN:
        y_pos = STAMINA_LOCATION_BOTTOM;
        break;
    case ACTOR_STATE_DYING:
        // Make the icon fall while disappearing...
        y_pos += _state_timer.PercentComplete();
        break;
    default:
        y_pos = STAMINA_LOCATION_BOTTOM + 50.0f;
        break;
    }

    // Add a shake effect when the battle actor has received damages
    if(_hurt_timer.IsRunning())
        x_pos += RandomFloat(-4.0f, 4.0f);

    _x_stamina_location = x_pos;
    _y_stamina_location = y_pos;
}

void BattleActor::DrawIndicators() const
{
    _indicator_supervisor->Draw();
}

void BattleActor::DrawStaminaIcon(const vt_video::Color &color) const
{
    if(!IsAlive())
        return;

    VideoManager->Move(_x_stamina_location, _y_stamina_location);
    // Make the stamina icon fade away when dying
    if(_state == ACTOR_STATE_DYING)
        _stamina_icon.Draw(Color(color.GetRed(), color.GetGreen(),
                                 color.GetBlue(), color.GetAlpha() - _state_timer.PercentComplete()));
    else
        _stamina_icon.Draw(color);
}

void BattleActor::SetAction(BattleAction *action)
{
    if(!action) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received NULL argument" << std::endl;
        return;
    }

    // If a previous action exists, we cancel and delete it.
    if (_action) {
        // Note: we do not display any warning if we are overwriting a previously set action in idle or command states.
        // This is a valid operation in those states
        if((_state != ACTOR_STATE_IDLE) && (_state != ACTOR_STATE_COMMAND)) {
            IF_PRINT_WARNING(BATTLE_DEBUG) << "overwriting previously set action while in actor state: " << _state << std::endl;
        }

        _action->Cancel();
        delete _action;
    }

    _action = action;
}

void BattleActor::SetAgility(uint32 agility)
{
    GlobalActor::SetAgility(agility);
    BattleMode::CurrentInstance()->SetActorIdleStateTime(this);
}

////////////////////////////////////////////////////////////////////////////////
// BattleCharacter class
////////////////////////////////////////////////////////////////////////////////

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
    _target_selection_text.SetStyle(TextStyle("text20"));
    _target_selection_text.SetText("");

    // Init the battle animation pointers
    _current_sprite_animation = _global_character->RetrieveBattleAnimation(_sprite_animation_alias);
    // Add custom weapon animation
    std::string weapon_animation;
    if (_global_character->GetWeaponEquipped())
            weapon_animation = _global_character->GetWeaponEquipped()->GetWeaponAnimationFile(_global_character->GetID(), _sprite_animation_alias);
    if (weapon_animation.empty() || !_current_weapon_animation.LoadFromAnimationScript(weapon_animation))
        _current_weapon_animation.Clear();

    // Load the potential the ammo image filename
    std::string ammo_animation_file = _global_character->GetWeaponEquipped() ?
                                      _global_character->GetWeaponEquipped()->GetAmmoImageFile() : std::string();
    if(!ammo_animation_file.empty())
        _ammo.LoadAmmoAnimatedImage(ammo_animation_file);

    // Prepare the flying height of potential ammo weapons
    _ammo.SetFlyingHeight(GetSpriteHeight() / 2.0f);
}

void BattleCharacter::ResetActor()
{
    BattleActor::ResetActor();
}

void BattleCharacter::ChangeState(ACTOR_STATE new_state)
{
    BattleActor::ChangeState(new_state);

    switch(_state) {
    case ACTOR_STATE_COMMAND:
        // The battle action should pause whenever a character enters the command state in the WAIT battle type
        if(BattleMode::CurrentInstance()->GetBattleType() == BATTLE_TYPE_WAIT) {
            BattleMode::CurrentInstance()->SetActorStatePaused(true);
        }
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
        ChangeSpriteAnimation("dying");
        _state_timer.Initialize(_current_sprite_animation->GetAnimationLength());
        _state_timer.Run();
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

    // In scene mode, only the animations are updated
    if (BattleMode::CurrentInstance()->IsInSceneMode())
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
    if(BattleMode::CurrentInstance()->GetState() == BATTLE_STATE_NORMAL) {
        _x_location = _x_origin;
        _y_location = _y_origin;
    }

    if(_sprite_animation_alias == "idle") {
        // Check whether character HP are low
        if (_is_stunned || GetHitPoints() < (GetMaxHitPoints() / 4))
            ChangeSpriteAnimation("poor");
    } else if(_sprite_animation_alias == "run") {
        // no need to do anything
    } else if(_sprite_animation_alias == "run_after_victory") {
        // no need to do anything
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
        uint32 dist = _state_timer.GetDuration() > 0 ?
                      120 * _state_timer.GetTimeExpired() / _state_timer.GetDuration() :
                      0;
        _x_location = _x_origin + dist;
    } else if(_sprite_animation_alias == "dodge") {
        _x_location = _x_origin - 20.0f;
    }

    // Add a shake effect when the battle actor has received damages
    if(_hurt_timer.IsRunning())
        _x_location = _x_origin + RandomFloat(-6.0f, 6.0f);

    // If the character has finished to execute its battle action,
    if(_state == ACTOR_STATE_ACTING && _state_timer.IsFinished()) {
        // Triggers here the skill or item action
        // and set the actor to cool down mode.
        if(!_action->Execute())
            // Indicate the the skill execution failed to the user.
            RegisterMiss();

        // If it was an item action, show the item used.
        if(_action->IsItemAction()) {
            ItemAction *item_action = static_cast<ItemAction *>(_action);
            _indicator_supervisor->AddItemIndicator(item_action->GetItem()->GetItem());
        }

        ChangeState(ACTOR_STATE_COOL_DOWN);
    }
}

void BattleCharacter::DrawSprite()
{
    VideoManager->Move(_x_location, _y_location);
    _current_sprite_animation->Draw();
    _current_weapon_animation.Draw();

    if(_is_stunned && (_state == ACTOR_STATE_IDLE || _state == ACTOR_STATE_WARM_UP || _state == ACTOR_STATE_COOL_DOWN)) {
        VideoManager->MoveRelative(0, -GetSpriteHeight());
        BattleMode::CurrentInstance()->GetMedia().GetStunnedIcon().Draw();
    }
} // void BattleCharacter::DrawSprite()

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
    if (_global_character->GetWeaponEquipped())
            weapon_animation = _global_character->GetWeaponEquipped()->GetWeaponAnimationFile(_global_character->GetID(), _sprite_animation_alias);
    if (weapon_animation.empty() || !_current_weapon_animation.LoadFromAnimationScript(weapon_animation))
        _current_weapon_animation.Clear();

    _current_sprite_animation->ResetAnimation();
    _current_weapon_animation.ResetAnimation();
    uint32 timer_length = _current_sprite_animation->GetAnimationLength();

    _animation_timer.Reset();
    _animation_timer.SetDuration(timer_length);
    _animation_timer.Run();
}

void BattleCharacter::ChangeActionText()
{
    // If the character has no action selected to be used, clear both action and target text
    if(_action == NULL) {
        // If the character is able to have an action selected, notify the player
        if((_state == ACTOR_STATE_IDLE) || (_state == ACTOR_STATE_COMMAND)) {
            _action_selection_text.SetText(Translate("[Select Action]"));
        } else {
            _action_selection_text.SetText("");
        }
        _target_selection_text.SetText("");
    }

    else {
        _action_selection_text.SetText(_action->GetName());
        _target_selection_text.SetText(_action->GetTarget().GetName());
    }
}

void BattleCharacter::DrawPortrait()
{
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_BOTTOM, VIDEO_BLEND, 0);
    VideoManager->Move(48.0f, 759.0f);

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

void BattleCharacter::DrawStatus(uint32 order, BattleCharacter* character_command)
{
    // Used to determine where to draw the character's status
    float y_offset = 0.0f;

    // Colors used for the HP/SP bars
    const Color green_hp(0.294f, 0.776f, 0.184f, 1.0f);
    const Color blue_sp(0.196f, 0.522f, 0.859f, 1.0f);

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
        VideoManager->Move(7.0f, 688.0f);
        _effects_supervisor->DrawVertical();
    }

    // draw the status, HP and SP bars (bars are 90 pixels wide and 6 pixels high)
    float bar_size;
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_NO_BLEND, 0);

    // Draw HP bar in green
    bar_size = static_cast<float>(90 * GetHitPoints()) / static_cast<float>(GetMaxHitPoints());
    VideoManager->Move(312.0f, 678.0f + y_offset);

    if(GetHitPoints() > 0) {
        if (bar_size < 90.0f / 4.0f)
            VideoManager->DrawRectangle(bar_size, 6, Color::orange);
        else
            VideoManager->DrawRectangle(bar_size, 6, green_hp);
    }

    // Draw SP bar in blue
    bar_size = static_cast<float>(90 * GetSkillPoints()) / static_cast<float>(GetMaxSkillPoints());
    VideoManager->Move(424.0f, 678.0f + y_offset);

    if(GetSkillPoints() > 0)
        VideoManager->DrawRectangle(bar_size, 6, blue_sp);

    // Draw the cover image over the top of the bar
    VideoManager->SetDrawFlags(VIDEO_BLEND, 0);
    VideoManager->Move(290.0f, 684.0f + y_offset);
    BattleMode::CurrentInstance()->GetMedia().character_HP_text.Draw();
    VideoManager->MoveRelative(114.0f, 0.0f);
    BattleMode::CurrentInstance()->GetMedia().character_SP_text.Draw();

    VideoManager->SetDrawFlags(VIDEO_X_CENTER, 0);
    // Draw the character's current health on top of the middle of the HP bar
    VideoManager->Move(355.0f, 680.0f + y_offset);
    _hit_points_text.Draw();

    // Draw the character's current skill points on top of the middle of the SP bar
    VideoManager->MoveRelative(114.0f, 0.0f);
    _skill_points_text.Draw();

    // TODO: The SetText calls below should not be done here. They should be made whenever the character's HP/SP
    // is modified. This re-renders the text every frame regardless of whether or not the HP/SP changed so its
    // not efficient

    // Update hit and skill points after drawing to reduce gpu stall
    if(_last_rendered_hp != GetHitPoints()) {
        _last_rendered_hp = GetHitPoints();
        _hit_points_text.SetText(NumberToString(_last_rendered_hp));
    }

    if(_last_rendered_sp != GetSkillPoints()) {
        _last_rendered_sp = GetSkillPoints();
        _skill_points_text.SetText(NumberToString(_last_rendered_sp));
    }

    // Note: if the command menu is visible, it will be drawn over all of the components that follow below. We still perform these draw calls
    // regardless because sometimes even if the battle is in the command state, the command menu may not be drawn if a dialogue is active or if
    // a scripted scene is taking place. Its easier (and not costly) to just always draw this information rather than check for all possible
    // conditions where the command menu is not drawn.
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_Y_CENTER, VIDEO_BLEND, 0);

    // Move to the position wher command button icons are drawn
    VideoManager->Move(545.0f, 673.0f + y_offset);

    // If this character can be issued a command, draw the appropriate command button to indicate this. The type of button drawn depends on
    // whether or not the character already has an action set. Characters that can not be issued a command have no button drawn
    if(CanSelectCommand() == true) {
        uint32 button_index = 0;
        if(IsActionSet() == false)
            button_index = 1;
        else
            button_index = 6;
        button_index += order;
        BattleMode::CurrentInstance()->GetMedia().GetCharacterActionButton(button_index)->Draw();
    }

    // Draw the action text
    VideoManager->MoveRelative(40.0f, 0.0f);
    _action_selection_text.Draw();

    // Draw the target text
    VideoManager->MoveRelative(225.0f, 0.0f);
    _target_selection_text.Draw();
} // void BattleCharacter::DrawStatus()

// /////////////////////////////////////////////////////////////////////////////
// BattleEnemy class
// /////////////////////////////////////////////////////////////////////////////

BattleEnemy::BattleEnemy(GlobalEnemy *enemy) :
    BattleActor(enemy),
    _global_enemy(enemy),
    _sprite_animation_alias("idle"),
    _sprite_alpha(1.0f)
{
    _LoadDeathAnimationScript();

    _sprite_animations = _global_enemy->GetBattleAnimations();
}

void BattleEnemy::_LoadDeathAnimationScript()
{
    // Loads potential death animation script functions
    if (_global_enemy->GetDeathScriptFilename().empty())
        return;

    std::string filename = _global_enemy->GetDeathScriptFilename();

    std::string tablespace = ScriptEngine::GetTableSpace(filename);
    ScriptManager->DropGlobalTable(tablespace);

    ReadScriptDescriptor death_script;
    if(!death_script.OpenFile(filename))
        return;

    if(death_script.OpenTablespace().empty()) {
        PRINT_ERROR << "The enemy death script file: " << filename
                    << "has got no valid namespace" << std::endl;
        death_script.CloseFile();
        return;
    }

    _death_init = death_script.ReadFunctionPointer("Initialize");
    _death_update = death_script.ReadFunctionPointer("Update");
    _death_draw_on_sprite = death_script.ReadFunctionPointer("DrawOnSprite");
    death_script.CloseFile();
}

BattleEnemy::~BattleEnemy()
{
    delete _global_actor;
}

void BattleEnemy::ResetActor()
{
    BattleActor::ResetActor();
}

void BattleEnemy::ChangeState(ACTOR_STATE new_state)
{
    BattleActor::ChangeState(new_state);

    switch(_state) {
    case ACTOR_STATE_COMMAND:
        _DecideAction();
        break;
    case ACTOR_STATE_ACTING:
        _state_timer.Initialize(400); // Default monster action time
        _state_timer.Run();
        break;
    case ACTOR_STATE_DYING:
        // Trigger the death sequence if it is valid
        if (_death_init.is_valid()) {
            try {
                ScriptCallFunction<void>(_death_init, BattleMode::CurrentInstance(), this);
            } catch(const luabind::error &e) {
                PRINT_ERROR << "Error while triggering Initialize() function of enemy id: " << _global_actor->GetID() << std::endl;
                ScriptManager->HandleLuaError(e);
            } catch(const luabind::cast_failed &e) {
                PRINT_ERROR << "Error while triggering Initialize() function of enemy id: " << _global_actor->GetID() << std::endl;
                ScriptManager->HandleCastError(e);
            }
        }
        else {
            // Default value, not used when scripted
            _state_timer.Initialize(1500);
            _state_timer.Run();
        }

        ChangeSpriteAnimation("dying");

        // Make the battle engine aware of the actor death
        _effects_supervisor->RemoveAllStatus();
        BattleMode::CurrentInstance()->NotifyActorDeath(this);
        break;
    default:
        break;
    }
}

void BattleEnemy::ChangeSpriteAnimation(const std::string &alias)
{
    _sprite_animation_alias = alias;
    // Default value used to create to left shifting
    _animation_timer.Initialize(400);
    _animation_timer.Run();
}

void BattleEnemy::Update()
{
    BattleActor::Update();

    // Updates the sprites animations
    for (uint32 i = 0; i < _sprite_animations->size(); ++i)
        _sprite_animations->at(i).Update();

    // In scene mode, only the animations are updated
    if (BattleMode::CurrentInstance()->IsInSceneMode())
        return;

    // Note: that update part only handles attack actions
    if(_state == ACTOR_STATE_ACTING) {
        if(_state_timer.PercentComplete() <= 0.50f)
            _x_location = _x_origin - TILE_SIZE * (2.0f * _state_timer.PercentComplete());
        else
            _x_location = _x_origin - TILE_SIZE * (2.0f - 2.0f * _state_timer.PercentComplete());
    } else if(_state == ACTOR_STATE_DYING) {
        if (_death_init.is_valid()) {
            if (_death_update.is_valid()) {
                // Change the state when the animation has finished.
                try {
                    if (ScriptCallFunction<bool>(_death_update))
                        ChangeState(ACTOR_STATE_DEAD);
                } catch(const luabind::error &e) {
                    PRINT_ERROR << "Error while triggering Update() function of enemy id: " << _global_actor->GetID() << std::endl;
                    ScriptManager->HandleLuaError(e);
                    // Do not block the player
                    ChangeState(ACTOR_STATE_DEAD);
                } catch(const luabind::cast_failed &e) {
                    PRINT_ERROR << "Error while triggering Update() function of enemy id: " << _global_actor->GetID() << std::endl;
                    ScriptManager->HandleCastError(e);
                    // Do not block the player
                    ChangeState(ACTOR_STATE_DEAD);
                }
            }
        }
        else {
            // Add a default fade out effect
            _sprite_alpha = 1.0f - _state_timer.PercentComplete();
        }
    }
    // Reset the animations set below to idle once done
    else if(_animation_timer.IsFinished()) {
        ChangeSpriteAnimation("idle");
    } else if(_sprite_animation_alias == "dodge") {
        _x_location = _x_origin + 20.0f;
    }

    // Add a shake effect when the battle actor has received damages
    if(_hurt_timer.IsRunning())
        _x_location = _x_origin + RandomFloat(-2.0f, 2.0f);

    if(_state == ACTOR_STATE_ACTING) {
        if(!_execution_finished) {
            if(!_action->Execute())
                RegisterMiss();
            _execution_finished = true;
        }

        if(_execution_finished && _state_timer.IsFinished() == true)
            ChangeState(ACTOR_STATE_COOL_DOWN);
    }
}

void BattleEnemy::DrawSprite()
{
    // Dead enemies are gone from screen.
    if(_state == ACTOR_STATE_DEAD)
        return;

    float hp_percent = static_cast<float>(GetHitPoints()) / static_cast<float>(GetMaxHitPoints());

    VideoManager->Move(_x_location, _y_location);
    // Alpha will range from 1.0 to 0.0 in the following calculations
    if(_state == ACTOR_STATE_DYING) {
        _sprite_animations->at(GLOBAL_ENEMY_HURT_HEAVILY).Draw(Color(1.0f, 1.0f, 1.0f, _sprite_alpha));

        try {
            if (_death_draw_on_sprite.is_valid())
                ScriptCallFunction<void>(_death_draw_on_sprite);
        } catch(const luabind::error &e) {
            PRINT_ERROR << "Error while triggering DrawOnSprite() function of enemy id: " << _global_actor->GetID() << std::endl;
            ScriptManager->HandleLuaError(e);
        } catch(const luabind::cast_failed &e) {
            PRINT_ERROR << "Error while triggering DrawOnSprite() function of enemy id: " << _global_actor->GetID() << std::endl;
            ScriptManager->HandleCastError(e);
        }

    } else if(GetHitPoints() == GetMaxHitPoints()) {
        _sprite_animations->at(GLOBAL_ENEMY_HURT_NONE).Draw();
    } else if(hp_percent > 0.75f) {
        _sprite_animations->at(GLOBAL_ENEMY_HURT_NONE).Draw();
    } else if(hp_percent >  0.5f) {
        _sprite_animations->at(GLOBAL_ENEMY_HURT_SLIGHTLY).Draw();
    } else if(hp_percent >  0.25f) {
        _sprite_animations->at(GLOBAL_ENEMY_HURT_MEDIUM).Draw();
    } else { // (hp_precent > 0.0f)
        _sprite_animations->at(GLOBAL_ENEMY_HURT_HEAVILY).Draw();
    }

    if(_is_stunned && (_state == ACTOR_STATE_IDLE || _state == ACTOR_STATE_WARM_UP || _state == ACTOR_STATE_COOL_DOWN)) {
        VideoManager->MoveRelative(0, -GetSpriteHeight());
        BattleMode::CurrentInstance()->GetMedia().GetStunnedIcon().Draw();
    }
} // void BattleEnemy::DrawSprite()

void BattleEnemy::DrawStaminaIcon(const vt_video::Color &color) const
{
    if(!IsAlive())
        return;

    VideoManager->Move(_x_stamina_location, _y_stamina_location);
    // Make the stamina icon fade away when dying, use the enemy sprite alpha
    if(_state == ACTOR_STATE_DYING) {
        _stamina_icon.Draw(Color(color.GetRed(), color.GetGreen(),
                                 color.GetBlue(), _sprite_alpha));
    }
    else {
        _stamina_icon.Draw(color);
    }
}

// TODO: No party target will work, this will have to be addressed eventually.
// The use of a skill on dead enemies is not supported either.
void BattleEnemy::_DecideAction()
{
    const std::vector<GlobalSkill*>& enemy_skills = _global_actor->GetSkills();
    if(enemy_skills.empty()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "enemy had no usable skills" << std::endl;
        ChangeState(ACTOR_STATE_IDLE);
        return;
    }

    // Obtain the living characters
    std::deque<BattleActor *> alive_characters = BattleMode::CurrentInstance()->GetCharacterParty();
    std::deque<BattleActor *>::iterator actor_iterator = alive_characters.begin();
    while(actor_iterator != alive_characters.end()) {
        if(!(*actor_iterator)->IsAlive())
            actor_iterator = alive_characters.erase(actor_iterator);
        else
            ++actor_iterator;
    }
    if(alive_characters.empty()) {
        ChangeState(ACTOR_STATE_IDLE);
        return;
    }

    // and the living enemies
    std::deque<BattleActor *> alive_enemies = BattleMode::CurrentInstance()->GetEnemyParty();
    actor_iterator = alive_enemies.begin();
    while(actor_iterator != alive_enemies.end()) {
        if(!(*actor_iterator)->IsAlive())
            actor_iterator = alive_enemies.erase(actor_iterator);
        else
            ++actor_iterator;
    }

    if(alive_enemies.empty()) {
        // it means that the enemy actually thinking now is already dead.
        PRINT_WARNING << "An enemy was deciding an action while being dead." << std::endl;
        ChangeState(ACTOR_STATE_IDLE);
        return;
    }

    // Targeting members
    BattleTarget target;
    BattleActor *actor_target = NULL;

    // Select a random skill to use
    uint32 skill_index = 0;
    if(enemy_skills.size() > 1)
        skill_index = RandomBoundedInteger(0, enemy_skills.size() - 1);
    GlobalSkill *skill = enemy_skills.at(skill_index);

    // Select the target
    GLOBAL_TARGET target_type = skill->GetTargetType();
    switch(target_type) {
    case GLOBAL_TARGET_FOE_POINT:
    case GLOBAL_TARGET_FOE:
        // Select a random living character
        if(alive_characters.size() == 1)
            actor_target = alive_characters[0];
        else
            actor_target = alive_characters[RandomBoundedInteger(0, alive_characters.size() - 1)];
        break;
    case GLOBAL_TARGET_SELF_POINT:
    case GLOBAL_TARGET_SELF:
        actor_target = this;
        break;
    case GLOBAL_TARGET_ALLY_POINT:
    case GLOBAL_TARGET_ALLY:
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
        // Select a random living enemy, selecting a dead enemy ally is unsupported at the moment.
        if(alive_enemies.size() == 1)
            actor_target = alive_enemies[0];
        else
            actor_target = alive_enemies[RandomBoundedInteger(0, alive_enemies.size() - 1)];
        break;
    case GLOBAL_TARGET_ALL_FOES: // TODO: Add support for this
    case GLOBAL_TARGET_ALL_ALLIES: // TODO: Add support for this
    default:
        PRINT_WARNING << "Unsupported enemy skill type found." << std::endl;
        ChangeState(ACTOR_STATE_IDLE);
        return;
        break;
    }

    // Potentially select the target point and finsh targeting
    switch(target_type) {
    case GLOBAL_TARGET_SELF_POINT:
    case GLOBAL_TARGET_FOE_POINT:
    case GLOBAL_TARGET_ALLY_POINT: {
        // Select a random attack point on the target
        uint32 num_points = actor_target->GetAttackPoints().size();
        uint32 point_target = 0;
        if(num_points == 1)
            point_target = 0;
        else
            point_target = RandomBoundedInteger(0, num_points - 1);

        target.SetPointTarget(target_type, point_target, actor_target);
        break;
    }

    case GLOBAL_TARGET_FOE:
    case GLOBAL_TARGET_SELF:
    case GLOBAL_TARGET_ALLY:
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
        target.SetActorTarget(target_type, actor_target);
        break;

    case GLOBAL_TARGET_ALL_FOES: // TODO: Add support for this
    case GLOBAL_TARGET_ALL_ALLIES: // TODO: Add support for this
    default:
        PRINT_WARNING << "Unsupported enemy skill type found." << std::endl;
        ChangeState(ACTOR_STATE_IDLE);
        return;
        break;
    }

    SetAction(new SkillAction(this, target, skill));
    ChangeState(ACTOR_STATE_WARM_UP);
}

} // namespace private_battle

} // namespace vt_battle
