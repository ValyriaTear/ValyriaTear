////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_actors.h
*** \author  Tyler Olsen, roots@allacrost.org
*** \author  Yohann Ferreira, yohann ferreira orange fr
*** \brief   Header file for global game actors
***
*** This file contains the implementation of "actors", which are living entities
*** in the game. Actors consist of playable characters and enemies in the game
*** that may participate in battles. Actors do not include NPCs found in towns or
*** other adversaries with which the player does not to battle with. This file
*** also contains classes that are closely related to the implementation of actors
*** such as attack targets and attack points.
*** ***************************************************************************/

#ifndef __GLOBAL_ACTORS_HEADER__
#define __GLOBAL_ACTORS_HEADER__

#include "global_utils.h"

#include "engine/video/image.h"

#include "utils/ustring.h"

/** \brief Namespace which contains all binding functions
*** Contains the binding code which makes the C++ engine available to Lua
*** This method should <b>only be called once</b>. It must be called after the
*** ScriptEngine is initialized, otherwise the application will crash.
**/
namespace vt_defs
{
void BindCommonCode();
}

namespace vt_script
{
class ReadScriptDescriptor;
}

namespace vt_global
{

class GlobalActor;
class GlobalArmor;
class GlobalCharacter;
class GlobalSkill;
class GlobalWeapon;

class GlobalStat {

public:
    GlobalStat():
        _base_value(0.0f),
        _modifier(1.0f),
        _final_value(0.0f)
    {}

    explicit GlobalStat(float value):
        _base_value(value),
        _modifier(1.0f),
        _final_value(value)
    {}

    void SetBase(float value) {
        _base_value = value;
        _ComputeFinalValue();
    }

    void SetModifier(float value) {
        _modifier = value;
        _ComputeFinalValue();
    }

    float GetValue() const {
        return _final_value;
    }

    float GetModifier() const {
        return _modifier;
    }

    float GetBase() const {
        return _base_value;
    }

private:

    float _base_value;

    float _modifier;

    float _final_value;

    void _ComputeFinalValue() {
        _final_value = _base_value * _modifier;
    }
}; // class GlobalStat

/** ****************************************************************************
*** \brief Represents the points of attack present on an actor
***
*** An attack point is a location where an actor may be attacked. It is <b>not</b> a numerical
*** quantity. Actors typically have multiple attack points, each with their own resistances and
*** weaknesses. For example, the number of attack points on all characters is four and they
*** are located on the head, torso, arms, and legs. Each attack points may have certain weaknesses
*** or resistances.
*** ***************************************************************************/
class GlobalAttackPoint
{
public:
    //! \param actor_owner A pointer to the GlobalActor owner of this attack point
    explicit GlobalAttackPoint(GlobalActor* owner);
    ~GlobalAttackPoint() {
        _actor_owner = nullptr;
    }

    /** \brief Reads in the attack point's data from a script file
    *** \param script A reference to the open script file where to retrieve the data from
    *** \return True upon success, false upon failure.
    ***
    *** There are two requirements for using this function. First, the script file must already
    *** be opened for reading permissions. Second, the table which contains the attack point data
    *** must be opened <b>prior</b> to making this function call. This function will not close the
    *** table containing the attack point when it finishes loading the data, so the calling routine
    *** must remember to close the table after this call is made.
    **/
    bool LoadData(vt_script::ReadScriptDescriptor& script);

    /** \brief Determines the total physical and magical defense of the attack point
    *** \param equipped_armor A pointer to the armor equipped on the attack point, or nullptr if no armor is equipped
    ***
    *** This method uses the owning GlobalActor's base defense stats, the attack point's defense modifiers stats,
    *** and the properties of the equipped armor to calculate the attack point's total physical and magical defense.
    *** This method should be called whenever the actor's base defense stats or equipped armor on this point changes.
    **/
    void CalculateTotalDefense(const std::shared_ptr<GlobalArmor>& equipped_armor);

    /** \brief Determines the total evade rating of the attack point
    ***
    *** This method uses the owning GlobalActor's base evade rating and the attack point's evade modifiers stats to
    *** calculate the attack point's total evade rating. This method should be called whenever the actor's base defense
    *** rating changes.
    **/
    void CalculateTotalEvade();

    //! \name Class Member Access Functions
    //@{
    vt_utils::ustring& GetName() {
        return _name;
    }

    GlobalActor* GetActorOwner() const {
        return _actor_owner;
    }

    int16_t GetXPosition() const {
        return _x_position;
    }

    int16_t GetYPosition() const {
        return _y_position;
    }

    float GetPhysDefModifier() const {
        return _phys_def_modifier;
    }

    float GetMagDefModifier() const {
        return _mag_def_modifier;
    }

    float GetEvadeModifier() const {
        return _evade_modifier;
    }

    uint16_t GetTotalPhysicalDefense() const {
        return _total_physical_defense;
    }

    uint16_t GetTotalMagicalDefense(GLOBAL_ELEMENTAL element) const {
        if (element <= GLOBAL_ELEMENTAL_INVALID || element >= GLOBAL_ELEMENTAL_TOTAL)
            element = GLOBAL_ELEMENTAL_NEUTRAL;

        return _total_magical_defense[element];
    }

    float GetTotalEvadeRating() const {
        return _total_evade_rating;
    }

    const std::vector<std::pair<GLOBAL_STATUS, float> >& GetStatusEffects() const {
        return _status_effects;
    }

    //! \note Use this method with extreme caution. It does not update defense/evade totals or any other members
    void SetActorOwner(GlobalActor* new_owner) {
        _actor_owner = new_owner;
    }
    //@}

private:
    /** \brief The name of the attack point as is displayed on the screen
    *** Usually, this is simply the name of a body part such as "head" or "tail". More elaborate names
    *** may be chosen for special foes and bosses, however.
    **/
    vt_utils::ustring _name;

    //! \brief A pointer to the actor which "owns" this attack point (i.e., the attack point is a location on the actor)
    GlobalActor* _actor_owner;

    /** \brief The position of the physical attack point relative to the actor's battle sprite
    *** These members treat the bottom center of the sprite as the origin (0, 0) and increase in the
    *** right and upwards directions. The combination of these two members point to the center pinpoint
    *** location of the attack point. The units of these two members are in number of pixels. The _y_position
    *** member should always be positive, by _x_position may be either positive or negative.
    **/
    int16_t _x_position;
    int16_t _y_position;

