////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software and
// you may modify it and/or redistribute it under the terms of this license.
// See https://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#include "battle_enemy.h"

#include "modes/battle/battle.h"
#include "modes/battle/actions/item_action.h"
#include "modes/battle/status_effects/status_effects_supervisor.h"

#include "common/global/global.h"

#include "utils/utils_random.h"

using namespace vt_global;
using namespace vt_script;
using namespace vt_utils;
using namespace vt_video;

namespace vt_battle
{

namespace private_battle
{
//! \brief Used to make the hardcoded action movement more or less wide.
const uint32_t MOVEMENT_SIZE = 64;

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
}

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
