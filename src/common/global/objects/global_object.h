////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_OBJECT_HEADER__
#define __GLOBAL_OBJECT_HEADER__

#include "common/global/status_effects/status_effect_enums.h"

#include "engine/video/image.h"

#include "utils/ustring.h"

#include <memory>

namespace vt_script {
class ReadScriptDescriptor;
}

namespace vt_global
{

class GlobalObject;

/** \name GlobalObject Types
*** \brief Used for identification of different game object types
**/
enum GLOBAL_OBJECT {
    GLOBAL_OBJECT_INVALID     = -1,
    GLOBAL_OBJECT_ITEM        =  0,
    GLOBAL_OBJECT_WEAPON      =  1,
    GLOBAL_OBJECT_HEAD_ARMOR  =  2,
    GLOBAL_OBJECT_TORSO_ARMOR =  3,
    GLOBAL_OBJECT_ARM_ARMOR   =  4,
    GLOBAL_OBJECT_LEG_ARMOR   =  5,
    GLOBAL_OBJECT_SPIRIT      =  6,
    GLOBAL_OBJECT_TOTAL       =  7
};

/** \name Object ID Range Constants
*** These constants set the maximum valid ID ranges for each object category.
*** The full valid range for each object category ID is:
*** - Items:            1-10000
*** - Weapons:      10001-20000
*** - Head Armor:   20001-30000
*** - Torso Armor:  30001-40000
*** - Arm Armor:    40001-50000
*** - Leg Armor:    50001-60000
*** - Spirits:      60001-70000
*** - Key Items:    70001-80000 // Old, now any kind of item can be a key item.
**/
//@{
const uint32_t OBJECT_ID_INVALID   = 0;
const uint32_t MAX_ITEM_ID         = 10000;
const uint32_t MAX_WEAPON_ID       = 20000;
const uint32_t MAX_HEAD_ARMOR_ID   = 30000;
const uint32_t MAX_TORSO_ARMOR_ID  = 40000;
const uint32_t MAX_ARM_ARMOR_ID    = 50000;
const uint32_t MAX_LEG_ARMOR_ID    = 60000;
const uint32_t MAX_SPIRIT_ID       = 70000;
const uint32_t MAX_KEY_ITEM_ID     = 80000;
const uint32_t OBJECT_ID_EXCEEDS   = 80001;
//@}

/** \brief Creates a new GlobalObject and returns a pointer to it
*** \param id The id value of the object to create
*** \param count The count of the new object to create (default value == 1)
*** \return A pointer to the newly created GlobalObject, or nullptr if the object could not be created
***
*** This function does not actually create a GlobalObject (it can't since its an abstract class).
*** It creates one of the derived object class types depending on the value of the id argument.
**/
std::shared_ptr<GlobalObject> GlobalCreateNewObject(uint32_t id, uint32_t count = 1);

/** ****************************************************************************
*** \brief An abstract base class for representing a game object
***
*** All game objects inherit from this class. This allows objects of all types to
*** be stored in the same container (an inventory list for instance) and promotes
*** efficient code reuse for all game objects. The class is designed so that a
*** single class object can represent multiple instances of the same game object.
*** In other words, you can represent 50 healing potions with a single GlobalObject
*** class object rather than having to create and managed 50 class objects, one for
*** each potion. The _count member achieves this convenient function.
***
*** A GlobalObject with an ID value of zero is considered invalid. Most of the
*** protected members of this class can only be set by the constructors or methods
*** of deriving classes.
***
*** \note The price of an object is not actually the price it is bought or sold
*** at in the game. It is a "base price" from which all levels of buy and sell
*** prices are derived from.
*** ***************************************************************************/
class GlobalObject
{
public:
    GlobalObject() :
        _id(0),
        _is_key_item(false),
        _count(0),
        _price(0),
        _trade_price(0)
    {
    }

    explicit GlobalObject(uint32_t id, uint32_t count = 1) :
        _id(id),
        _is_key_item(false),
        _count(count),
        _price(0),
        _trade_price(0)
    {
    }

    virtual ~GlobalObject()
    {
    }

    //! \brief Returns true if the object is properly initialized and ready to be used
    bool IsValid() const {
        return (_id != 0);
    }

