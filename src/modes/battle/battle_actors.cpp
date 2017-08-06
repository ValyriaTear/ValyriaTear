////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
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

#include "utils/utils_pch.h"
#include "modes/battle/battle_actors.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_actions.h"
#include "modes/battle/battle_command.h"
#include "modes/battle/battle_effects.h"
#include "modes/battle/battle_utils.h"

#include "engine/input.h"

using namespace vt_utils;
using namespace vt_audio;
using namespace vt_video;
using namespace vt_input;
using namespace vt_system;
using namespace vt_global;
using namespace vt_script;
using namespace vt_common;

namespace vt_battle
{

namespace private_battle
{

// Used to make the hardcoded action movement more or less wide.
const uint32_t MOVEMENT_SIZE = 64;

//! \brief The bottom most position of the stamina bar
const float STAMINA_LOCATION_BOTTOM = 640.0f;

//! \brief The location where each actor is allowed to select a command
const float STAMINA_LOCATION_COMMAND = STAMINA_LOCATION_BOTTOM - 354.0f;

//! \brief The top most position of the stamina bar where actors are ready to execute their actions
const float STAMINA_LOCATION_TOP = STAMINA_LOCATION_BOTTOM - 508.0f;


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

// Battle animation class
BattleAnimation::BattleAnimation(const std::string& animation_filename):
    BattleObject(),
    _visible(true),
    _can_be_removed(false)
{
    if(!_animation.LoadFromAnimationScript(animation_filename))
        PRINT_WARNING << "Invalid battle animation file requested: "
                      << animation_filename << std::endl;
}

void BattleAnimation::DrawSprite()
{
    if(!IsVisible() || CanBeRemoved())
        return;

    VideoManager->Move(GetXLocation(), GetYLocation());
    _animation.Draw();
}

////////////////////////////////////////////////////////////////////////////////
// BattleActor class
////////////////////////////////////////////////////////////////////////////////

BattleActor::BattleActor(GlobalActor *actor) :
    GlobalActor(*actor),
    BattleObject(),
    _state(ACTOR_STATE_INVALID),
    _global_actor(actor),
    _action(nullptr),
    _idle_state_time(0),
    _hurt_timer(0),
    _is_stunned(false),
    _sprite_alpha(1.0f),
    _animation_timer(0),
    _stamina_location(0.0f, 0.0f),
    _effects_supervisor(new BattleStatusEffectsSupervisor(this))
{
    if(actor == nullptr) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "constructor received nullptr argument" << std::endl;
        return;
    }

    _InitStats();
    _LoadDeathAnimationScript();
    _LoadAIScript();
}

void BattleActor::_InitStats()
{
    _char_phys_atk.SetBase(_global_actor->GetPhysAtk());
    SetPhysAtkModifier(1.0f);

    _char_mag_atk.SetBase(_global_actor->GetMagAtk());
    SetMagAtkModifier(1.0f);

    _char_phys_def.SetBase(_global_actor->GetPhysDef());
    SetPhysDefModifier(1.0f);

    _char_mag_def.SetBase(_global_actor->GetMagDef());
    SetMagDefModifier(1.0f);

    _stamina.SetBase(_global_actor->GetStamina());
    SetStaminaModifier(1.0f);

    _evade.SetBase(_global_actor->GetEvade());
    SetEvadeModifier(1.0f);

    // debug
    //std::cout << "Name: " << MakeStandardString(_global_actor->GetName()) << std::endl;
    //std::cout << "phys atk base: " << _char_phys_atk.GetBase() << "should be equal to global actor total phys atk: " << _global_actor->GetPhysAtk() << std::endl;
    //std::cout << "phys atk mod (should be 1.0f): " << _char_phys_atk.GetModifier() << ", global actor phys atk mod (independant): " << _global_actor->GetPhysAtkModifier() << std::endl;
    //std::cout << "phys atk value: " << GetPhysAtk() << std::endl << std::endl;
}

BattleActor::~BattleActor()
{
    // Reset the luabind objects so their lua counterparts can be freed
    // when the lua script coroutine is removed from stack,
    // to avoid a potential segfault.
    _ai_decide_action = luabind::object();
    _death_init = luabind::object();
    _death_update = luabind::object();
    _death_draw_on_sprite = luabind::object();

    // If the actor did not get a chance to execute their action, delete it
    if(_action != nullptr) {
        delete _action;
        _action = nullptr;
    }

    delete _effects_supervisor;
}

void BattleActor::ResetActor()
{
    _effects_supervisor->RemoveAllActiveStatusEffects();

    ResetHitPoints();
    ResetSkillPoints();
    ResetPhysAtk();
    ResetMagAtk();
    ResetPhysDef();
    ResetMagDef();
    ResetStamina();
    ResetEvade();

    // If the actor did not get a chance to execute their action, delete it
    if(_action != nullptr) {
        delete _action;
        _action = nullptr;
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
    case ACTOR_STATE_COMMAND:
        // If an AI is used, it will change itself the actor state.
        if (_ai_decide_action.is_valid()) {
            try {
                luabind::call_function<void>(_ai_decide_action, BattleMode::CurrentInstance(), this);
            } catch(const luabind::error &e) {
                PRINT_ERROR << "Error while triggering DecideAction() function of actor id: " << _global_actor->GetID() << std::endl;
                ScriptManager->HandleLuaError(e);
            } catch(const luabind::cast_failed &e) {
                PRINT_ERROR << "Error while triggering DecideAction() function of actor id: " << _global_actor->GetID() << std::endl;
                ScriptManager->HandleCastError(e);
            }
            // Make the actor keep on anyway.
            _DecideAction();
        }
        else if (!_global_actor->GetBattleAIScriptFilename().empty()) {
            // Hardcoded fallback behaviour for AI-based actors.
            _DecideAction();
        }
        break;
    case ACTOR_STATE_IDLE:
        if(_action != nullptr) {
            delete _action;
            _action = nullptr;
        }
        _state_timer.Initialize(_idle_state_time);
        _state_timer.Run();
        break;
    case ACTOR_STATE_WARM_UP:
        if(_action == nullptr) {
            IF_PRINT_WARNING(BATTLE_DEBUG) << "no action available during state change: " << _state << std::endl;
        } else {
            _state_timer.Initialize(_action->GetWarmUpTime() * GetStaminaModifier());
            _state_timer.Run();
        }
        break;
    case ACTOR_STATE_READY:
        if(_action == nullptr) {
            IF_PRINT_WARNING(BATTLE_DEBUG) << "no action available during state change: " << _state << std::endl;
        } else {
            BattleMode::CurrentInstance()->NotifyActorReady(this);
        }
        break;
    case ACTOR_STATE_COOL_DOWN:
    {
        uint32_t cool_down_time = 1000; // Default value, overridden by valid actions
        if(_action)
            cool_down_time = _action->GetCoolDownTime() * GetStaminaModifier();

        _state_timer.Initialize(cool_down_time);
        _state_timer.Run();
        break;
    }
    case ACTOR_STATE_DYING:
        ChangeSpriteAnimation("dying");
        // Note that the state timer is initialized in Battle Character
        // or In BattleEnemy

        // Make the battle engine aware of the actor death
        _effects_supervisor->RemoveAllActiveStatusEffects();
        BattleMode::CurrentInstance()->NotifyActorDeath(this);

        // Init the death animation script when valid.
        if (_death_init.is_valid()) {
            try {
                luabind::call_function<void>(_death_init, BattleMode::CurrentInstance(), this);
            } catch(const luabind::error &e) {
                PRINT_ERROR << "Error while triggering Initialize() function of actor id: " << _global_actor->GetID() << std::endl;
                ScriptManager->HandleLuaError(e);
            } catch(const luabind::cast_failed &e) {
                PRINT_ERROR << "Error while triggering Initialize() function of actor id: " << _global_actor->GetID() << std::endl;
                ScriptManager->HandleCastError(e);
            }
        }
        break;
    default:
        break;
    }
}

void BattleActor::RegisterDamage(uint32_t amount)
{
    RegisterDamage(amount, nullptr);
}

void BattleActor::RegisterDamage(uint32_t amount, BattleTarget *target)
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