    /** \brief The defense and evasion percentage modifiers for this attack point
    ***
    *** These are called "modifiers" because they modify the value of phys_def, mag_def, and evade ratings of the
    *** actor. They represent percentage change from the base stat. So for example, a phys_def modifer that is 0.25f
    *** increases the phys_def of the attack point by 25%. If the base mag_def rating was 10 and the mag_def
    *** modifier was -0.30f, the resulting mag_def for the attack point would be: 10 + (10 * -0.30f) = 7.
    ***
    *** The lower bound for each modifier is -1.0f (-100%), which will result in a value of zero for that stat. No
    *** actor stats can be negative so even if the modifier drops below -1.0f, the resulting value will still be zero.
    *** There is no theoretical upper bound, but it is usually advised to keep it under 1.0f (100%).
    **/
    //@{
    float _phys_def_modifier;
    float _mag_def_modifier;
    float _evade_modifier;
    //@}

    /** \brief The cumulative defense and evade stats for this attack point
    *** These totals include the actor's base stat, the percentage modifier for the attack point, and the stats of any
    *** armor that is equipped on the attack point.
    **/
    //@{
    uint32_t _total_physical_defense;
    //! \brief The magical defense is computed against each elements.
    uint32_t _total_magical_defense[GLOBAL_ELEMENTAL_TOTAL];
    float _total_evade_rating;
    //@}

    /** \brief A vector containing all status effects that may be triggered by attacking the point
    *** This vector contains only the status effects that have a non-zero chance of affecting their target. Therefore,
    *** it is very possible that this vector may be empty, and usually it will contain only a single entry. The first
    *** element of the pair is an identifier for the type of status. The second element is a floating point value from
    *** 0.0 to 100.0, representing a 0-100% probability range for this effect to be triggered upon being hit.
    **/
    std::vector<std::pair<GLOBAL_STATUS, float> > _status_effects;
}; // class GlobalAttackPoint


/** ****************************************************************************
*** \brief Represents an actor that can participate in battles
***
*** This is an abstract parent class that both playable characters and enemies
*** inherit from in order to provide a consistent interface to the statistics
*** that characters and enemies share.
*** ***************************************************************************/
class GlobalActor
{
public:
    GlobalActor();
    virtual ~GlobalActor();

    GlobalActor(const GlobalActor &copy);
    GlobalActor &operator=(const GlobalActor &copy);

    /** \brief Adds a new skill to the actor's skill set
    *** \param skill_id The id number of the skill to add
    *** \return whether the skill addition succeeded.
    ***
    *** No skill may be added more than once. If this case is detected or an error occurs when trying
    *** to load the skill data, it will not be added.
    **/
    virtual bool AddSkill(uint32_t /*skill_id*/)
    { return false; }

    //! \brief Tells whether the actor has already learned the given skill.
    bool HasSkill(uint32_t skill_id);

    /** \brief Determines if the actor is "alive" and able to perform actions
    *** \return True if the character has a non-zero amount of hit points
    **/
    virtual bool IsAlive() const {
        return _hit_points != 0;
    }

    /** \name Class member get functions
    *** Some of these functions take an index argument to retrieve a particular
    *** attack point stat or piece of armor. If an invalid index is given, a zero
    *** or nullptr value will be returned.
    **/
    //@{
    uint32_t GetID() const {
        return _id;
    }

    vt_utils::ustring &GetName() {
        return _name;
    }

    const std::string &GetMapSpriteName() {
        return _map_sprite_name;
    }

    vt_video::StillImage &GetPortrait() {
        return _portrait;
    }

    vt_video::StillImage &GetFullPortrait() {
        return _full_portrait;
    }

    vt_video::StillImage &GetStaminaIcon() {
        return _stamina_icon;
    }

    uint32_t GetHitPoints() const {
        return _hit_points;
    }

    uint32_t GetMaxHitPoints() const {
        return _max_hit_points;
    }

    uint32_t GetSkillPoints() const {
        return _skill_points;
    }

    uint32_t GetMaxSkillPoints() const {
        return _max_skill_points;
    }

    uint32_t GetExperiencePoints() const {
        return _experience_points;
    }

    uint32_t GetPhysAtk() const {
        return (uint32_t)_char_phys_atk.GetValue();
    }

    float GetPhysAtkModifier() const {
        return _char_phys_atk.GetModifier();
    }

    uint32_t GetMagAtk() const {
        return (uint32_t)_char_mag_atk.GetValue();
    }

    float GetMagAtkModifier() const {
        return _char_mag_atk.GetModifier();
    }

    uint32_t GetPhysDef() const {
        return (uint32_t)_char_phys_def.GetValue();
    }

    float GetPhysDefModifier() const {
        return _char_phys_def.GetModifier();
    }

    uint32_t GetMagDef() const {
        return (uint32_t)_char_mag_def.GetValue();
    }

    float GetMagDefModifier() const {
        return _char_mag_def.GetModifier();
    }

    uint32_t GetStamina() const {
        return (uint32_t)_stamina.GetValue();
    }

    float GetStaminaModifier() const {
        return _stamina.GetModifier();
    }

    float GetEvade() const {
        return _evade.GetValue();
    }

    float GetEvadeModifier() const {
        return _evade.GetModifier();
    }

    uint32_t GetTotalPhysicalAttack() const {
        return _total_physical_attack;
    }

    uint32_t GetTotalMagicalAttack(GLOBAL_ELEMENTAL element) const {
        if (element <= GLOBAL_ELEMENTAL_INVALID ||
            element >= GLOBAL_ELEMENTAL_TOTAL)
            element = GLOBAL_ELEMENTAL_NEUTRAL;
        return _total_magical_attack[element];
    }

    uint32_t GetTotalPhysicalDefense(uint32_t index) const;

    //! \brief Get the magical defense of a body point against the given element.
    uint32_t GetTotalMagicalDefense(uint32_t index, GLOBAL_ELEMENTAL element) const;

    float GetTotalEvadeRating(uint32_t index) const;

    /** \brief Returns the average defense/evasion totals
    *** of all of the actor's attack points. It used against global attacks.
    **/
    uint32_t GetAverageDefense();
    uint32_t GetAverageMagicalDefense(GLOBAL_ELEMENTAL element);
    float GetAverageEvadeRating();

    const std::vector<GlobalAttackPoint *>& GetAttackPoints() {
        return _attack_points;
    }

    GlobalAttackPoint *GetAttackPoint(uint32_t index) const;

    const std::vector<GlobalSkill *>& GetSkills() {
        return _skills;
    }

    const std::string& GetDeathScriptFilename() const {
        return _death_script_filename;
    }

    const std::string& GetBattleAIScriptFilename() const {
        return _ai_script_filename;
    }
    //@}

    /** \name Class member set functions
    *** Normally you should not need to directly set the value of these members, but rather add or subtract
    *** an amount from the current value of the member. Total attack, defense, and evade ratings are
    *** re-calculated when an appropriately related stat is changed.
    **/
    //@{
    void SetExperiencePoints(uint32_t xp_points) {
        _experience_points = xp_points;
    }

