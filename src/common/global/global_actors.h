////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    global_actors.h
*** \author  Tyler Olsen, roots@allacrost.org
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

#include <map>
#include <deque>

/** \brief Namespace which contains all binding functions
*** Contains the binding code which makes the C++ engine available to Lua
*** This method should <b>only be called once</b>. It must be called after the
*** ScriptEngine is initialized, otherwise the application will crash.
**/
namespace hoa_defs
{
void BindCommonCode();
}

namespace hoa_script
{
class ReadScriptDescriptor;
}

namespace hoa_global
{

class GlobalActor;
class GlobalArmor;
class GlobalCharacter;
class GlobalSkill;
class GlobalWeapon;

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
    GlobalAttackPoint(GlobalActor *owner);

    ~GlobalAttackPoint() {
        _actor_owner = NULL;
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
    bool LoadData(hoa_script::ReadScriptDescriptor &script);

    /** \brief Determines the total physical and magical defense of the attack point
    *** \param equipped_armor A pointer to the armor equipped on the attack point, or NULL if no armor is equipped
    ***
    *** This method uses the owning GlobalActor's base defense stats, the attack point's defense modifiers stats,
    *** and the properties of the equipped armor to calculate the attack point's total physical and magical defense.
    *** This method should be called whenever the actor's base defense stats or equipped armor on this point changes.
    **/
    void CalculateTotalDefense(const GlobalArmor *equipped_armor);

    /** \brief Determines the total evade rating of the attack point
    ***
    *** This method uses the owning GlobalActor's base evade rating and the attack point's evade modifiers stats to
    *** calculate the attack point's total evade rating. This method should be called whenever the actor's base defense
    *** rating changes.
    **/
    void CalculateTotalEvade();

    //! \name Class Member Access Functions
    //@{
    hoa_utils::ustring &GetName() {
        return _name;
    }

    GlobalActor *GetActorOwner() const {
        return _actor_owner;
    }

    int16 GetXPosition() const {
        return _x_position;
    }

    int16 GetYPosition() const {
        return _y_position;
    }

    float GetFortitudeModifier() const {
        return _fortitude_modifier;
    }

    float GetProtectionModifier() const {
        return _protection_modifier;
    }

    float GetEvadeModifier() const {
        return _evade_modifier;
    }

    uint16 GetTotalPhysicalDefense() const {
        return _total_physical_defense;
    }

    uint16 GetTotalMagicalDefense() const {
        return _total_magical_defense;
    }

    float GetTotalEvadeRating() const {
        return _total_evade_rating;
    }

    const std::vector<std::pair<GLOBAL_STATUS, float> >& GetStatusEffects() const {
        return _status_effects;
    }

    //! \note Use this method with extreme caution. It does not update defense/evade totals or any other members
    void SetActorOwner(GlobalActor *new_owner) {
        _actor_owner = new_owner;
    }
    //@}

private:
    /** \brief The name of the attack point as is displayed on the screen
    *** Usually, this is simply the name of a body part such as "head" or "tail". More elaborate names
    *** may be chosen for special foes and bosses, however.
    **/
    hoa_utils::ustring _name;

    //! \brief A pointer to the actor which "owns" this attack point (i.e., the attack point is a location on the actor)
    GlobalActor *_actor_owner;

    /** \brief The position of the physical attack point relative to the actor's battle sprite
    *** These members treat the bottom center of the sprite as the origin (0, 0) and increase in the
    *** right and upwards directions. The combination of these two members point to the center pinpoint
    *** location of the attack point. The units of these two members are in number of pixels. The _y_position
    *** member should always be positive, by _x_position may be either positive or negative.
    **/
    int16 _x_position, _y_position;

    /** \brief The defense and evasion percentage modifiers for this attack point
    ***
    *** These are called "modifiers" because they modify the value of fortitude, protection, and evade ratings of the
    *** actor. They represent percentage change from the base stat. So for example, a fortitude modifer that is 0.25f
    *** increases the fortitude of the attack point by 25%. If the base protection rating was 10 and the protection
    *** modifier was -0.30f, the resulting protection for the attack point would be: 10 + (10 * -0.30f) = 7.
    ***
    *** The lower bound for each modifier is -1.0f (-100%), which will result in a value of zero for that stat. No
    *** actor stats can be negative so even if the modifier drops below -1.0f, the resulting value will still be zero.
    *** There is no theoretical upper bound, but it is usually advised to keep it under 1.0f (100%).
    **/
    //@{
    float _fortitude_modifier;
    float _protection_modifier;
    float _evade_modifier;
    //@}

    /** \brief The cumulative defense and evade stats for this attack point
    *** These totals include the actor's base stat, the percentage modifier for the attack point, and the stats of any
    *** armor that is equipped on the attack point.
    **/
    //@{
    uint16 _total_physical_defense;
    uint16 _total_magical_defense;
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

    /** \brief Equips a new weapon on the actor
    *** \param weapon The new weapon to equip on the actor
    *** \return A pointer to the weapon that was previouslly equipped, or NULL if no weapon was equipped.
    ***
    *** This function will also automatically re-calculate all attack ratings, elemental, and status bonuses.
    **/
    GlobalWeapon *EquipWeapon(GlobalWeapon *weapon);

    /** \brief Equips a new armor on the actor
    *** \param armor The piece of armor to equip
    *** \param index The index into the _armor_equippd vector where to equip the armor
    *** \return A pointer to the armor that was previously equipped, or NULL if no armor was equipped
    ***
    *** This function will also automatically re-calculate all defense ratings, elemental, and status bonuses
    *** for the attack point that the armor was equipped on. If the index argument is invalid (out-of-bounds),
    *** the function will return the armor argument.
    **/
    GlobalArmor *EquipArmor(GlobalArmor *armor, uint32 index);

    /** \brief Adds a new skill to the actor's skill set
    *** \param skill_id The id number of the skill to add
    ***
    *** No skill may be added more than once. If this case is detected or an error occurs when trying
    *** to load the skill data, it will not be added.
    **/
    virtual void AddSkill(uint32 skill_id) = 0;

    /** \brief Determines if the actor is "alive" and able to perform actions
    *** \return True if the character has a non-zero amount of hit points
    **/
    bool IsAlive() const {
        return (_hit_points != 0);
    }

    /** \name Class member get functions
    *** Some of these functions take an index argument to retrieve a particular
    *** attack point stat or piece of armor. If an invalid index is given, a zero
    *** or NULL value will be returned.
    **/
    //@{
    uint32 GetID() const {
        return _id;
    }

    hoa_utils::ustring &GetName() {
        return _name;
    }

    const std::string &GetMapSpriteName() {
        return _map_sprite_name;
    }

    hoa_video::StillImage &GetPortrait() {
        return _portrait;
    }

    hoa_video::StillImage &GetFullPortrait() {
        return _full_portrait;
    }

    hoa_video::StillImage &GetStaminaIcon() {
        return _stamina_icon;
    }

    uint32 GetHitPoints() const {
        return _hit_points;
    }

    uint32 GetMaxHitPoints() const {
        return _max_hit_points;
    }

    uint32 GetSkillPoints() const {
        return _skill_points;
    }

    uint32 GetMaxSkillPoints() const {
        return _max_skill_points;
    }

    uint32 GetExperienceLevel() const {
        return _experience_level;
    }

    uint32 GetExperiencePoints() const {
        return _experience_points;
    }

    uint32 GetStrength() const {
        return _strength;
    }

    uint32 GetVigor() const {
        return _vigor;
    }

    uint32 GetFortitude() const {
        return _fortitude;
    }

    uint32 GetProtection() const {
        return _protection;
    }

    uint32 GetAgility() const {
        return _agility;
    }

    float GetEvade() const {
        return _evade;
    }

    uint32 GetTotalPhysicalAttack() const {
        return _total_physical_attack;
    }

    uint32 GetTotalMagicalAttack() const {
        return _total_magical_attack;
    }

    uint32 GetTotalPhysicalDefense(uint32 index) const;

    uint32 GetTotalMagicalDefense(uint32 index) const;

    float GetTotalEvadeRating(uint32 index) const;

    GlobalWeapon *GetWeaponEquipped() const {
        return _weapon_equipped;
    }

    const std::vector<GlobalArmor *>& GetArmorEquipped() {
        return _armor_equipped;
    }

    GlobalArmor *GetArmorEquipped(uint32 index) const;

    const std::vector<GlobalAttackPoint *>& GetAttackPoints() {
        return _attack_points;
    }

    //! \brief Tells whether the actor has got equipment.
    bool HasEquipment() const;

    GlobalAttackPoint *GetAttackPoint(uint32 index) const;

    const std::map<uint32, GlobalSkill *>& GetSkills() {
        return _skills;
    }

    /** \brief Retrieves a pointer to a skill with a specific id
    *** \param skill_id The unique ID of the skill to find and return
    *** \return A pointer to the skill if it is found, or NULL if the skill was not found
    **/
    GlobalSkill *GetSkill(uint32 skill_id) const;

    //! \brief An alternative GetSkill call that takes a skill pointer as an argument
    GlobalSkill *GetSkill(const GlobalSkill *skill) const;

    // TODO: elemental and status effects not yet available in game
// 	std::vector<GlobalElementalEffect*>& GetElementalAttackBonuses()
// 		{ return _elemental_attack_bonuses; }
//
// 	std::vector<std::pair<float, GlobalStatusEffect*> >& GetStatusAttackBonuses()
// 		{ return _status_attack_bonuses; }
//
// 	std::vector<GlobalElementalEffect*>& GetElementalDefenseBonuses()
// 		{ return _elemental_defense_bonuses; }
//
// 	std::vector<std::pair<float, GlobalStatusEffect*> >& GetStatusDefenseBonuses()
// 		{ return _status_defense_bonuses; }
    //@}

    /** \name Class member set functions
    *** Normally you should not need to directly set the value of these members, but rather add or subtract
    *** an amount from the current value of the member. Total attack, defense, and evade ratings are
    *** re-calculated when an appropriately related stat is changed.
    **/
    //@{
    void SetExperienceLevel(uint32 xp_level) {
        _experience_level = xp_level;
    }

    void SetExperiencePoints(uint32 xp_points) {
        _experience_points = xp_points;
    }

    void SetHitPoints(uint32 hp) {
        if(hp > _max_hit_points) _hit_points = _max_hit_points;
        else _hit_points = hp;
    }

    void SetMaxHitPoints(uint32 hp) {
        _max_hit_points = hp;
        if(_hit_points > _max_hit_points) _hit_points = _max_hit_points;
    }

    void SetSkillPoints(uint32 sp) {
        if(sp > _max_skill_points) _skill_points = _max_skill_points;
        else _skill_points = sp;
    }

    void SetMaxSkillPoints(uint32 sp) {
        _max_skill_points = sp;
        if(_skill_points > _max_skill_points) _skill_points = _max_skill_points;
    }

    void SetStrength(uint32 st) {
        _strength = st;
        _CalculateAttackRatings();
    }

    void SetVigor(uint32 vi) {
        _vigor = vi;
        _CalculateAttackRatings();
    }

    void SetFortitude(uint32 fo) {
        _fortitude = fo;
        _CalculateDefenseRatings();
    }

    void SetProtection(uint32 pr) {
        _protection = pr;
        _CalculateDefenseRatings();
    }

    //! Made virtual to permit Battle Actors to recompute the idle state time.
    virtual void SetAgility(uint32 ag) {
        _agility = ag;
    }

    void SetEvade(float ev) {
        _evade = ev;
        _CalculateEvadeRatings();
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
    void AddHitPoints(uint32 amount);

    void SubtractHitPoints(uint32 amount);

    void AddMaxHitPoints(uint32 amount);

    //! \note The number of hit points will be decreased if they are greater than the new maximum
    void SubtractMaxHitPoints(uint32 amount);

    void AddSkillPoints(uint32 amount);

    void SubtractSkillPoints(uint32 amount);

    void AddMaxSkillPoints(uint32 amount);

    //! \note The number of skill points will be decreased if they are greater than the new maximum
    void SubtractMaxSkillPoints(uint32 amount);

    void AddStrength(uint32 amount);

    void SubtractStrength(uint32 amount);

    void AddVigor(uint32 amount);

    void SubtractVigor(uint32 amount);

    void AddFortitude(uint32 amount);

    void SubtractFortitude(uint32 amount);

    void AddProtection(uint32 amount);

    void SubtractProtection(uint32 amount);

    void AddAgility(uint32 amount);

    void SubtractAgility(uint32 amount);

    void AddEvade(float amount);

    void SubtractEvade(float amount);
    //@}

protected:
    //! \brief An identification number to represent the actor
    uint32 _id;

    //! \brief The name of the actor as it will be displayed on the screen
    hoa_utils::ustring _name;

    //! \brief Used to know the sprite linked to the character in map mode.
    std::string _map_sprite_name;

    //! \brief The character portrait
    hoa_video::StillImage _portrait;

    //! \brief The character full pose portrait
    hoa_video::StillImage _full_portrait;

    //! \brief The character stamina icon
    hoa_video::StillImage _stamina_icon;

    //! \name Base Actor Statistics
    //@{
    //! \brief The current experience level of the actor
    uint32 _experience_level;

    //! \brief The number of experience points the actor has earned
    uint32 _experience_points;

    //! \brief The current number of hit points that the actor has
    uint32 _hit_points;

    //! \brief The maximum number of hit points that the actor may have
    uint32 _max_hit_points;

    //! \brief The current number of skill points that the actor has
    uint32 _skill_points;

    //! \brief The maximum number of skill points that the actor may have
    uint32 _max_skill_points;

    //! \brief Used to determine the actor's physical attack rating
    uint32 _strength;

    //! \brief Used to determine the actor's magical attack rating
    uint32 _vigor;

    //! \brief Used to determine the actor's physical defense rating
    uint32 _fortitude;

    //! \brief Used to determine the actor's magical defense rating
    uint32 _protection;

    //! \brief Used to calculate the time it takes to recover stamina in battles
    uint32 _agility;

    //! \brief The attack evade percentage of the actor, ranged from 0.0 to 1.0
    float _evade;
    //@}

    //! \brief The sum of the character's strength and their weapon's physical attack
    uint32 _total_physical_attack;

    //! \brief The sum of the character's vigor and their weapon's magical attack
    uint32 _total_magical_attack;

    /** \brief The attack points that are located on the actor
    *** \note All actors must have at least one attack point.
    **/
    std::vector<GlobalAttackPoint *> _attack_points;

    /** \brief The weapon that the actor has equipped
    *** \note If no weapon is equipped, this member will be equal to NULL.
    ***
    *** Actors are not required to have weapons equipped, and indeed most enemies will probably not have any
    *** weapons explicitly equipped. The various bonuses to attack ratings, elemental attacks, and status
    *** attacks are automatically added to the appropriate members of this class when the weapon is equipped,
    *** and likewise those bonuses are removed when the weapon is unequipped.
    **/
    GlobalWeapon *_weapon_equipped;

    /** \brief The various armors that the actor has equipped
    *** \note The size of this vector will always be equal to the number of attack points on the actor.
    ***
    *** Actors are not required to have armor of any sort equipped. Note that the defense bonuses that
    *** are afforded by the armors are not directly applied to the character's defense ratings, but rather to
    *** the defense ratings of their attack points. However the elemental and status bonuses of the armor are
    *** applied to the character as a whole. The armor must be equipped on one of the actor's attack points to
    *** really afford any kind of defensive bonus.
    **/
    std::vector<GlobalArmor *> _armor_equipped;

    /** \brief A map containing all skills that the actor can use
    *** Unlike with characters, there is no need to hold the various types of skills in seperate containers
    *** for enemies. An enemy must have <b>at least</b> one skill in order to do anything useful in battle.
    **/
    std::map<uint32, GlobalSkill *> _skills;

    /** \brief The elemental effects added to the actor's attack
    *** Actors may carry various elemental attack bonuses, or they may carry none. These bonuses include
    *** those that are brought upon by the weapon that the character may have equipped.
    **/
// 	std::vector<GlobalElementalEffect*> _elemental_attack_bonuses;

    /** \brief The status effects added to the actor's attack
    *** Actors may carry various status attack bonuses, or they may carry none. These bonuses include
    *** those that are brought upon by the weapon that the character may have equipped. The first member
    *** in the pair is the likelihood (between 0.0 and 1.0) that the actor has of inflicting that status
    *** effect upon a targeted foe.
    **/
// 	std::vector<std::pair<float, GlobalStatusEffect*> > _status_attack_bonuses;

    /** \brief The elemental effects added to the actor's defense
    *** Actors may carry various elemental defense bonuses, or they may carry none. These bonuses include
    *** those that are brought upon by all of the armors that the character may have equipped.
    **/
// 	std::vector<GlobalElementalEffect*> _elemental_defense_bonuses;

    /** \brief The status effects added to the actor's defense
    *** Actors may carry various status defense bonuses, or they may carry none. These bonuses include
    *** those that are brought upon by the armors that the character may have equipped. The first member
    *** in the pair is the reduction in the likelihood (between 0.0 and 1.0) that the actor has of
    *** repelling an attack with a status effect.
    **/
// 	std::vector<std::pair<float, GlobalStatusEffect*> > _status_defense_bonuses;

    // ---------- Private methods

    /** \brief Calculates an actor's physical and magical attack ratings
    *** This function sums the actor's strength/vigor with their weapon's attack ratings
    *** and places the result in total physical/magical attack members
    **/
    void _CalculateAttackRatings();

    //! \brief Calculates the physical and magical defense ratings for each attack point
    void _CalculateDefenseRatings();

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
    friend void hoa_defs::BindCommonCode();
    // TODO: investigate whether we can replace declaring the entire GameGlobal class as a friend with declaring
    // the GameGlobal::_SaveCharacter and GameGlobal::_LoadCharacter methods instead.
    friend class GameGlobal;
//     friend void GameGlobal::_SaveCharacter(hoa_script::WriteScriptDescriptor &file, GlobalCharacter *character, bool last);
//     friend void GameGlobal::_LoadCharacter(hoa_script::ReadScriptDescriptor &file, uint32 id);
public:
    /** \brief Constructs a new character from its definition in a script file
    *** \param id The integer ID of the character to create
    *** \param initial If true, the character's stats, equipment, and skills are set
    *** to the character's initial status
    *** \note If initial is set to false, the character's stats, equipment, and skills
    *** must be set by external code, otherwise they will remain 0/NULL/empty.
    **/
    GlobalCharacter(uint32 id, bool initial = true);

    virtual ~GlobalCharacter()
        {}

    //! \brief Tells whether a character is in the visible game formation
    void Enable(bool enable) {
        _enabled = enable;
    }

    bool IsEnabled() const {
        return _enabled;
    }

    GlobalArmor *EquipHeadArmor(GlobalArmor *armor) {
        return EquipArmor(armor, GLOBAL_POSITION_HEAD);
    }

    GlobalArmor *EquipTorsoArmor(GlobalArmor *armor) {
        return EquipArmor(armor, GLOBAL_POSITION_TORSO);
    }

    GlobalArmor *EquipArmArmor(GlobalArmor *armor) {
        return EquipArmor(armor, GLOBAL_POSITION_ARMS);
    }

    GlobalArmor *EquipLegArmor(GlobalArmor *armor) {
        return EquipArmor(armor, GLOBAL_POSITION_LEGS);
    }

    /** \brief Adds experience points to the character
    *** \param xp The amount of experience points to add
    *** \return True if the new experience points triggered character growth
    **/
    bool AddExperiencePoints(uint32 xp);

    //! \brief Adds a new skill to the character, inherited from GlobalActor
    void AddSkill(uint32 skill_id);

    /** \brief Adds a new skill for the character to learn once the next experience level is gained
    *** \param skill_id The ID number of the skill to add
    *** \note This function is bound to Lua and used whenever a character gains a level.
    ***
    *** The difference between this method and AddSkill() is that the skill added is also copied to the
    *** _new_skills_learned container. This allows external code to easily know what skill or skills have
    *** been added to the character.
    **/
    void AddNewSkillLearned(uint32 skill_id);

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
    /** \note The reason why next level experience requirements are added and not simply set is so that any
    *** additional experience that was earned above the amount that was needed to achieve the next level will
    *** be factored in to reducing the amount of experience required for the next level. This is possible because
    *** the _experience_for_next_level member is allowed to become a negative value.
    **/
    void AddExperienceForNextLevel(uint32 xp) {
        _experience_for_next_level += xp;
    }

    int32 GetExperienceForNextLevel() const {
        return _experience_for_next_level;
    }

    GlobalArmor *GetHeadArmorEquipped() {
        return _armor_equipped[GLOBAL_POSITION_HEAD];
    }

    GlobalArmor *GetTorsoArmorEquipped() {
        return _armor_equipped[GLOBAL_POSITION_TORSO];
    }

    GlobalArmor *GetArmArmorEquipped() {
        return _armor_equipped[GLOBAL_POSITION_ARMS];
    }

    GlobalArmor *GetLegArmorEquipped() {
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

    uint32 GetHitPointsGrowth() const {
        return _hit_points_growth;
    }

    uint32 GetSkillPointsGrowth() const {
        return _skill_points_growth;
    }

    uint32 GetStrengthGrowth() const {
        return _strength_growth;
    }

    uint32 GetVigorGrowth() const {
        return _vigor_growth;
    }

    uint32 GetFortitudeGrowth() const {
        return _fortitude_growth;
    }

    uint32 GetProtectionGrowth() const {
        return _protection_growth;
    }

    uint32 GetAgilityGrowth() const {
        return _agility_growth;
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
    hoa_video::AnimatedImage *RetrieveBattleAnimation(const std::string &name);

    //! \brief Returns the corresponding battle **weapon** sprite animation
    //! Usually displayed on top of the sprite to make it look like holding it.
    hoa_video::AnimatedImage *RetrieveWeaponAnimation(const std::string &name);

    std::vector<hoa_video::StillImage>* GetBattlePortraits() {
        return &_battle_portraits;
    }

    const hoa_utils::ustring& GetSpecialCategoryName() const {
        return _special_category_name;
    }

    const std::string& GetSpecialCategoryIconFilename() const {
        return _special_category_icon;
    }
    //@}

protected:
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
    // Skills available when no weapons
    std::vector<GlobalSkill *> _bare_hands_skills;
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
    hoa_video::StillImage _map_portrait_standard;

    /** \brief The frame images for the character's battle sprite.
    *** This map container contains various animated images for the character's battle sprites. The key to the
    *** map is a simple string which describes the animation, such as "idle".
    **/
    std::map<std::string, hoa_video::AnimatedImage> _battle_animation;

    /** \brief The frame images for the character's battle portrait
    *** Each character has 5 battle portraits which represent the character's health with damage levels of 0%,
    *** 25%, 50%, 75%, and 100% (this is also the order in which the frames are stored, starting with the 0%
    *** frame at index 0). Thus, the size of this vector is always five elements. Each portait is 100x100
    *** pixels in size.
    **/
    std::vector<hoa_video::StillImage> _battle_portraits;

    /** \brief The character's full-body portrait image for use in menu mode
    *** This image is a detailed, full-scale portait of the character and is intended for use in menu mode.
    *** The size of the image is 150x350 pixels.
    **/
    hoa_video::StillImage _menu_portrait;
    //@}

    //! \brief The special skills category name and icon
    //! Used in battles to show the corresponding name and icon.
    hoa_utils::ustring _special_category_name;
    std::string _special_category_icon;

    //! \brief Tells whether a character is in the visible game formation
    bool _enabled;

    //! \brief Add a skill available only when the character has got no weapons.
    void _AddBareHandsSkill(uint32 skill_id);

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
    int32 _experience_for_next_level;

    /** \brief The amount of growth that should be added to each of the character's stats
    *** These members are incremented by the _ProcessPeriodicGrowth() function, which detects when a character
    *** has enough experience points to meet a growth requirement. They are all cleared to zero after
    *** a call to AcknowledgeGrowth().
    ***
    *** \note These members are given read/write access in Lua so that Lua may use them to hold new
    *** growth amounts when a character reaches a new level. Refer to the function DetermineLevelGrowth(character)
    *** defined in dat/actors/characters.lua
    **/
    //@{
    uint32 _hit_points_growth;
    uint32 _skill_points_growth;
    uint32 _strength_growth;
    uint32 _vigor_growth;
    uint32 _fortitude_growth;
    uint32 _protection_growth;
    uint32 _agility_growth;
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

}; // class GlobalCharacter : public GlobalActor


/** ****************************************************************************
*** \brief Representation of enemies that fight in battles
***
*** The game handles enemies a little different than most RPGs. Instead of an
*** enemy always having the same statistics for health, strength, etc., enemy
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
    GlobalEnemy(uint32 id);

    virtual ~GlobalEnemy()
    {}

    /** \brief Initializes the enemy and prepares it for battle
    ***
    *** This function sets the enemy's experience level, modifies its stats using Guassian
    *** random values, and constructs the skills that the enemy is capable of using. Call this
    *** function once only, because after the enemy has skills enabled it will not be able to
    *** re-initialize. If you need to initialize the enemy once more, you'll have to create a
    *** brand new GlobalEnemy object and initialize that instead.
    ***
    *** \note Certain enemies will skip the stat modification step.
    **/
    void Initialize();

    /** \brief Enables the enemy to be able to use a specific skill
    *** \param skill_id The integer ID of the skill to add to the enemy
    ***
    *** This method should be called only <b>after</b> the Initialize() method has been invoked. The
    *** purpose of this method is to allow non-standard skills to be used by enemies under certain
    *** circumstances. For example, in scripted battle sequences where an enemy may become stronger
    *** and gain access to new skills after certain criteria are met. Normally you would want to define
    *** any skills that you wish an enemy to be able to use within their Lua definition file.
    **/
    void AddSkill(uint32 skill_id);

    /** \brief Uses random variables to calculate which objects, if any, the enemy dropped
    *** \param objects A reference to a vector to hold the GlobalObject pointers
    ***
    *** The objects vector is cleared immediately once this function is called so make sure
    *** that it does not hold anything meaningful. Any objects which are added to this
    *** vector are created with new GlobalObject() and it becomes the callee's repsonsibility
    *** to manage this memory and delete those objects when they are no longer needed.
    **/
    void DetermineDroppedObjects(std::vector<GlobalObject *>& objects);

    //! \name Class member access functions
    //@{
    uint32 GetDrunesDropped() const {
        return _drunes_dropped;
    }

    uint32 GetSpriteWidth() const {
        return _sprite_width;
    }

    uint32 GetSpriteHeight() const {
        return _sprite_height;
    }

    std::vector<hoa_video::StillImage>* GetBattleSpriteFrames() {
        return &_battle_sprite_frames;
    }
    //@}

protected:
    //! \brief If set to true, when initialized the enemy will not randomize its statistic values
    bool _no_stat_randomization;

    //! \brief The dimensions of the enemy's battle sprite in pixels
    uint32 _sprite_width, _sprite_height;

    //! \brief The amount of drunes that the enemy will drop
    uint32 _drunes_dropped;

    /** \brief Dropped object containers
    *** These two vectors are of the same size. _dropped_objects contains the IDs of the objects that the enemy
    *** may drop. _dropped_chance contains a value from 0.0f to 1.0f that determines the probability of the
    *** enemy dropping that object.
    **/
    //@{
    std::vector<uint32> _dropped_objects;
    std::vector<float> _dropped_chance;
    //@}

    /** \brief Contains all of the possible skills that the enemy may possess
    *** This container holds the IDs of all skills that the enemy may execute in battle.
    *** The Initialize() function uses this data to populates the GlobalActor _skills container.
    **/
    std::vector<uint32> _skill_set;

    /** \brief The battle sprite frame images for the enemy
    *** Each enemy has four frames representing damage levels of 0%, 33%, 66%, and 100%. This vector thus
    *** always has a size of four holding each of these image frames. The first element contains the 0%
    *** damage frame, the second element contains the 33% damage frame, and so on.
    **/
    std::vector<hoa_video::StillImage> _battle_sprite_frames;
}; // class GlobalEnemy : public GlobalActor


/** ****************************************************************************
*** \brief Represents a party of actors
***
*** This class is a container for a group or "party" of actors. A party is a type
*** of target for items and skills. The GameGlobal class also organizes characters
*** into parties for convienence. Note that an actor may be either an enemy or
*** a character, but you should avoid creating parties that contain both characters
*** and enemies, as it can lead to conflicts. For example, a character and enemy which
*** enemy which have the same ID value).
***
*** Parties may or may not allow duplicate actors (a duplicate actor is defined
*** as an actor that has the same _id member as another actor in the party).
*** This property is determined in the GlobalParty constructor
***
*** \note When this class is destroyed, the actors contained within the class are
*** <i>not</i> destroyed. Only the references to those actors through this class object
*** are lost.
***
*** \note All methods which perform an operation by using an actor ID are
*** <b>only</b> valid to use if the party does not allow duplicates.
*** ***************************************************************************/
class GlobalParty
{
public:
    //! \param allow_duplicates Determines whether or not the party allows duplicate actors to be added (default value == false)
    GlobalParty(bool allow_duplicates = false) :
        _allow_duplicates(allow_duplicates) {}

    ~GlobalParty()
    {}

    // ---------- Actor addition, removal, and retrieval methods

    /** \brief Adds an actor to the party
    *** \param actor A pointer to the actor to add to the party
    *** \param index The index where the actor should be inserted. If negative, actor is added to the end
    *** \note The actor will not be added if it is already in the party and duplicates are not allowed
    **/
    void AddActor(GlobalActor *actor, int32 index = -1);

    /** \brief Removes an actor from the party
    *** \param index The index of the actor in the party to remove
    *** \return A pointer to the actor that was removed, or NULL if the index provided was invalid
    **/
    GlobalActor *RemoveActorAtIndex(uint32 index);

    /** \brief Removes an actor from the party
    *** \param id The id value of the actor to remove
    *** \return A pointer to the actor that was removed, or NULL if the actor was not found in the party
    **/
    GlobalActor *RemoveActorByID(uint32 id);

    /** \brief Clears the party of all actors
    *** \note This function does not return the actor pointers, so if you wish to get the
    *** GlobalActors make sure you do so prior to invoking this call.
    **/
    void RemoveAllActors() {
        _actors.clear();
    }

    /** \brief Retrieves a poitner to the actor in the party at a specified index
    *** \param index The index where the actor may be found in the party
    *** \return A pointer to the actor at the specified index, or NULL if the index argument was invalid
    **/
    GlobalActor *GetActorAtIndex(uint32 index) const;

    /** \brief Retrieves a poitner to the actor in the party with the spefified id
    *** \param id The id of the actor to return
    *** \return A pointer to the actor with the requested ID, or NULL if the actor was not found
    **/
    GlobalActor *GetActorByID(uint32 id) const;

    // ---------- Actor swap and replacement methods

    /** \brief Swaps the location of two actors in the party by their indeces
    *** \param first_index The index of the first actor to swap
    *** \param second_index The index of the second actor to swap
    **/
    void SwapActorsByIndex(uint32 first_index, uint32 second_index);

    /** \brief Swaps the location of two actors in the party by looking up their IDs
    *** \param first_id The id of the first actor to swap
    *** \param second_id The id of the second actor to swap
    **/
    void SwapActorsByID(uint32 first_id, uint32 second_id);

    /** \brief Replaces an actor in the party at a specified index with a new actor
    *** \param index The index of the actor to be replaced
    *** \param new_actor A pointer to the actor that will replace the existing actor
    *** \return A pointer to the replaced actor, or NULL if the operation did not take place
    **/
    GlobalActor *ReplaceActorByIndex(uint32 index, GlobalActor *new_actor);

    /** \brief Replaces an actor in the party with the specified id with a new actor
    *** \param id The id of the actor to be replaced
    *** \param new_actor A pointer to the actor that will replace the existing actor
    *** \return A pointer to the replaced actor, or NULL if the operation did not take place
    **/
    GlobalActor *ReplaceActorByID(uint32 id, GlobalActor *new_actor);

    // ---------- Other methods

    /** \brief Computes the average experience level of all actors in the party
    *** \return A float representing the average experience level (0.0f if party is empty)
    **/
    float AverageExperienceLevel() const;

    /** \brief Adds a certain amount of hit points to all actors in the party
    *** \param hp The number of health points to add
    **/
    void AddHitPoints(uint32 hp);

    /** \brief Adds a certain amount of skill points to all actors in the party
    *** \param sp The number of skill points to add
    **/
    void AddSkillPoints(uint32 sp);

    //! \name Class member accessor methods
    //@{
    bool IsAllowDuplicates() const {
        return _allow_duplicates;
    }

    bool IsPartyEmpty() const {
        return (_actors.size() == 0);
    }

    uint32 GetPartySize() const {
        return _actors.size();
    }

    const std::vector<GlobalActor *>& GetAllActors() const {
        return _actors;
    }
    //@}

private:
    /** \brief Actors are allowed to be inserted into the party multiple times when this member is true
    *** \note The value of this member is set in the class constructor and can not be changed at a later time
    **/
    bool _allow_duplicates;

    /** \brief A container of actors that are in this party
    *** The GlobalActor objects pointed to by the elements in this vector are not managed by this class. Therefore
    *** one needs to be careful that if any of the GlobalActor objects are destroyed outside the context of this
    *** class, the actor should be removed from this container immediately to avoid a possible segmentation fault.
    **/
    std::vector<GlobalActor *> _actors;
}; // class GlobalParty

} // namespace hoa_global

#endif // __GLOBAL_ACTORS_HEADER__