    // Set the indicator parameters
    BattleMode* BM = BattleMode::CurrentInstance();
    vt_mode_manager::IndicatorSupervisor& indicator = BM->GetIndicatorSupervisor();
    indicator.AddDamageIndicator(GetXLocation(), GetYLocation(), amount, _GetDamageTextStyle(amount, false));

    if(GetHitPoints() == 0) {
        ChangeState(ACTOR_STATE_DYING);
        return;
    }

    ChangeSpriteAnimation("hurt");

    // Apply a stun to the actor timer depending on the amount of damage dealt
    float damage_percent = static_cast<float>(amount) / static_cast<float>(GetMaxHitPoints());
    uint32_t hurt_time = 0;
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
    hurt_time /= BM->GetBattleTypeTimeFactor();

    // Run a shake effect for the same time.
    _hurt_timer.Initialize(hurt_time);
    _hurt_timer.Run();

    // If the damage dealt was to a point target type, check for and apply any status effects triggered by this point hit
    if((target != nullptr) && (IsTargetPoint(target->GetType()))) {
        GlobalAttackPoint* damaged_point = _global_actor->GetAttackPoint(target->GetAttackPoint());
        if(damaged_point == nullptr) {
            IF_PRINT_WARNING(BATTLE_DEBUG) << "target argument contained an invalid point index: " << target->GetAttackPoint() << std::endl;
        } else {
            std::vector<std::pair<GLOBAL_STATUS, float> > status_effects = damaged_point->GetStatusEffects();
            for(std::vector<std::pair<GLOBAL_STATUS, float> >::const_iterator i = status_effects.begin(); i != status_effects.end(); ++i) {
                if(RandomFloat(0.0f, 100.0f) <= i->second) {
                    ApplyActiveStatusEffect(i->first, GLOBAL_INTENSITY_NEG_LESSER);
                }
            }
        }
    }
}