    void SetHitPoints(uint32_t hp) {
        if(hp > _max_hit_points) _hit_points = _max_hit_points;
        else _hit_points = hp;
    }

    void SetMaxHitPoints(uint32_t hp) {
        _max_hit_points = hp;
        if(_hit_points > _max_hit_points) _hit_points = _max_hit_points;
    }

    void SetSkillPoints(uint32_t sp) {
        if(sp > _max_skill_points) _skill_points = _max_skill_points;
        else _skill_points = sp;
    }

    void SetMaxSkillPoints(uint32_t sp) {
        _max_skill_points = sp;
        if(_skill_points > _max_skill_points) _skill_points = _max_skill_points;
    }

    virtual void SetPhysAtk(uint32_t base) {
        _char_phys_atk.SetBase((float) base);
        _CalculateAttackRatings();
    }

    virtual void SetPhysAtkModifier(float mod) {
        _char_phys_atk.SetModifier(mod);
        _CalculateAttackRatings();
    }

    virtual void SetMagAtk(uint32_t base) {
        _char_mag_atk.SetBase((float) base);
        _CalculateAttackRatings();
    }

    virtual void SetMagAtkModifier(float mod) {
        _char_mag_atk.SetModifier(mod);
        _CalculateAttackRatings();
    }

    virtual void SetPhysDef(uint32_t base) {
        _char_phys_def.SetBase((float) base);
        _CalculateDefenseRatings();
    }

    virtual void SetPhysDefModifier(float mod) {
        _char_phys_def.SetModifier(mod);
        _CalculateDefenseRatings();
    }

    virtual void SetMagDef(uint32_t base) {
        _char_mag_def.SetBase((float) base);
        _CalculateDefenseRatings();
    }

    virtual void SetMagDefModifier(float mod) {
        _char_mag_def.SetModifier(mod);
        _CalculateDefenseRatings();
    }

    //! Made virtual to permit Battle Actors to recompute the idle state time.
    virtual void SetStamina(uint32_t base) {
        _stamina.SetBase((float) base);
    }

    virtual void SetStaminaModifier(float mod) {
        _stamina.SetModifier(mod);
    }

    virtual void SetEvade(float base) {
        _evade.SetBase(base);
        _CalculateEvadeRatings();
    }

    virtual void SetEvadeModifier(float mod) {
        _evade.SetModifier(mod);
        _CalculateEvadeRatings();
    }

    float GetElementalModifier(GLOBAL_ELEMENTAL element) const {
        if (element <= GLOBAL_ELEMENTAL_INVALID || element >= GLOBAL_ELEMENTAL_TOTAL)
            return 1.0f;
        return _elemental_modifier[element];
    }

    void SetElementalModifier(GLOBAL_ELEMENTAL element, float value) {
        if (element <= GLOBAL_ELEMENTAL_INVALID || element >= GLOBAL_ELEMENTAL_TOTAL)
            return;
        _elemental_modifier[element] = value;
        // Updates ratings
        _CalculateAttackRatings();
        _CalculateDefenseRatings();
    }
    //@}

    /** \name Class member add and subtract functions
    *** These methods provide a means to easily add or subtract amounts off of certain stats, such
    *** as hit points or stength. Total attack, defense, or evade ratings are re-calculated when
    *** an appropriately related stat is changed. Corner cases are checked to prevent overflow conditions
    *** and other invalid values, such as current hit points exceeded maximum hit points.
    ***
    *** \note When making changes to the maximum hit points or skill points, you should also consider
    *** making the same addition or subtraction to the current hit points / skill points. Modifying the
    *** maximum values will not modify the current values unless the change causes the new maximum to
    *** exceed the current values.
    **/
    //@{
    void AddHitPoints(uint32_t amount);

    void SubtractHitPoints(uint32_t amount);

    void AddMaxHitPoints(uint32_t amount);

    //! \note The number of hit points will be decreased if they are greater than the new maximum
    void SubtractMaxHitPoints(uint32_t amount);

    void AddSkillPoints(uint32_t amount);

    void SubtractSkillPoints(uint32_t amount);

    void AddMaxSkillPoints(uint32_t amount);

    //! \note The number of skill points will be decreased if they are greater than the new maximum
    void SubtractMaxSkillPoints(uint32_t amount);

    virtual void AddPhysAtk(uint32_t amount);

    virtual void SubtractPhysAtk(uint32_t amount);

    virtual void AddMagAtk(uint32_t amount);

    virtual void SubtractMagAtk(uint32_t amount);

    virtual void AddPhysDef(uint32_t amount);

    virtual void SubtractPhysDef(uint32_t amount);

    virtual void AddMagDef(uint32_t amount);

    virtual void SubtractMagDef(uint32_t amount);

    void AddStamina(uint32_t amount);

    void SubtractStamina(uint32_t amount);

    void AddEvade(float amount);

    void SubtractEvade(float amount);
    //@}

protected:
    //! \brief An identification number to represent the actor
    uint32_t _id;

    //! \brief The name of the actor as it will be displayed on the screen
    vt_utils::ustring _name;

    //! \brief Used to know the sprite linked to the character in map mode.
    std::string _map_sprite_name;

    //! \brief The character portrait
    vt_video::StillImage _portrait;

    //! \brief The character full pose portrait
    vt_video::StillImage _full_portrait;

    //! \brief The character stamina icon
    vt_video::StillImage _stamina_icon;

    //! \name Base Actor Statistics
    //@{
    //! \brief The number of experience points the actor has earned
    uint32_t _experience_points;

    //! \brief The current number of hit points that the actor has
    uint32_t _hit_points;

    //! \brief The maximum number of hit points that the actor may have
    uint32_t _max_hit_points;

    //! \brief The current number of skill points that the actor has
    uint32_t _skill_points;

    //! \brief The maximum number of skill points that the actor may have
    uint32_t _max_skill_points;

    //! \brief Used to determine the actor's physical attack rating
    GlobalStat _char_phys_atk;

    //! \brief Used to determine the actor's magical attack rating
    GlobalStat _char_mag_atk;

    //! \brief Used to determine the actor's physical defense rating
    GlobalStat _char_phys_def;

    //! \brief Used to determine the actor's magical defense rating
    GlobalStat _char_mag_def;

    //! \brief Used to calculate the time it takes to recover stamina in battles
    GlobalStat _stamina;

    //! \brief The attack evade percentage of the actor, ranged from 0.0 to 1.0
    GlobalStat _evade;
    //@}

    //! \brief The sum of the character's phys_atk and their weapon's physical attack
    uint32_t _total_physical_attack;

    //! \brief The sum of the character's mag_atk and their weapon's magical attack for each elements.
    uint32_t _total_magical_attack[GLOBAL_ELEMENTAL_TOTAL];

