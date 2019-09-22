////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_SPIRIT_HEADER__
#define __GLOBAL_SPIRIT_HEADER__

#include "global_object.h"

namespace vt_global
{

/** ****************************************************************************
*** \brief Represents any type of spirit that can be attached to weapons and armor
***
*** Spirits are small gems or stones that can be placed into sockets available on
*** weapons and armor. Spirits have the ability to enhance the properties of
*** equipment it is attached to, allowing the player a degree of customization
*** in the weapons and armor that their character use.
*** ***************************************************************************/
class GlobalSpirit : public GlobalObject
{
public:
    explicit GlobalSpirit(uint32_t id, uint32_t count = 1);
    virtual ~GlobalSpirit() override
    {
    }

    GLOBAL_OBJECT GetObjectType() const override {
        return GLOBAL_OBJECT_SPIRIT;
    }
}; // class GlobalSpirit : public GlobalObject

} // namespace vt_global

#endif // __GLOBAL_SPIRIT_HEADER__