vt_video::TextStyle BattleActor::_GetDamageTextStyle(uint32_t amount, bool is_sp_damage)
{
    const Color low_red(1.0f, 0.75f, 0.0f, 1.0f);
    const Color mid_red(1.0f, 0.50f, 0.0f, 1.0f);
    const Color high_red(1.0f, 0.25f, 0.0f, 1.0f);

    const Color low_blue(0.0f, 0.75f, 1.0f, 1.0f);
    const Color mid_blue(0.0f, 0.50f, 1.0f, 1.0f);
    const Color high_blue(0.0f, 0.25f, 1.0f, 1.0f);

    TextStyle style;

    float damage_percent = static_cast<float>(amount) / static_cast<float>(GetMaxHitPoints());
    if(damage_percent < 0.10f) {
        style.SetColor(is_sp_damage ? low_blue : low_red);
    } else if(damage_percent < 0.20f) {
        style.SetColor(is_sp_damage ? mid_blue : mid_red);
    } else if(damage_percent < 0.30f) {
        style.SetColor(is_sp_damage ? high_blue : high_red);
    } else { // (damage_percent >= 0.30f)
        style.SetColor(is_sp_damage ? Color::blue : Color::red);
    }
    style.SetShadowStyle(VIDEO_TEXT_SHADOW_BLACK);
    style.SetShadowOffsets(1, -2);

    // Set the text size depending on the amount of pure damage.
    std::string font;
    if(amount < 50) {
        font = "text24"; // text24
    } else if(amount < 100) {
        font = "text24.2";
    } else if(amount < 250) {
        font = "text26";
    } else if(amount < 500) {
        font = "text28";
    } else if(amount < 1000) {
        font = "text36";
    } else {
        font = "text48";
    }
    style.SetFont(font);

    return style;
}

void BattleActor::RegisterSPDamage(uint32_t amount)
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

    SubtractSkillPoints(amount);

    // Set the indicator parameters
    BattleMode* BM = BattleMode::CurrentInstance();
    vt_mode_manager::IndicatorSupervisor& indicator = BM->GetIndicatorSupervisor();
    indicator.AddDamageIndicator(GetXLocation(), GetYLocation(), amount, _GetDamageTextStyle(amount, true));
}

void BattleActor::RegisterHealing(uint32_t amount, bool hit_points)
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

    // Set the indicator parameters
    float y_pos = GetYLocation() - (GetSpriteHeight() / 3 * 2);
    BattleMode* BM = BattleMode::CurrentInstance();
    vt_mode_manager::IndicatorSupervisor& indicator = BM->GetIndicatorSupervisor();
    indicator.AddHealingIndicator(GetXLocation(), y_pos, amount, _GetHealingTextStyle(amount, hit_points));
}

void BattleActor::RegisterRevive(uint32_t amount)
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

    // Set the indicator parameters
    float y_pos = GetYLocation() - (GetSpriteHeight() / 3 * 2);
    BattleMode* BM = BattleMode::CurrentInstance();
    vt_mode_manager::IndicatorSupervisor& indicator = BM->GetIndicatorSupervisor();
    indicator.AddHealingIndicator(GetXLocation(), y_pos, amount, _GetHealingTextStyle(amount, true));

    // Reset the stamina icon position and battle state time to the minimum
    BM->SetActorIdleStateTime(this);

    ChangeState(ACTOR_STATE_REVIVE);
}

vt_video::TextStyle BattleActor::_GetHealingTextStyle(uint32_t amount, bool is_hp)
{
    const Color low_green(0.0f, 1.0f, 0.60f, 1.0f);
    const Color mid_green(0.0f, 1.0f, 0.30f, 1.0f);
    const Color high_green(0.0f, 1.0f, 0.15f, 1.0f);

    const Color low_blue(0.0f, 0.60f, 1.0f, 1.0f);
    const Color mid_blue(0.0f, 0.30f, 1.0f, 1.0f);
    const Color high_blue(0.0f, 0.15f, 1.0f, 1.0f);

    TextStyle style;

    // Use different colors/shades of green/blue for different degrees of healing
    std::string font;
    float healing_percent = static_cast<float>(amount / GetMaxHitPoints());
    if(healing_percent < 0.10f) {
        font = "text24";
        style.SetColor(is_hp ? low_green : low_blue);
    } else if(healing_percent < 0.20f) {
        font = "text24";
        style.SetColor(is_hp ? mid_green : mid_blue);
    } else if(healing_percent < 0.30f) {
        font = "text26";
        style.SetColor(is_hp ? high_green : high_blue);
    } else { // (healing_percent >= 0.30f)
        font = "text26";
        style.SetColor(is_hp ? Color::green : Color::blue);
    }
    style.SetFont(font);
    style.SetShadowStyle(VIDEO_TEXT_SHADOW_BLACK);
    style.SetShadowOffsets(1, -2);

    return style;
}

void BattleActor::RegisterMiss(bool was_attacked)
{
    // Set the indicator parameters
    float y_pos = GetYLocation() - (GetSpriteHeight() / 2);
    BattleMode* BM = BattleMode::CurrentInstance();
    vt_mode_manager::IndicatorSupervisor& indicator = BM->GetIndicatorSupervisor();
    indicator.AddMissIndicator(GetXLocation(), y_pos);

    if(was_attacked && IsAlive())
        ChangeSpriteAnimation("dodge");
}

void BattleActor::ApplyActiveStatusEffect(GLOBAL_STATUS status, GLOBAL_INTENSITY intensity, uint32_t duration)
{
    _effects_supervisor->ChangeActiveStatusEffect(status, intensity, duration);
}

vt_global::GLOBAL_INTENSITY BattleActor::GetActiveStatusEffectIntensity(vt_global::GLOBAL_STATUS status)
{
    return _effects_supervisor->GetActiveStatusIntensity(status);
}

void BattleActor::RemoveActiveStatusEffect(GLOBAL_STATUS status_effect)
{
    _effects_supervisor->RemoveActiveStatusEffect(status_effect);
}

