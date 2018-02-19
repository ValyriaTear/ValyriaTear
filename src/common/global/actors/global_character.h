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

#include "common/global/global_utils.h"

/** \brief Namespace which contains all binding functions
*** Contains the binding code which makes the C++ engine available to Lua
*** This method should <b>only be called once</b>. It must be called after the
*** ScriptEngine is initialized, otherwise the application will crash.
**/
//! FIXME: Remove this once the common bindings code doesn't modify private members
//! anymore. See common_bindings.cpp
namespace vt_defs
{
void BindCommonCode();
}

namespace vt_global
{

class GlobalArmor;
class GlobalWeapon;

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
    //! FIXME: Remove this once the common bindings code doesn't modify private members
    //! anymore. See common_bindings.cpp
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
    bool AddSkill(uint32_t skill_id) override {
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

    //! \brief The number of experience points that can be spent to obtain skill nodes
    uint32_t _experience_points;

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
    int32_t _experience_for_next_level; // TODO: Drop this using skill nodes

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
    //@} // TODO: Drop this using skill nodes

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
    std::vector<GlobalSkill*> _new_skills_learned; // TODO: Drop this using skill nodes

    //! \brief Stores the list of skill nodes already learned by the character
    std::vector<uint32_t> _obtained_skill_nodes;

    //! \brief Stores the current skill node id the character is located at,
    //! in the skill_graph
    uint32_t _current_skill_node_id;

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