    //! \brief Tells the current mag atk/def stats modifier of the actor against each elemental.
    //! \note The modifier is multiplied to the current magical atk/def for the given elemental.
    std::vector<float> _elemental_modifier;

    /** \brief The attack points that are located on the actor
    *** \note All actors must have at least one attack point.
    **/
    std::vector<GlobalAttackPoint *> _attack_points;

    /** \brief A map containing all skills that the actor can use
    *** Unlike with characters, there is no need to hold the various types of skills in separate containers
    *** for enemies. An enemy must have <b>at least</b> one skill in order to do anything useful in battle.
    **/
    std::vector<GlobalSkill *> _skills;

    //! A vector keeping all the skills ids learned. Used for fast id requests.
    std::vector<uint32_t> _skills_id;

    //! \brief Stores the animation script filename used when the actor dies.
    std::string _death_script_filename;

    //! \brief Stores the battle AI script filename used when the actor is fighting.
    std::string _ai_script_filename;

    /** \brief Calculates an actor's physical and magical attack ratings
    *** This function sums the actor's phys_atk/mag_atk with their weapon's attack ratings
    *** and places the result in total physical/magical attack members
    **/
    virtual void _CalculateAttackRatings();

    //! \brief Calculates the physical and magical defense ratings for each attack point
    virtual void _CalculateDefenseRatings();

    //! \brief Calculates the evade rating for each attack point
    void _CalculateEvadeRatings();
}; // class GlobalActor


/** ****************************************************************************
*** \brief Represents a playable game character
***
*** This class represents playable game characters that join the party and can
*** participate in battles. It does not cover NPCs or any other form of character.
*** All characters have four attack points on the head, torso, arms, and legs.
*** Armor may also be equipped to cover all four of these points. This class
*** additionally retains references to loaded images of the character in various
*** formats such as sprites and portraits that are used across the different game modes.
***
*** Whenever a character gains additional experience points, there is a possibility that
*** growth may occur. Growth can occur even when the character has not reached a new experience
*** level, as the code allows for a gradual growth over time. A significant amount of growth should
*** always occur after achieving a new experience level.
***
*** The advised procedure for processing character growth is as follows.
*** -# Call AddExperiencePoints() to give the character additional XP.
*** -# If this method returns false, no further action is needed. Otherwise, growth has occurred and needs to be processed.
*** -# Call ReachedNewExperienceLevel() to determine whether the type growth is gradual or due to a
***    new experience level being reached.
*** -# If the growth type is gradual, call the various Get[STAT]Growth() methods and
***    report any non-zero values to the player. Then call AcknoledgeGrowth().
*** -# Otherwise if the growth type is a new level, report growth plus any skills
***    learned and call AcknoledgeGrowth() (*see note)
***
*** \note When an experience level is gained, after the call to AcknowledgeGrowth()
*** there may be new growth available (because the character gained multiple
*** experience levels or met the requirements for additional gradual growth for
*** the new experience level to gain). It is recommended practice to call AcknowledgeGrowth()
*** continuously until the fuction returns a false value, which indicates that no additional
*** growth is available.
***
*** \note When adding a large number of experience points to a character (at the end of a
*** battle for instance), it is advisable to add those points gradually over many calls in a
*** short period of time rather than all at once. Not only is it more aesthetically appealing to
*** the player to see their growth numbers add up in this way, but it also helps to mitigate issues
*** that may occur when a character earns so much experience that they gain more than one experience
*** level (an unlikely but not impossible scenario).
***
*** \todo This class needs a better organized set of containers for its images.
*** The current containers and accessor methods are considered temporary.
*** ***************************************************************************/
class GlobalCharacter : public GlobalActor
{
    friend void vt_defs::BindCommonCode();
    // TODO: investigate whether we can replace declaring the entire GameGlobal class as a friend with declaring
    // the GameGlobal::_SaveCharacter and GameGlobal::_LoadCharacter methods instead.
    friend class GameGlobal;
//     friend void GameGlobal::_SaveCharacter(vt_script::WriteScriptDescriptor &file, GlobalCharacter *character, bool last);
//     friend void GameGlobal::_LoadCharacter(vt_script::ReadScriptDescriptor &file, uint32_t id);
public:
    /** \brief Constructs a new character from its definition in a script file
    *** \param id The integer ID of the character to create
    *** \param initial If true, the character's stats, equipment, and skills are set
    *** to the character's initial status
    *** \note If initial is set to false, the character's stats, equipment, and skills
    *** must be set by external code, otherwise they will remain 0/nullptr/empty.
    **/
    explicit GlobalCharacter(uint32_t id, bool initial = true);
    virtual ~GlobalCharacter() override;

    //! \brief Tells whether a character is in the visible game formation
    void Enable(bool enable) {
        _enabled = enable;
    }

    bool IsEnabled() const {
        return _enabled;
    }

    void SetExperienceLevel(uint32_t xp_level) {
        _experience_level = xp_level;
    }

    uint32_t GetPhysAtkBase() const {
        return (uint32_t) _char_phys_atk.GetBase();
    }
    uint32_t GetMagAtkBase() const {
        return (uint32_t) _char_mag_atk.GetBase();
    }
    uint32_t GetPhysDefBase() const {
        return (uint32_t) _char_phys_def.GetBase();
    }
    uint32_t GetMagDefBase() const {
        return (uint32_t) _char_mag_def.GetBase();
    }
    uint32_t GetStaminaBase() const {
        return (uint32_t) _stamina.GetBase();
    }
    float GetEvadeBase() const {
        return _evade.GetBase();
    }

    // Character's stats changers, taking equipment in account
    virtual void SetPhysAtk(uint32_t base) {
        _char_phys_atk.SetBase(base);
        _CalculateAttackRatings();
    }

    virtual void SetPhysAtkModifier(float mod) {
        _char_phys_atk.SetModifier(mod);
        _CalculateAttackRatings();
    }

    virtual void SetMagAtk(uint32_t base) {
        _char_mag_atk.SetBase(base);
        _CalculateAttackRatings();
    }

    virtual void SetMagAtkModifier(float mod) {
        _char_mag_atk.SetModifier(mod);
        _CalculateAttackRatings();
    }

    virtual void SetPhysDef(uint32_t base) {
        _char_phys_def.SetBase(base);
        _CalculateDefenseRatings();
    }

    virtual void SetPhysDefModifier(float mod) {
        _char_phys_def.SetModifier(mod);
        _CalculateDefenseRatings();
    }

    virtual void SetMagDef(uint32_t pr) {
        _char_mag_def.SetBase(pr);
        _CalculateDefenseRatings();
    }

    virtual void SetMagDefModifier(float mod) {
        _char_mag_def.SetModifier(mod);
        _CalculateDefenseRatings();
    }

    virtual void AddPhysAtk(uint32_t amount);

    virtual void SubtractPhysAtk(uint32_t amount);

