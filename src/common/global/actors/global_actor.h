////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_ACTOR_HEADER__
#define __GLOBAL_ACTOR_HEADER__

#include "global_stat.h"

#include "common/global/global_utils.h"

#include "utils/ustring.h"

namespace vt_global
{
class GlobalAttackPoint;
class GlobalSkill;

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

    uint32_t GetTotalExperiencePoints() const {
        return _total_experience_points;
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

    GlobalAttackPoint* GetAttackPoint(uint32_t index) const;

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
    void SetTotalExperiencePoints(uint32_t xp_points) {
        _total_experience_points = xp_points;
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
    //! \brief The number of total experience points the actor has earned
    //! TODO: To remove from here.
    uint32_t _total_experience_points;

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

} // namespace vt_global

#endif // __GLOBAL_ACTOR_HEADER__
