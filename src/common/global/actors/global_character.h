////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_CHARACTER_HEADER__
#define __GLOBAL_CHARACTER_HEADER__

#include "global_actor.h"

#include "common/global/objects/global_object.h"
#include "common/global/status_effects/global_active_effect.h"
#include "global_attack_point.h"

#include <memory>
#include <map>

namespace vt_script {
class ReadScriptDescriptor;
class WriteScriptDescriptor;
}

namespace vt_global
{

class GlobalArmor;
class GlobalWeapon;

/** \name Game Character IDs
*** \brief Integers that are used for identification of characters
*** These series of constants are used as bit-masks for determining things such as if the character
*** may use a certain item. Only one bit should be set for each character ID.
***
*** \note The IDs for each character are defined in the data/global.lua file.
**/
//@{
const uint32_t GLOBAL_CHARACTER_INVALID     = 0x00000000;
const uint32_t GLOBAL_CHARACTER_ALL         = 0xFFFFFFFF;
//@}

/** ****************************************************************************
*** \brief Represents a playable game character
***
*** This class represents playable game characters that join the party and can
*** participate in battles. It does not cover NPCs or any other form of character.
*** All characters have four attack points on the head, torso, arms, and legs.
*** Armor may also be equipped to cover all four of these points. This class
*** additionally retains references to loaded images of the character in various
*** formats such as sprites and portraits that are used across the different game modes.
*** ***************************************************************************/
class GlobalCharacter : public GlobalActor
{
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

    /** \brief Loads character data from a saved game file and id key.
    *** \param file A reference to the open and valid file from where to read the character from
    *** \returns Whether the character was successfully loaded.
    **/
    bool LoadCharacter(vt_script::ReadScriptDescriptor& file);

    /** \brief Writes character data to the saved game file
    *** \param file A reference to the open and valid file where to write the character data
    *** \returns Whether the character could successfully be saved to the save file.
    **/
    bool SaveCharacter(vt_script::WriteScriptDescriptor& file);

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
    virtual void SetPhysAtk(uint32_t base) override {
        _char_phys_atk.SetBase(base);
        _CalculateAttackRatings();
    }

    virtual void SetPhysAtkModifier(float mod) override {
        _char_phys_atk.SetModifier(mod);
        _CalculateAttackRatings();
    }

    virtual void SetMagAtk(uint32_t base) override {
        _char_mag_atk.SetBase(base);
        _CalculateAttackRatings();
    }

    virtual void SetMagAtkModifier(float mod) override {
        _char_mag_atk.SetModifier(mod);
        _CalculateAttackRatings();
    }

    virtual void SetPhysDef(uint32_t base) override {
        _char_phys_def.SetBase(base);
        _CalculateDefenseRatings();
    }

    virtual void SetPhysDefModifier(float mod) override {
        _char_phys_def.SetModifier(mod);
        _CalculateDefenseRatings();
    }

    virtual void SetMagDef(uint32_t pr) override {
        _char_mag_def.SetBase(pr);
        _CalculateDefenseRatings();
    }

    virtual void SetMagDefModifier(float mod) override {
        _char_mag_def.SetModifier(mod);
        _CalculateDefenseRatings();
    }

    virtual void AddPhysAtk(uint32_t amount) override;

    virtual void SubtractPhysAtk(uint32_t amount) override;

    virtual void AddMagAtk(uint32_t amount) override;

    virtual void SubtractMagAtk(uint32_t amount) override;

    virtual void AddPhysDef(uint32_t amount) override;

    virtual void SubtractPhysDef(uint32_t amount) override;

    virtual void AddMagDef(uint32_t amount) override;

    virtual void SubtractMagDef(uint32_t amount) override;

    /** \brief Adds experience points to the character
    *** \param xp The amount of experience points to add
    *** \return True if the new experience points triggered character level up
    **/
    bool AddExperiencePoints(uint32_t xp);

    //! \brief Equip the given piece of armor and return the previous armor shared_ptr if any.
    std::shared_ptr<GlobalArmor> EquipArmor(const std::shared_ptr<GlobalArmor>& armor);

    //! \brief Unequip the given piece of armor and return its shared_ptr if any was equipped.
    std::shared_ptr<GlobalArmor> UnequipArmor(GLOBAL_OBJECT object_type);

    std::shared_ptr<GlobalArmor> GetEquippedArmor(GLOBAL_OBJECT object_type) const;