    //! \brief Returns true if the object is properly initialized and ready to be used
    bool IsKeyItem() const {
        return _is_key_item;
    }

    /** \brief Purely virtual function used to distinguish between object types
    *** \return A value that represents the type of object
    **/
    virtual GLOBAL_OBJECT GetObjectType() const = 0;

    /** \brief Increments the number of objects represented by this class
    *** \param count The count increment value (default value == 1)
    **/
    void IncrementCount(uint32_t count = 1) {
        _count += count;
    }

    /** \brief Decrements the number of objects represented by this class
    *** \param count The count decrement value (default value == 1)
    *** \note When the count reaches zero, this class object does <i>not</i> self-destruct. It is the user's
    *** responsiblity to check if the count becomes zero, and to destroy the object if it is appropriate to do so.
    **/
    void DecrementCount(uint32_t count = 1) {
        if(count > _count) _count = 0;
        else _count -= count;
    }

    //! \name Class Member Access Functions
    //@{
    uint32_t GetID() const {
        return _id;
    }

    const vt_utils::ustring &GetName() const {
        return _name;
    }

    const vt_utils::ustring &GetDescription() const {
        return _description;
    }

    void SetCount(uint32_t count) {
        _count = count;
    }

    uint32_t GetCount() const {
        return _count;
    }

    uint32_t GetPrice() const {
        return _price;
    }

    uint32_t GetTradingPrice() const {
        return _trade_price;
    }

    const std::vector<std::pair<uint32_t, uint32_t> >& GetTradeConditions() const {
        return _trade_conditions;
    }

    const vt_video::StillImage& GetIconImage() const {
        return _icon_image;
    }

    const std::vector<std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY> >& GetStatusEffects() const {
        return _status_effects;
    }
    //@}

protected:
    /** \brief An identification number for each unique item
    *** \note An ID number of zero indicates an invalid object
    **/
    uint32_t _id;

    //! \brief The name of the object as it would be displayed on a screen
    vt_utils::ustring _name;

    //! \brief A short description of the item to display on the screen
    vt_utils::ustring _description;

    //! \brief Tells whether an item is a key item, preventing from being consumed or sold.
    bool _is_key_item;

    //! \brief Retains how many occurences of the object are represented by this class object instance
    uint32_t _count;

    //! \brief The base price of the object for purchase/sale in the game
    uint32_t _price;

    //! \brief The additional price of the object requested when trading it.
    uint32_t _trade_price;

    //! \brief The trade conditions of the item <item_id, number>
    //! There is an exception: If the item_id is zero, the second value is the trade price.
    std::vector<std::pair<uint32_t, uint32_t> > _trade_conditions;

    //! \brief A loaded icon image of the object at its original size of 60x60 pixels
    vt_video::StillImage _icon_image;

    /** \brief Container that holds the intensity of each type of status effect of the object
    *** Effects with an intensity of GLOBAL_INTENSITY_NEUTRAL indicate no status effect bonus
    **/
    std::vector<std::pair<GLOBAL_STATUS, GLOBAL_INTENSITY> > _status_effects;

    //! \brief The skills that can be learned when equipping that piece of equipment.
    std::vector<uint32_t> _equipment_skills;

    //! \brief Causes the object to become invalid due to a loading error or other significant issue
    void _InvalidateObject() {
        _id = 0;
    }

    /** \brief Reads object data from an open script file
    *** \param script A reference to a script file that has been opened and prepared
    ***
    *** This method does not do any of its own error case checking. Only derived classes may call this
    *** protected function and they are expected to have the script file successfully opened and the correct
    *** table context prepared. This function will do nothing more but read the expected key/values of
    *** the open table in the script file and return.
    **/
    void _LoadObjectData(vt_script::ReadScriptDescriptor &script);

    //! \brief Loads status effects data
    void _LoadStatusEffects(vt_script::ReadScriptDescriptor &script);

    //! \brief Loads trading conditions data
    void _LoadTradeConditions(vt_script::ReadScriptDescriptor &script);

    //! \brief Loads the object linked skills (used by equipment only)
    void _LoadEquipmentSkills(vt_script::ReadScriptDescriptor &script);
}; // class GlobalObject

} // namespace vt_global

#endif // __GLOBAL_OBJECT_HEADER__