    virtual void AddMagAtk(uint32_t amount);

    virtual void SubtractMagAtk(uint32_t amount);

    virtual void AddPhysDef(uint32_t amount);

    virtual void SubtractPhysDef(uint32_t amount);

    virtual void AddMagDef(uint32_t amount);

    virtual void SubtractMagDef(uint32_t amount);

    /** \brief Adds experience points to the character
    *** \param xp The amount of experience points to add
    *** \return True if the new experience points triggered character growth
    **/
    bool AddExperiencePoints(uint32_t xp);

    std::shared_ptr<GlobalArmor> EquipHeadArmor(const std::shared_ptr<GlobalArmor>& armor) {
        return _EquipArmor(armor, GLOBAL_POSITION_HEAD);
    }

    std::shared_ptr<GlobalArmor> EquipTorsoArmor(const std::shared_ptr<GlobalArmor>& armor) {
        return _EquipArmor(armor, GLOBAL_POSITION_TORSO);
    }

    std::shared_ptr<GlobalArmor> EquipArmArmor(const std::shared_ptr<GlobalArmor>& armor) {
       return _EquipArmor(armor, GLOBAL_POSITION_ARMS);
    }

    std::shared_ptr<GlobalArmor> EquipLegArmor(const std::shared_ptr<GlobalArmor>& armor) {
        return _EquipArmor(armor, GLOBAL_POSITION_LEGS);
    }

    const std::vector<std::shared_ptr<GlobalArmor>>& GetArmorsEquipped() {
        return _armor_equipped;
    }

    std::shared_ptr<GlobalArmor> GetArmorEquipped(uint32_t index) const;

    std::shared_ptr<GlobalWeapon> GetWeaponEquipped() const {
        return _weapon_equipped;
    }

    /** \brief Equips a new weapon on the actor
    *** \param weapon The new weapon to equip on the actor
    *** \return A pointer to the weapon that was previouslly equipped, or nullptr if no weapon was equipped.
    ***
    *** This function will also automatically re-calculate all attack ratings, elemental, and status bonuses.
    **/
    std::shared_ptr<GlobalWeapon> EquipWeapon(const std::shared_ptr<GlobalWeapon>& weapon);

    //! \brief Tells whether the actor has got equipment.
    bool HasEquipment() const;

    //! \brief Permanently adds a new skill to the character (inherited from GlobalActor)
    //! \returns whether the skill was successfully added.
    bool AddSkill(uint32_t skill_id) {
        return AddSkill(skill_id, true);
    }

    //! \brief Adds a new skill to the character, inherited from GlobalActor
    //! \param permanently Tells whether the skill is permanently learned.
    //! This is usually the case for skill learned when levelling, but not for skills
    //! available through equipment.
    //! \returns whether the skill was successfully added.
    bool AddSkill(uint32_t skill_id, bool permanently);

    /** \brief Adds a new skill for the character to learn once the next experience level is gained
    *** \param skill_id The ID number of the skill to add
    *** \returns whether the skill was successfully added.
    *** \note This function is bound to Lua and used whenever a character gains a level.
    ***
    *** The difference between this method and AddSkill() is that the skill added is also copied to the
    *** _new_skills_learned container. This allows external code to easily know what skill or skills have
    *** been added to the character.
    **/
    bool AddNewSkillLearned(uint32_t skill_id);

    //! \brief Returns true if the character has earned enough experience points to reach the next level
    bool ReachedNewExperienceLevel() const
    { return _experience_for_next_level <= 0; }

    /** \brief Adds any growth that has occured by modifying the character's stats
    *** \return True if additional growth is detected and requires another AcknowledgeGrowth() call.
    ***
    *** If an experience level is gained, this function will open up the script file that contains
    *** the character's definition and get new growth stats for the next experience level. Often this
    *** requires another call to this function to process growth that has occurred after the level
    *** was gained.
    ***
    *** \note If multiple experience levels were gained as a result of adding a large amount of XP, this
    *** function will only increment the experience level by one. In the case where multiple levels are
    *** gained, this function will need to be called once for each level up.
    **/
    void AcknowledgeGrowth();

    //! \name Public Member Access Functions
    //@{
    uint32_t GetExperienceLevel() const {
        return _experience_level;
    }

    /** \note The reason why next level experience requirements are added and not simply set is so that any
    *** additional experience that was earned above the amount that was needed to achieve the next level will
    *** be factored in to reducing the amount of experience required for the next level. This is possible because
    *** the _experience_for_next_level member is allowed to become a negative value.
    **/
    void AddExperienceForNextLevel(uint32_t xp) {
        _experience_for_next_level += xp;
    }

    int32_t GetExperienceForNextLevel() const {
        return _experience_for_next_level;
    }

    std::shared_ptr<GlobalArmor> GetHeadArmorEquipped() {
        return _armor_equipped[GLOBAL_POSITION_HEAD];
    }

    std::shared_ptr<GlobalArmor> GetTorsoArmorEquipped() {
        return _armor_equipped[GLOBAL_POSITION_TORSO];
    }

    std::shared_ptr<GlobalArmor> GetArmArmorEquipped() {
        return _armor_equipped[GLOBAL_POSITION_ARMS];
    }

    std::shared_ptr<GlobalArmor> GetLegArmorEquipped() {
        return _armor_equipped[GLOBAL_POSITION_LEGS];
    }

    std::vector<GlobalSkill *>* GetWeaponSkills() {
        return &_weapon_skills;
    }

    //! \brief Gets the skills useable when the character hasn't got any weapon.
    std::vector<GlobalSkill *>* GetBareHandsSkills() {
        return &_bare_hands_skills;
    }

    std::vector<GlobalSkill *>* GetMagicSkills() {
        return &_magic_skills;
    }

    std::vector<GlobalSkill *>* GetSpecialSkills() {
        return &_special_skills;
    }

    //! The permanent skills are saved between two game sessions.
    //! whereas the equipment skills are reloaded through equipment.
    std::vector<uint32_t>& GetPermanentSkills() {
        return _permanent_skills;
    }

    const std::vector<GLOBAL_INTENSITY>& GetEquipementStatusEffects() const {
        return _equipment_status_effects;
    }

    //! Gets the currently active status effects on the global actor.
    const std::vector<ActiveStatusEffect>& GetActiveStatusEffects() const {
        return _active_status_effects;
    }

    //! Reset all the active status effects on the global actor.
    void ResetActiveStatusEffects() {
        _active_status_effects.clear();
        _active_status_effects.resize(GLOBAL_STATUS_TOTAL, ActiveStatusEffect());
    }

    //! Sets the given active status effect state on the global actor.
    void SetActiveStatusEffect(GLOBAL_STATUS status_effect, GLOBAL_INTENSITY intensity,
                               uint32_t duration, uint32_t elapsed_time) {
        _active_status_effects[status_effect] = ActiveStatusEffect(status_effect, intensity, duration, elapsed_time);
    }