void BattleActor::Update()
{
    BattleMode *BM = BattleMode::CurrentInstance();

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

    // Don't update the state timer when the battle tells is to pause
    // when in idle state.
    // Also don't elapse the status effect time when paused.
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

        // Updates the scripted death animation if any.
        if (_death_init.is_valid() && _death_update.is_valid()) {
            // Change the state when the animation has finished.
            try {
                if (luabind::call_function<bool>(_death_update))
                    ChangeState(ACTOR_STATE_DEAD);
            } catch(const luabind::error &e) {
                PRINT_ERROR << "Error while triggering Update() function of actor id: " << _global_actor->GetID() << std::endl;
                ScriptManager->HandleLuaError(e);
                // Do not block the player
                ChangeState(ACTOR_STATE_DEAD);
            } catch(const luabind::cast_failed &e) {
                PRINT_ERROR << "Error while triggering Update() function of actor id: " << _global_actor->GetID() << std::endl;
                ScriptManager->HandleCastError(e);
                // Do not block the player
                ChangeState(ACTOR_STATE_DEAD);
            }
        }
    }

    // The shaking updates even in pause mode, so that the shaking
    // doesn't last indefinitely in that state.
    _hurt_timer.Update();

    _UpdateStaminaIconPosition();

    if (_state_timer.IsFinished()) {
        switch(_state) {
        case ACTOR_STATE_IDLE:
            // If an action is already set for the actor,
            // skip the command state and immediately begin the warm up state.
            if (_action == nullptr)
                ChangeState(ACTOR_STATE_COMMAND);
            else
                ChangeState(ACTOR_STATE_WARM_UP);
            break;
        case ACTOR_STATE_WARM_UP:
            ChangeState(ACTOR_STATE_READY);
            break;
        case ACTOR_STATE_SHOWNOTICE:
            ChangeState(ACTOR_STATE_NOTICEDONE);
            break;
        case ACTOR_STATE_COOL_DOWN:
            ChangeState(ACTOR_STATE_IDLE);
            break;
        case ACTOR_STATE_DYING:
            ChangeState(ACTOR_STATE_DEAD);
            break;
        case ACTOR_STATE_REVIVE:
            ChangeState(ACTOR_STATE_IDLE);
            break;
        default:
            break;
        }
    }
}

void BattleActor::_UpdateStaminaIconPosition()
{
    Position2D pos = _stamina_location;

    if(CanFight()) {
        if(IsEnemy())
            pos.x = STAMINA_BAR_POSITION_X + 25.0f;
        else
            pos.x = STAMINA_BAR_POSITION_X - 25.0f;
    }

    switch(_state) {
    case ACTOR_STATE_IDLE:
        pos.y = STAMINA_LOCATION_BOTTOM + ((STAMINA_LOCATION_COMMAND - STAMINA_LOCATION_BOTTOM) *
                                           _state_timer.PercentComplete());
        break;
    case ACTOR_STATE_COMMAND:
        pos.y = STAMINA_LOCATION_COMMAND;
        break;
    case ACTOR_STATE_WARM_UP:
        pos.y = STAMINA_LOCATION_COMMAND + ((STAMINA_LOCATION_TOP - STAMINA_LOCATION_COMMAND) *
                                            _state_timer.PercentComplete());
        break;
    case ACTOR_STATE_READY:
        pos.y = STAMINA_LOCATION_TOP;
        break;
    case ACTOR_STATE_SHOWNOTICE:
    case ACTOR_STATE_NOTICEDONE:
    case ACTOR_STATE_ACTING:
        pos.y = STAMINA_LOCATION_TOP - 25.0f;
        break;
    case ACTOR_STATE_COOL_DOWN:
        pos.y = STAMINA_LOCATION_BOTTOM;
        break;
    case ACTOR_STATE_DYING:
        // Make the icon fall while disappearing...
        pos.y += _state_timer.PercentComplete();
        break;
    default:
        pos.y = STAMINA_LOCATION_BOTTOM + 50.0f;
        break;
    }

    // Add a shake effect when the battle actor has received damages
    if(_hurt_timer.IsRunning())
        pos.x += RandomFloat(-4.0f, 4.0f);

    _stamina_location = pos;
}

void BattleActor::DrawStaminaIcon(const vt_video::Color &color) const
{
    if(!IsAlive())
        return;

    VideoManager->Move(_stamina_location.x, _stamina_location.y);
    // Make the stamina icon fade away when dying
    if(_state == ACTOR_STATE_DYING)
        _stamina_icon.Draw(Color(color.GetRed(), color.GetGreen(),
                                 color.GetBlue(), color.GetAlpha() - _state_timer.PercentComplete()));
    else
        _stamina_icon.Draw(color);
}