    std::shared_ptr<GlobalWeapon> GetEquippedWeapon() const {
        return _equipped_weapon;
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
    bool AddSkill(uint32_t skill_id) override {
        return AddSkill(skill_id, true);
    }

    //! \brief Adds a new skill to the character, inherited from GlobalActor
    //! \param permanently Tells whether the skill is permanently learned.
    //! This is usually the case for skill learned when levelling, but not for skills
    //! available through equipment.
    //! \returns whether the skill was successfully added.
    bool AddSkill(uint32_t skill_id, bool permanently);

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

    uint32_t GetTotalExperiencePoints() const {
        return _total_experience_points;
    }

    void SetTotalExperiencePoints(uint32_t xp) {
        _total_experience_points = xp;
    }

    void SetUnspentExperiencePoints(uint32_t xp) {
        _unspent_experience_points = xp;
    }

    uint32_t GetUnspentExperiencePoints() const {
        return _unspent_experience_points;
    }

    void RemoveUnspentExperiencePoints(uint32_t xp) {
        if (xp >= _unspent_experience_points) {
            _unspent_experience_points = 0;
        }
        else {
            _unspent_experience_points -= xp;
        }
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

    //! \brief The permanent skills are saved between two game sessions.
    //! whereas the equipment skills are reloaded through equipment.
    std::vector<uint32_t>& GetPermanentSkills() {
        return _permanent_skills;
    }

    //! \brief Returns the skill nodes learned
    const std::vector<uint32_t>& GetObtainedSkillNodes() const {
        return _obtained_skill_nodes;
    }

    //! \brief reset the skill nodes learned
    void ResetObtainedSkillNodes() {
        _obtained_skill_nodes.clear();
    }

    //! \brief Set the skill nodes learned
    void SetObtainedSkillNodes(std::vector<uint32_t>& skill_nodes) {
        _obtained_skill_nodes.swap(skill_nodes);
    }

    //! \brief Adds a new skill nodes learned
    void AddObtainedSkillNode(uint32_t skill_node_id);

    //! \brief Tells whether the given skill node is obtained by the character
    bool IsSkillNodeObtained(uint32_t skill_node_id) const;

    //! \brief Set the skill nodes location
    void SetSkillNodeLocation(uint32_t skill_node_id) {
        _current_skill_node_id = skill_node_id;
    }

    //! \brief Get the skill nodes location
    uint32_t GetSkillNodeLocation() const {
        return _current_skill_node_id;
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

    //! \brief A vector storing only the skills that are permanently learned. This is useful when recomputing
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
    std::shared_ptr<GlobalWeapon> _equipped_weapon;

    /** \brief The various armors that the character has equipped
    *** \note The size of this vector will always be equal to the number of attack points on the character.
    ***
    *** Actors are not required to have armor of any sort equipped. Note that the defense bonuses that
    *** are afforded by the armors are not directly applied to the character's defense ratings, but rather to
    *** the defense ratings of their attack points. However the elemental and status bonuses of the armor are
    *** applied to the character as a whole. The armor must be equipped on one of the caracter's attack points to
    *** really afford any kind of defensive bonus.
    **/
    std::vector<std::shared_ptr<GlobalArmor>> _equipped_armors;

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

    //! \brief Updates the equipment status effects.
    void _UpdateEquipmentStatusEffects();

    //! \brief Recomputes which skills are available, based on equipment and permanent skills.
    void _UpdatesAvailableSkills();

private:
    //! \brief The amount of XP the character can spend to buy skill nodes
    uint32_t _unspent_experience_points;

    //! \brief The total (spent and unspent) amount of experience points
    uint32_t _total_experience_points;

    //! \brief The current experience level of the actor
    uint32_t _experience_level;

    //! \brief The remaining experience points required to reach the next experience level
    int32_t _experience_for_next_level;

    //! \brief Stores the XP needed for each next levels
    //! i.e. _xp_per_level[0] returns XP needed to reach lvl 2
    std::vector<int32_t> _xp_per_level;

    //! \brief Stores the list of skill nodes already learned by the character
    std::vector<uint32_t> _obtained_skill_nodes;

    //! \brief Stores the current skill node id the character is located at,
    //! in the skill_graph
    uint32_t _current_skill_node_id;

    //! \brief Returns the XP needed for next level based on character data
    int32_t _GetXPNeededForNextLevel() const;

    /** \brief Calculates an actor's physical and magical attack ratings
    *** This function sums the actor's phys_atk/mag_atk with their weapon's attack ratings
    *** and places the result in total physical/magical attack members
    **/
    virtual void _CalculateAttackRatings() override;

    //! \brief Calculates the physical and magical defense ratings for each attack point
    virtual void _CalculateDefenseRatings() override;

}; // class GlobalCharacter : public GlobalActor

} // namespace vt_global

#endif // __GLOBAL_CHARACTER_HEADER__