    //! Sets a newly active status effect on the global actor, but taking in account a possible previous active one.
    void ApplyActiveStatusEffect(GLOBAL_STATUS status_effect, GLOBAL_INTENSITY intensity,
                                 uint32_t duration);

    //! \brief Tells the intensity of the active status effect currently applied on the character.
    vt_global::GLOBAL_INTENSITY GetActiveStatusEffectIntensity(vt_global::GLOBAL_STATUS status_effect) const {
        return _active_status_effects[status_effect].GetIntensity();
    }

    //! \brief Removes the given status effect.
    //! \note No scripted function is called.
    void RemoveActiveStatusEffect(GLOBAL_STATUS status_effect) {
        _active_status_effects[status_effect] = ActiveStatusEffect();
    }

    uint32_t GetHitPointsGrowth() const {
        return _hit_points_growth;
    }

    uint32_t GetSkillPointsGrowth() const {
        return _skill_points_growth;
    }

    uint32_t GetPhysAtkGrowth() const {
        return _phys_atk_growth;
    }

    uint32_t GetMagAtkGrowth() const {
        return _mag_atk_growth;
    }

    uint32_t GetPhysDefGrowth() const {
        return _phys_def_growth;
    }

    uint32_t GetMagDefGrowth() const {
        return _mag_def_growth;
    }

    uint32_t GetStaminaGrowth() const {
        return _stamina_growth;
    }

    float GetEvadeGrowth() const {
        return _evade_growth;
    }

    std::vector<GlobalSkill*>* GetNewSkillsLearned() {
        return &_new_skills_learned;
    }
    //@}

    //! Image accessor functions
    //@{
    //! \brief Returns the corresponding battle character sprite animation
    vt_video::AnimatedImage *RetrieveBattleAnimation(const std::string &name);

    //! \brief Returns the corresponding battle **weapon** sprite animation
    //! Usually displayed on top of the sprite to make it look like holding it.
    vt_video::AnimatedImage *RetrieveWeaponAnimation(const std::string &name);

    std::vector<vt_video::StillImage>* GetBattlePortraits() {
        return &_battle_portraits;
    }

    const vt_utils::ustring& GetSpecialCategoryName() const {
        return _special_category_name;
    }

    const std::string& GetSpecialCategoryIconFilename() const {
        return _special_category_icon;
    }
    //@}

protected:
    //! \brief The current experience level of the actor
    uint32_t _experience_level;

    /** \brief Sortable skill containers
    *** Skills are divided into three types: weapon, magic, and special. There is really no functional
    *** distinguishment between the various skill types, they just serve an organizational means and are
    *** used to identify a skill's general purpose/use. Characters keep their skills in these seperate
    *** containers because they are presented in this way to the player.
    **/
    //@{
    std::vector<GlobalSkill *> _weapon_skills;
    std::vector<GlobalSkill *> _magic_skills;
    std::vector<GlobalSkill *> _special_skills;
    // Skills available when no weapon is equipped.
    std::vector<GlobalSkill *> _bare_hands_skills;

    //! A vector storing only the skills that are permanently learned. This is useful when recomputing
    //! the available skills, on equip/unequip.
    std::vector<uint32_t> _permanent_skills;
    //@}

    //! \brief The script filename used to trigger a battle character animation when dealing with a particular skill.
    std::map<std::string, std::string> _skill_scripts;

    /** \name Character Images
    *** \note Although many of the names of these members would imply that they are only used in one particular
    *** mode of operation (map, battle, etc.), these members may be freely used by different game modes for
    *** which they were not specifically designed for. The names are simply meant to indicate the primary game
    *** mode where the images are intended to be used.
    **/
    //@{
    /** \brief The character's standard map portrait image
    *** The standard map portrait is ususally used in dialogues, but may also be used in other modes where
    *** appropriate.
    **/
    vt_video::StillImage _map_portrait_standard;

    /** \brief The frame images for the character's battle sprite.
    *** This map container contains various animated images for the character's battle sprites. The key to the
    *** map is a simple string which describes the animation, such as "idle".
    **/
    std::map<std::string, vt_video::AnimatedImage> _battle_animation;

    /** \brief The frame images for the character's battle portrait
    *** Each character has 5 battle portraits which represent the character's health with damage levels of 0%,
    *** 25%, 50%, 75%, and 100% (this is also the order in which the frames are stored, starting with the 0%
    *** frame at index 0). Thus, the size of this vector is always five elements. Each portait is 100x100
    *** pixels in size.
    **/
    std::vector<vt_video::StillImage> _battle_portraits;

    /** \brief The character's full-body portrait image for use in menu mode
    *** This image is a detailed, full-scale portait of the character and is intended for use in menu mode.
    *** The size of the image is 150x350 pixels.
    **/
    vt_video::StillImage _menu_portrait;
    //@}

    //! \brief The special skills category name and icon
    //! Used in battles to show the corresponding name and icon.
    vt_utils::ustring _special_category_name;
    std::string _special_category_icon;

    //! \brief Tells whether a character is in the visible game formation
    bool _enabled;

    /** \brief The weapon that the character has equipped
    *** \note If no weapon is equipped, this member will be equal to nullptr.
    ***
    *** Actors are not required to have weapons equipped, and indeed most enemies will probably not have any
    *** weapons explicitly equipped. The various bonuses to attack ratings, elemental attacks, and status
    *** attacks are automatically added to the appropriate members of this class when the weapon is equipped,
    *** and likewise those bonuses are removed when the weapon is unequipped.
    **/
    std::shared_ptr<GlobalWeapon> _weapon_equipped;

    /** \brief The various armors that the character has equipped
    *** \note The size of this vector will always be equal to the number of attack points on the character.
    ***
    *** Actors are not required to have armor of any sort equipped. Note that the defense bonuses that
    *** are afforded by the armors are not directly applied to the character's defense ratings, but rather to
    *** the defense ratings of their attack points. However the elemental and status bonuses of the armor are
    *** applied to the character as a whole. The armor must be equipped on one of the caracter's attack points to
    *** really afford any kind of defensive bonus.
    **/
    std::vector<std::shared_ptr<GlobalArmor>> _armor_equipped;

    /** \brief The status effects given by equipment, aka passive status effects.
    *** \note elemental effects are handled as status effects also.
    *** The vector is initialized with the size of GLOBAL_STATUS_TOTAL with
    *** GLOBAL_INTENSITY_NEUTRAL values. Those intensities corresponds to each
    *** status effect passive intensity.
    *** On equipping/unequipping, this vector should be updated, and his values
    *** applied on the character stats by calling the corresponding status effect function.
    **/
    std::vector<GLOBAL_INTENSITY> _equipment_status_effects;