void BattleActor::SetAction(BattleAction* action)
{
    if(!action) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "function received nullptr argument" << std::endl;
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

void BattleActor::SetAction(uint32_t skill_id, BattleActor* target_actor)
{
    const std::vector<GlobalSkill *>& actor_skills = _global_actor->GetSkills();

    GlobalSkill* skill = nullptr;

    for (uint32_t i = 0; i < actor_skills.size(); ++i) {
        if (actor_skills[i]->GetID() == skill_id && actor_skills[i]->IsExecutableInBattle()) {
            skill = actor_skills[i];
            break;
        }
    }

    if (!skill) {
        PRINT_WARNING << "The actor has got no usable skill with ID: " << skill_id
            << ". Its battle action failed." << std::endl;
        ChangeState(ACTOR_STATE_IDLE);
        return;
    }

    if (skill->GetSPRequired() > GetSkillPoints()) {
        PRINT_WARNING << "The skill cost of this skill: " << skill_id
            << " was too high. The battle action failed" << std::endl;
        ChangeState(ACTOR_STATE_IDLE);
        return;
    }

    BattleTarget target;
    GLOBAL_TARGET target_type = skill->GetTargetType();

    // Auto-adjust target for self directed skills.
    if (target_type == GLOBAL_TARGET_SELF || target_type == GLOBAL_TARGET_SELF_POINT)
        target_actor = this;

    switch(target_type) {
    case GLOBAL_TARGET_ALL_FOES:
    case GLOBAL_TARGET_ALL_ALLIES:
        target.SetTarget(this, target_type);
        break;

    case GLOBAL_TARGET_FOE:
    case GLOBAL_TARGET_SELF:
    case GLOBAL_TARGET_ALLY:
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
    case GLOBAL_TARGET_DEAD_ALLY_ONLY:
        target.SetTarget(this, target_type, target_actor);
        break;

    case GLOBAL_TARGET_SELF_POINT:
    case GLOBAL_TARGET_FOE_POINT:
    case GLOBAL_TARGET_ALLY_POINT: {
        // Select a random attack point on the target
        uint32_t num_points = target_actor->GetAttackPoints().size();
        uint32_t point_target = 0;
        if(num_points == 1)
            point_target = 0;
        else
            point_target = RandomBoundedInteger(0, num_points - 1);

        target.SetTarget(this, target_type, target_actor, point_target);
        break;
    }

    default:
        PRINT_ERROR << "Invalid target type in SetAction()" << std::endl;
        ChangeState(ACTOR_STATE_IDLE);
        return;
        break;
    }

    SetAction(new SkillAction(this, target, skill));
    ChangeState(ACTOR_STATE_WARM_UP);
}

void BattleActor::SetStamina(uint32_t base)
{
    GlobalActor::SetStamina(base);
    BattleMode::CurrentInstance()->SetActorIdleStateTime(this);
}

void BattleActor::SetStaminaModifier(float modifier)
{
    GlobalActor::SetStaminaModifier(modifier);
    BattleMode::CurrentInstance()->SetActorIdleStateTime(this);
}

void BattleActor::_LoadDeathAnimationScript()
{
    // Loads potential death animation script functions
    if (_global_actor->GetDeathScriptFilename().empty())
        return;

    std::string filename = _global_actor->GetDeathScriptFilename();

    std::string tablespace = ScriptEngine::GetTableSpace(filename);
    ScriptManager->DropGlobalTable(tablespace);

    if(!_death_script.OpenFile(filename))
        return;

    if(_death_script.OpenTablespace().empty()) {
        PRINT_ERROR << "The actor death script file: " << filename
                    << "has got no valid namespace" << std::endl;
        _death_script.CloseFile();
        return;
    }

    _death_init = _death_script.ReadFunctionPointer("Initialize");
    _death_update = _death_script.ReadFunctionPointer("Update");
    _death_draw_on_sprite = _death_script.ReadFunctionPointer("DrawOnSprite");
}

void BattleActor::_LoadAIScript()
{
    std::string filename = _global_actor->GetBattleAIScriptFilename();

    if (filename.empty())
        return;

    std::string tablespace = ScriptEngine::GetTableSpace(filename);
    ScriptManager->DropGlobalTable(tablespace);

    if(!_ai_script.OpenFile(filename))
        return;

    if(_ai_script.OpenTablespace().empty()) {
        PRINT_ERROR << "The actor battle AI script file: " << filename
                    << "has got no valid namespace" << std::endl;
        _ai_script.CloseFile();
        return;
    }

    _ai_decide_action = _ai_script.ReadFunctionPointer("DecideAction");
}

void BattleActor::_DecideAction()
{
    const std::vector<GlobalSkill *>& actor_skills = _global_actor->GetSkills();
    std::vector<GlobalSkill *> usable_skills;
    std::vector<GlobalSkill*>::const_iterator skill_it = actor_skills.begin();
    while(skill_it != actor_skills.end()) {
        if((*skill_it)->IsExecutableInBattle() && (*skill_it)->GetSPRequired() <= GetSkillPoints())
            usable_skills.push_back(*skill_it);
        ++skill_it;
    }

    if(usable_skills.empty()) {
        IF_PRINT_WARNING(BATTLE_DEBUG) << "The actor had no usable skills" << std::endl;
        ChangeState(ACTOR_STATE_IDLE);
        return;
    }

    BattleMode* BM = BattleMode::CurrentInstance();
    // If this function is used by an enemy, then enemies and characters must be swapped,
    // as the roles are inversed.
    std::deque<BattleActor *>& characters = IsEnemy() ? BM->GetEnemyParty() : BM->GetCharacterParty();
    std::deque<BattleActor *>& enemies = IsEnemy() ? BM->GetCharacterParty() : BM->GetEnemyParty();

    std::deque<BattleActor *> alive_characters;
    std::deque<BattleActor *> dead_characters;
    std::deque<BattleActor *>::const_iterator it = characters.begin();
    while(it != characters.end()) {
        if((*it)->IsAlive())
            alive_characters.push_back(*it);
        else
            dead_characters.push_back(*it);
        ++it;
    }
    if(alive_characters.empty()) {
        ChangeState(ACTOR_STATE_IDLE);
        return;
    }

    // and the enemies depending on their state
    std::deque<BattleActor *> alive_enemies;
    it = enemies.begin();
    while(it != enemies.end()) {
        if((*it)->IsAlive())
            alive_enemies.push_back(*it);
        ++it;
    }

    if(alive_enemies.empty()) {
        ChangeState(ACTOR_STATE_IDLE);
        return;
    }

    // Targeting members
    BattleTarget target;
    BattleActor* actor_target = nullptr;

    // Select a random skill to use
    uint32_t skill_index = 0;
    if(usable_skills.size() > 1)
        skill_index = RandomBoundedInteger(0, usable_skills.size() - 1);
    GlobalSkill* skill = usable_skills.at(skill_index);

    // Select the target
    GLOBAL_TARGET target_type = skill->GetTargetType();
    switch(target_type) {
    case GLOBAL_TARGET_FOE_POINT:
    case GLOBAL_TARGET_FOE:
        // Select a random living enemy
        if(alive_enemies.size() == 1)
            actor_target = alive_enemies[0];
        else
            actor_target = alive_enemies[RandomBoundedInteger(0, alive_enemies.size() - 1)];
        break;
    case GLOBAL_TARGET_SELF_POINT:
    case GLOBAL_TARGET_SELF:
        actor_target = this;
        break;
    case GLOBAL_TARGET_ALLY_POINT:
    case GLOBAL_TARGET_ALLY:
        // Select a random living character
        if(alive_characters.size() == 1)
            actor_target = alive_characters[0];
        else
            actor_target = alive_characters[RandomBoundedInteger(0, alive_characters.size() - 1)];
        break;
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
        // Select a random ally, living or not
        if(characters.size() == 1)
            actor_target = characters[0];
        else
            actor_target = characters[RandomBoundedInteger(0, characters.size() - 1)];
        break;
    case GLOBAL_TARGET_DEAD_ALLY_ONLY:
        if (dead_characters.empty()) {
            // Abort the skill since there is no valid targets.
            ChangeState(ACTOR_STATE_IDLE);
            return;
        }

        // Select a random ally, living or not
        if(dead_characters.size() == 1)
            actor_target = dead_characters[0];
        else
            actor_target = dead_characters[RandomBoundedInteger(0, dead_characters.size() - 1)];
        break;
    case GLOBAL_TARGET_ALL_FOES:
    case GLOBAL_TARGET_ALL_ALLIES:
        // Nothing to do here, the party deques are ready
        break;
    default:
        PRINT_WARNING << "Unsupported enemy skill type found." << std::endl;
        ChangeState(ACTOR_STATE_IDLE);
        return;
        break;
    }

    // Potentially select the target point and finish targeting
    switch(target_type) {
    case GLOBAL_TARGET_SELF_POINT:
    case GLOBAL_TARGET_FOE_POINT:
    case GLOBAL_TARGET_ALLY_POINT: {
        // Select a random attack point on the target
        uint32_t num_points = actor_target->GetAttackPoints().size();
        uint32_t point_target = 0;
        if(num_points == 1)
            point_target = 0;
        else
            point_target = RandomBoundedInteger(0, num_points - 1);

        target.SetTarget(this, target_type, actor_target, point_target);
        break;
    }

    case GLOBAL_TARGET_FOE:
    case GLOBAL_TARGET_SELF:
    case GLOBAL_TARGET_ALLY:
    case GLOBAL_TARGET_ALLY_EVEN_DEAD:
    case GLOBAL_TARGET_DEAD_ALLY_ONLY:
        target.SetTarget(this, target_type, actor_target);
        break;
    case GLOBAL_TARGET_ALL_FOES: // Supported at script level
        target.SetTarget(this, target_type, enemies.at(0));
        break;
    case GLOBAL_TARGET_ALL_ALLIES: // Supported at script level
        target.SetTarget(this, target_type, characters.at(0));
        break;
    default:
        PRINT_WARNING << "Unsupported enemy skill type found." << std::endl;
        ChangeState(ACTOR_STATE_IDLE);
        return;
        break;
    }

    SetAction(new SkillAction(this, target, skill));
    ChangeState(ACTOR_STATE_WARM_UP);
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

    // Init the battle animation pointers
    _current_sprite_animation = _global_character->RetrieveBattleAnimation(_sprite_animation_alias);
    // Add custom weapon animation
    std::string weapon_animation;
    if (_global_character->GetWeaponEquipped())
            weapon_animation = _global_character->GetWeaponEquipped()->GetWeaponAnimationFile(_global_character->GetID(), _sprite_animation_alias);
    if (weapon_animation.empty() || !_current_weapon_animation.LoadFromAnimationScript(weapon_animation))
        _current_weapon_animation.Clear();

    // Load the potential the ammo image filename
    _ammo_animation_file = _global_character->GetWeaponEquipped() ?
                               _global_character->GetWeaponEquipped()->GetAmmoAnimationFile() : std::string();

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
        if(BattleMode::CurrentInstance()->GetBattleType() == BATTLE_TYPE_WAIT) {
            // The battle action should pause whenever a character enters the command state in the WAIT battle type
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
    case ACTOR_STATE_SHOWNOTICE:
        if (_action && _action->ShouldShowSkillNotice()) {
            _state_timer.Initialize(1000);
            _state_timer.Run();

            // Determine the current weapon icon if existing...
            std::string icon_filename;
            if (_action->GetIconFilename() == "weapon") {
                std::shared_ptr<GlobalWeapon> wpn = _global_character->GetWeaponEquipped();
                if (wpn) {
                    icon_filename = _global_character->GetWeaponEquipped()->GetIconImage().GetFilename();
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
            ItemAction *item_action = static_cast<ItemAction *>(_action);

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
    if (_global_character->GetWeaponEquipped())
            weapon_animation = _global_character->GetWeaponEquipped()->GetWeaponAnimationFile(_global_character->GetID(), _sprite_animation_alias);
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
                std::shared_ptr<GlobalWeapon> char_wpn = GetGlobalCharacter()->GetWeaponEquipped();
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

void BattleCharacter::DrawStatus(uint32_t order, BattleCharacter* character_command)
{
    BattleMedia& battle_media = GlobalManager->GetBattleMedia();
    GlobalMedia& media = GlobalManager->Media();
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

    // Draw the status, HP and SP bars (bars are 88 pixels wide and 6 pixels high).
    const float BAR_BASE_SIZE_X = 88.0f;
    const float BAR_BASE_SIZE_Y = 6.0f;
    VideoManager->SetDrawFlags(VIDEO_X_LEFT, VIDEO_NO_BLEND, 0);

    // Draw the HP bar in green.
    float bar_size = static_cast<float>(BAR_BASE_SIZE_X * GetHitPoints()) / static_cast<float>(GetMaxHitPoints());
    VideoManager->Move(313.0f, 678.0f + y_offset);

    if (GetHitPoints() > 0) {
        if (bar_size < BAR_BASE_SIZE_X / 4.0f)
            VideoManager->DrawRectangle(bar_size, BAR_BASE_SIZE_Y, Color::orange);
        else
            VideoManager->DrawRectangle(bar_size, BAR_BASE_SIZE_Y, green_hp);
    }

    // Draw the SP bar in blue.
    bar_size = static_cast<float>(BAR_BASE_SIZE_X * GetSkillPoints()) / static_cast<float>(GetMaxSkillPoints());
    VideoManager->Move(425.0f, 678.0f + y_offset);

    if (GetSkillPoints() > 0)
        VideoManager->DrawRectangle(bar_size, BAR_BASE_SIZE_Y, blue_sp);

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

// /////////////////////////////////////////////////////////////////////////////
// BattleEnemy class
// /////////////////////////////////////////////////////////////////////////////

BattleEnemy::BattleEnemy(uint32_t enemy_id) :
    BattleActor(new vt_global::GlobalEnemy(enemy_id)),
    _sprite_animation_alias("idle"),
    _action_finished(false)
{
    _global_enemy = static_cast<GlobalEnemy*>(_global_actor);

    _sprite_animations = _global_enemy->GetBattleAnimations();
}

BattleEnemy::~BattleEnemy()
{
    // If the actor is an enemy, we can delete it as only the characters
    // will be needed to remain between battles.
    // NOTE: We don't delete the _global_actor pointer as it is an alias
    // of this one.
    delete _global_enemy;
}

void BattleEnemy::ChangeState(ACTOR_STATE new_state)
{
    BattleActor::ChangeState(new_state);

    switch(_state) {
    case ACTOR_STATE_COMMAND:
        // Hardcoded fallback behaviour for enemies if no AI script is provided.
        _DecideAction();
        break;
    case ACTOR_STATE_SHOWNOTICE:
        if (_action && _action->ShouldShowSkillNotice()) {
            _state_timer.Initialize(1000);
            _state_timer.Run();

            // Determine the current weapon icon if existing...
            std::string icon_filename;
            if (_action->GetIconFilename() != "weapon")
                icon_filename = _action->GetIconFilename();
            BattleMode::CurrentInstance()->GetIndicatorSupervisor().AddShortNotice(_action->GetName(),
                                                                                   icon_filename,
                                                                                   _state_timer.GetDuration());
        }
        else {
            _state = ACTOR_STATE_NOTICEDONE;
        }
        break;
    case ACTOR_STATE_ACTING:
        _action->Initialize();
        if(_action->IsScripted())
            return;

        _state_timer.Initialize(400); // Default monster action time
        _state_timer.Run();
        break;
    case ACTOR_STATE_REVIVE:
        _state_timer.Initialize(2000);
        _state_timer.Run();
        break;
    case ACTOR_STATE_DYING:
        // Trigger the death sequence if it is valid
        if (_death_init.is_valid()) {
            try {
                luabind::call_function<void>(_death_init, BattleMode::CurrentInstance(), this);
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
        _effects_supervisor->RemoveAllActiveStatusEffects();
        BattleMode::CurrentInstance()->NotifyActorDeath(this);
        break;
    default:
        break;
    }
}

void BattleEnemy::ChangeSpriteAnimation(const std::string &alias)
{
    _sprite_animation_alias = alias;
    _animation_timer.Reset();
    // Default value used to create to left shifting
    _animation_timer.SetDuration(400);
    _animation_timer.Run();
}

void BattleEnemy::Update()
{
    BattleActor::Update();

    _animation_timer.Update();

    // Updates the sprites animations
    for (uint32_t i = 0; i < _sprite_animations->size(); ++i)
        _sprite_animations->at(i).Update();

    // In scene mode, only the animations are updated
    if (BattleMode::CurrentInstance()->IsInSceneMode())
        return;

    // Note: that update part only handles attack actions
    if(_state == ACTOR_STATE_ACTING) {
        // Update potential scripted Battle action without hardcoded logic in that case
        if(_action && _action->IsScripted()) {
            if(!_action->Update())
                return;
            else
                ChangeState(ACTOR_STATE_COOL_DOWN);
        }

        // Hardcoded action handling
        if(_state_timer.PercentComplete() <= 0.50f) {
            _location.x = _origin.x - MOVEMENT_SIZE * (2.0f * _state_timer.PercentComplete());
        }
        else {
            // Execute before moving back
            if(!_action_finished) {
                if(!_action->Execute())
                    RegisterMiss();

                // If it was an item action, show the item used.
                if(_action->IsItemAction()) {
                    ItemAction *item_action = static_cast<ItemAction *>(_action);

                    // Creates an item indicator
                    float y_pos = GetYLocation() - GetSpriteHeight();
                    vt_mode_manager::IndicatorSupervisor& indicator = BattleMode::CurrentInstance()->GetIndicatorSupervisor();
                    indicator.AddItemIndicator(GetXLocation(), y_pos, item_action->GetBattleItem()->GetGlobalItem());
                }

                _action_finished = true;
            }

            _location.x = _origin.x - MOVEMENT_SIZE * (2.0f - 2.0f * _state_timer.PercentComplete());
        }

        if(_action_finished && _state_timer.IsFinished()) {
            // For the next action
            _action_finished = false;
            ChangeState(ACTOR_STATE_COOL_DOWN);
        }

    } else if(_state == ACTOR_STATE_DYING) {
        if (!_death_init.is_valid() || !_death_update.is_valid()) {
            // Use a default fade out effect when not scripted.
            _sprite_alpha = 1.0f - _state_timer.PercentComplete();
        }
    }
    else if (_state == ACTOR_STATE_REVIVE) {
        // Fade the enemy in.
        _sprite_alpha = _state_timer.PercentComplete();
    }
    else if(_animation_timer.IsFinished()) {
        // Reset the animations set below to idle once done
        ChangeSpriteAnimation("idle");
        _location.x = _origin.x;
    } else if(_sprite_animation_alias == "dodge") {
        _location.x = _origin.x + 20.0f;
    }

    // Add a shake effect when the battle actor has received damages
    if(_hurt_timer.IsRunning())
        _location.x = _origin.x + RandomFloat(-2.0f, 2.0f);
}

void BattleEnemy::DrawSprite()
{
    // Dead enemies are gone from screen.
    if(_state == ACTOR_STATE_DEAD)
        return;

    float hp_percent = static_cast<float>(GetHitPoints()) / static_cast<float>(GetMaxHitPoints());

    VideoManager->Move(_location.x, _location.y);
    // Alpha will range from 1.0 to 0.0 in the following calculations
    if(_state == ACTOR_STATE_DYING) {
        _sprite_animations->at(GLOBAL_ENEMY_HURT_HEAVILY).Draw(Color(1.0f, 1.0f, 1.0f, _sprite_alpha));

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

    } else if(GetHitPoints() == GetMaxHitPoints()) {
        _sprite_animations->at(GLOBAL_ENEMY_HURT_NONE).Draw(Color(1.0f, 1.0f, 1.0f, _sprite_alpha));
    } else if(hp_percent > 0.75f) {
        _sprite_animations->at(GLOBAL_ENEMY_HURT_NONE).Draw(Color(1.0f, 1.0f, 1.0f, _sprite_alpha));
    } else if(hp_percent >  0.5f) {
        _sprite_animations->at(GLOBAL_ENEMY_HURT_SLIGHTLY).Draw(Color(1.0f, 1.0f, 1.0f, _sprite_alpha));
    } else if(hp_percent >  0.25f) {
        _sprite_animations->at(GLOBAL_ENEMY_HURT_MEDIUM).Draw(Color(1.0f, 1.0f, 1.0f, _sprite_alpha));
    } else { // (hp_precent > 0.0f)
        _sprite_animations->at(GLOBAL_ENEMY_HURT_HEAVILY).Draw(Color(1.0f, 1.0f, 1.0f, _sprite_alpha));
    }

    if(_is_stunned && (_state == ACTOR_STATE_IDLE || _state == ACTOR_STATE_WARM_UP || _state == ACTOR_STATE_COOL_DOWN)) {
        VideoManager->MoveRelative(0, -GetSpriteHeight());
        GlobalManager->GetBattleMedia().GetStunnedIcon().Draw();
    }
} // void BattleEnemy::DrawSprite()

void BattleEnemy::DrawStaminaIcon(const vt_video::Color &color) const
{
    if(!IsAlive())
        return;

    VideoManager->Move(_stamina_location.x, _stamina_location.y);
    // Make the stamina icon fade away when dying, use the enemy sprite alpha
    if(_state == ACTOR_STATE_DYING) {
        _stamina_icon.Draw(Color(color.GetRed(), color.GetGreen(),
                                 color.GetBlue(), _sprite_alpha));
    }
    else {
        _stamina_icon.Draw(color);
    }
}

} // namespace private_battle

} // namespace vt_battle
