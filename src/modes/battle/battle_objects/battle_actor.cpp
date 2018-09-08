////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "modes/battle/battle_objects/battle_actor.h"

#include "modes/battle/battle.h"
#include "modes/battle/battle_effects.h"
#include "modes/battle/battle_actions/skill_action.h"
#include "modes/battle/battle_target.h"

#include "common/global/actors/global_attack_point.h"
#include "common/global/global_skills.h"

#include "utils/utils_random.h"

using namespace vt_common;
using namespace vt_global;
using namespace vt_script;
using namespace vt_utils;
using namespace vt_video;

namespace vt_battle
{

namespace private_battle
{

//! \brief The bottom most position of the stamina bar
const float STAMINA_LOCATION_BOTTOM = 640.0f;

//! \brief The location where each actor is allowed to select a command
const float STAMINA_LOCATION_COMMAND = STAMINA_LOCATION_BOTTOM - 354.0f;

//! \brief The top most position of the stamina bar where actors are ready to execute their actions
const float STAMINA_LOCATION_TOP = STAMINA_LOCATION_BOTTOM - 508.0f;

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
                // Make the actor keep on anyway.
                _DecideAction();
            } catch(const luabind::cast_failed &e) {
                PRINT_ERROR << "Error while triggering DecideAction() function of actor id: " << _global_actor->GetID() << std::endl;
                ScriptManager->HandleCastError(e);
                // Make the actor keep on anyway.
                _DecideAction();
            }
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
                    ApplyActiveStatusEffect(i->first, GLOBAL_INTENSITY_NEG_MODERATE, 200000);
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

void BattleActor::_UpdateState()
{
    // Permits the actor to die even in pause mode,
    // so that the sprite fade out is properly done.
    if (_state == ACTOR_STATE_DYING) {
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

    // Don't update the state timer when the battle tells is to pause when in idle state.
    // Also don't elapse the status effect time when paused.
    BattleMode* BM = BattleMode::CurrentInstance();
    if (BM->AreActorStatesPaused() || BM->IsInSceneMode())
        return;

    if (IsAlive())
        _effects_supervisor->Update();

    // Don't update the state_timer if the character is hurt.
    if (_hurt_timer.IsRunning())
        return;

    switch(_state) {
    // Check the stun effect only when in idle, warm up or cool down state
    case ACTOR_STATE_IDLE:
    case ACTOR_STATE_WARM_UP:
    case ACTOR_STATE_COOL_DOWN:
        if (!_is_stunned)
            _state_timer.Update();
        break;
    default:
        _state_timer.Update();
        break;
    }
}

void BattleActor::Update()
{
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

    _UpdateState();

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

} // namespace private_battle

} // namespace vt_battle