    /** \brief Active status effects currently applied on the character.
    *** Active status effects are effects not applied through equipment, but rather through
    *** battle wounds, dungeon trap, potions from the menu, ...
    *** The given status effect vector is used to store active status effect data and pass it
    *** between game modes. Each mode is then responsible for properly updating, displaying and applying it.
    *** The vector is initialized with the size of GLOBAL_STATUS_TOTAL with empty status effects.
    **/
    std::vector<ActiveStatusEffect> _active_status_effects;

    /** \brief Equips a new armor on the character
    *** \param armor The piece of armor to equip
    *** \param index The index into the _armor_equipped vector where to equip the armor
    *** \return A pointer to the armor that was previously equipped, or nullptr if no armor was equipped
    ***
    *** This function will also automatically re-calculate all defense ratings, elemental, and status bonuses
    *** for the attack point that the armor was equipped on. If the index argument is invalid (out-of-bounds),
    *** the function will return the armor argument.
    **/
    std::shared_ptr<GlobalArmor> _EquipArmor(const std::shared_ptr<GlobalArmor>& armor, uint32_t index);

    //! \brief Updates the equipment status effects.
    void _UpdateEquipmentStatusEffects();

    //! \brief Recomputes which skills are available, based on equipment and permanent skills.
    void _UpdatesAvailableSkills();

private:
    /** \brief The remaining experience points required to reach the next experience level
    ***
    *** As a character earns experience points, the value held in this member decreases by the amount of XP learned. Whenever the value stored member
    *** zero or a negative number, the character has gained a new level. When a new level is achieved, the amount of experience required to then
    *** proceed another level is added to this member.
    ***
    *** \note The reason this member is a signed integer and not unsigned is so that it may be allowed to go negative. This makes the logic for processing
    *** experience level growth easier.
    ***
    **/
    int32_t _experience_for_next_level;

    /** \brief The amount of growth that should be added to each of the character's stats
    *** These members are incremented by the _ProcessPeriodicGrowth() function, which detects when a character
    *** has enough experience points to meet a growth requirement. They are all cleared to zero after
    *** a call to AcknowledgeGrowth().
    ***
    *** \note These members are given read/write access in Lua so that Lua may use them to hold new
    *** growth amounts when a character reaches a new level. Refer to the function DetermineLevelGrowth(character)
    *** defined in data/entities/characters.lua
    **/
    //@{
    uint32_t _hit_points_growth;
    uint32_t _skill_points_growth;
    uint32_t _phys_atk_growth;
    uint32_t _mag_atk_growth;
    uint32_t _phys_def_growth;
    uint32_t _mag_def_growth;
    uint32_t _stamina_growth;
    float _evade_growth;
    //@}

    /** \brief Contains pointers to all skills that were learned by achieving the current experience level
    ***
    *** This container will not contain skills learned if the character was constructed using their initial stats.
    *** The skills listed within this container have already been added to the character's active usable skill set.
    *** This container is cleared and reset after every level up. The most common use for this container is for
    *** external code to be able to show the player what skills have been learned upon their character reaching a
    *** new experience level.
    ***
    *** \note The pointers in this container are copies of the pointers contained within the _skills container. No
    *** memory management needs to be performed by this vector.
    ***
    *** \note An issue that needs to be considered is that if the character has an existing skill removed
    *** and that skill is also referenced by this container, the container will then point to an invalid memory
    *** location (assuming the GlobalSkill object that was removed was also deleted). Therefore, any skills that
    *** are removed from a character should also be removed from this container if they exist.
    **/
    std::vector<GlobalSkill*> _new_skills_learned;

    /** \brief Calculates an actor's physical and magical attack ratings
    *** This function sums the actor's phys_atk/mag_atk with their weapon's attack ratings
    *** and places the result in total physical/magical attack members
    **/
    virtual void _CalculateAttackRatings();

    //! \brief Calculates the physical and magical defense ratings for each attack point
    virtual void _CalculateDefenseRatings();

}; // class GlobalCharacter : public GlobalActor


/** ****************************************************************************
*** \brief Representation of enemies that fight in battles
***
*** The game handles enemies a little different than most RPGs. Instead of an
*** enemy always having the same statistics for health, phys_atk, etc., enemy
*** stats are randomized so that the same type of enemy does not always have
*** the exact same stats. Guassian random values are applied to each enemy's
*** "base" stats before the player begins battle with that enemy, making
*** the enemy tougher or weaker than the base level depending on the outcome. Some
*** enemies (notably bosses) do not have this randomization applied to their stats
*** in order to make sure that bosses are challenging, but not overwhemlingly strong
*** or surprisingly weak.
***
*** Enemies have one to several different skills that they may use in battle. An enemy
*** has to have at least one skill defined for it, otherwise they would not be able to
*** perform any action in battle. Enemy's may also carry a small chance of dropping an
*** item or other object after they are defeated.
*** ***************************************************************************/
class GlobalEnemy : public GlobalActor
{
public:
    explicit GlobalEnemy(uint32_t id);
    virtual ~GlobalEnemy() override
    {
    }

    /** \brief Enables the enemy to be able to use a specific skill
    *** \param skill_id The integer ID of the skill to add to the enemy
    *** \returns whether the skill was added successfully.
    ***
    *** This method should be called only <b>after</b> the Initialize() method has been invoked. The
    *** purpose of this method is to allow non-standard skills to be used by enemies under certain
    *** circumstances. For example, in scripted battle sequences where an enemy may become stronger
    *** and gain access to new skills after certain criteria are met. Normally you would want to define
    *** any skills that you wish an enemy to be able to use within their Lua definition file.
    **/
    bool AddSkill(uint32_t skill_id);

    /** \brief Uses random variables to calculate which objects, if any, the enemy dropped.
    **/
    std::vector<std::shared_ptr<GlobalObject>> DetermineDroppedObjects();

    //! \name Class member access functions
    //@{
    uint32_t GetDrunesDropped() const {
        return _drunes_dropped;
    }

    uint32_t GetSpriteWidth() const {
        return _sprite_width;
    }

    uint32_t GetSpriteHeight() const {
        return _sprite_height;
    }

    std::vector<vt_video::AnimatedImage>* GetBattleAnimations() {
        return &_battle_animations;
    }
    //@}

protected:
    //! \brief The dimensions of the enemy's battle sprite in pixels
    uint32_t _sprite_width, _sprite_height;

    //! \brief The amount of drunes that the enemy will drop
    uint32_t _drunes_dropped;

    /** \brief Dropped object containers
    *** These two vectors are of the same size. _dropped_objects contains the IDs of the objects that the enemy
    *** may drop. _dropped_chance contains a value from 0.0f to 1.0f that determines the probability of the
    *** enemy dropping that object.
    **/
    //@{
    std::vector<uint32_t> _dropped_objects;
    std::vector<float> _dropped_chance;
    //@}

    /** \brief Contains all of the possible skills that the enemy may possess
    *** This container holds the IDs of all skills that the enemy may execute in battle.
    *** The Initialize() function uses this data to populates the GlobalActor _skills container.
    **/
    std::vector<uint32_t> _skill_set;

    /** \brief The battle sprite animations for the enemy
    *** Each enemy has four animations representing damage levels of 0%, 33%, 66%, and 100%. This vector thus
    *** always has a size of four holding each of these image frames. The first element contains the 0%
    *** damage frame, the second element contains the 33% damage frame, and so on.
    **/
    std::vector<vt_video::AnimatedImage> _battle_animations;

    /** \brief Initializes the enemy stats and skills
    ***
    *** This function sets the enemy's experience level, modifies its stats using Gaussian
    *** random values, and constructs the skills that the enemy is capable of using.
    ***
    *** \note Certain enemies can skip the stat randomization step.
    **/
    void _Initialize();
}; // class GlobalEnemy : public GlobalActor


/** ****************************************************************************
*** \brief Represents a party of characters
***
*** This class is a container for a group or "party" of characters. A party is a type
*** of target for items and skills. The GameGlobal class also organizes characters
*** into parties for convienence. Note that an character may be either an enemy or
*** a character, but you should avoid creating parties that contain both characters
*** and enemies, as it can lead to conflicts. For example, a character and enemy which
*** enemy which have the same ID value).
***
*** Parties may or may not allow duplicate characters (a duplicate character is defined
*** as an character that has the same _id member as another character in the party).
*** This property is determined in the GlobalParty constructor
***
*** \note When this class is destroyed, the characters contained within the class are
*** <i>not</i> destroyed. Only the references to those characters through this class object
*** are lost.
***
*** \note All methods which perform an operation by using an character ID are
*** <b>only</b> valid to use if the party does not allow duplicates.
*** ***************************************************************************/
class GlobalParty
{
public:
    //! \param allow_duplicates Determines whether or not the party allows duplicate characters to be added (default value == false)
    explicit GlobalParty(bool allow_duplicates = false) :
        _allow_duplicates(allow_duplicates)
    {}

    ~GlobalParty()
    {}

    // ---------- Character addition, removal, and retrieval methods

    /** \brief Adds an character to the party
    *** \param character A pointer to the character to add to the party
    *** \param index The index where the character should be inserted. If negative, character is added to the end
    *** \note The character will not be added if it is already in the party and duplicates are not allowed
    **/
    void AddCharacter(GlobalCharacter *character, int32_t index = -1);

    /** \brief Removes an character from the party
    *** \param index The index of the character in the party to remove
    *** \return A pointer to the character that was removed, or nullptr if the index provided was invalid
    **/
    GlobalCharacter *RemoveCharacterAtIndex(uint32_t index);

    /** \brief Removes an character from the party
    *** \param id The id value of the character to remove
    *** \return A pointer to the character that was removed, or nullptr if the character was not found in the party
    **/
    GlobalCharacter *RemoveCharacterByID(uint32_t id);

    /** \brief Clears the party of all characters
    *** \note This function does not return the character pointers, so if you wish to get the
    *** GlobalCharacter make sure you do so prior to invoking this call.
    **/
    void RemoveAllCharacters() {
        _characters.clear();
    }

    /** \brief Retrieves a poitner to the character in the party at a specified index
    *** \param index The index where the character may be found in the party
    *** \return A pointer to the character at the specified index, or nullptr if the index argument was invalid
    **/
    GlobalCharacter *GetCharacterAtIndex(uint32_t index) const;

    /** \brief Retrieves a poitner to the character in the party with the spefified id
    *** \param id The id of the character to return
    *** \return A pointer to the character with the requested ID, or nullptr if the character was not found
    **/
    GlobalCharacter *GetCharacterByID(uint32_t id) const;

    // ---------- Character swap and replacement methods

    /** \brief Swaps the location of two characters in the party by their indeces
    *** \param first_index The index of the first character to swap
    *** \param second_index The index of the second character to swap
    **/
    void SwapCharactersByIndex(uint32_t first_index, uint32_t second_index);

    /** \brief Swaps the location of two characters in the party by looking up their IDs
    *** \param first_id The id of the first character to swap
    *** \param second_id The id of the second character to swap
    **/
    void SwapCharactersByID(uint32_t first_id, uint32_t second_id);

    /** \brief Replaces an character in the party at a specified index with a new character
    *** \param index The index of the character to be replaced
    *** \param new_character A pointer to the character that will replace the existing character
    *** \return A pointer to the replaced character, or nullptr if the operation did not take place
    **/
    GlobalCharacter *ReplaceCharacterByIndex(uint32_t index, GlobalCharacter *new_character);

    /** \brief Replaces an character in the party with the specified id with a new character
    *** \param id The id of the character to be replaced
    *** \param new_character A pointer to the character that will replace the existing character
    *** \return A pointer to the replaced character, or nullptr if the operation did not take place
    **/
    GlobalCharacter *ReplaceCharacterByID(uint32_t id, GlobalCharacter *new_character);

    // ---------- Other methods

    /** \brief Computes the average experience level of all characters in the party
    *** \return A float representing the average experience level (0.0f if party is empty)
    **/
    float AverageExperienceLevel() const;

    /** \brief Adds a certain amount of hit points to all characters in the party
    *** \param hp The number of health points to add
    **/
    void AddHitPoints(uint32_t hp);

    /** \brief Adds a certain amount of skill points to all characters in the party
    *** \param sp The number of skill points to add
    **/
    void AddSkillPoints(uint32_t sp);

    //! \name Class member accessor methods
    //@{
    bool IsAllowDuplicates() const {
        return _allow_duplicates;
    }

    bool IsPartyEmpty() const {
        return _characters.empty();
    }

    uint32_t GetPartySize() const {
        return _characters.size();
    }

    const std::vector<GlobalCharacter *>& GetAllCharacters() const {
        return _characters;
    }
    //@}

private:
    /** \brief Characters are allowed to be inserted into the party multiple times when this member is true
    *** \note The value of this member is set in the class constructor and can not be changed at a later time
    **/
    bool _allow_duplicates;

    /** \brief A container of characters that are in this party
    *** The GlobalCharacter objects pointed to by the elements in this vector are not managed by this class. Therefore
    *** one needs to be careful that if any of the GlobalCharacter objects are destroyed outside the context of this
    *** class, the character should be removed from this container immediately to avoid a possible segmentation fault.
    **/
    std::vector<GlobalCharacter *> _characters;
}; // class GlobalParty

} // namespace vt_global

#endif // __GLOBAL_ACTORS_HEADER__
