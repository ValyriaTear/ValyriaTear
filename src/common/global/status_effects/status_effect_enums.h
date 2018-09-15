////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2016 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __STATUS_EFFECT_ENUMS_HEADER__
#define __STATUS_EFFECT_ENUMS_HEADER__

namespace vt_global
{

/** \name Elemental Effect Types
*** \brief Used to identify the eight different types of elementals
*** There are a total of four physical and four magical elemental effects
**/
enum GLOBAL_ELEMENTAL {
    GLOBAL_ELEMENTAL_INVALID    = -1,
    GLOBAL_ELEMENTAL_FIRE       =  0,
    GLOBAL_ELEMENTAL_WATER      =  1,
    GLOBAL_ELEMENTAL_VOLT       =  2,
    GLOBAL_ELEMENTAL_EARTH      =  3,
    GLOBAL_ELEMENTAL_LIFE       =  4,
    GLOBAL_ELEMENTAL_DEATH      =  5,
    GLOBAL_ELEMENTAL_NEUTRAL    =  6,
    GLOBAL_ELEMENTAL_TOTAL      =  7
};

/** \name Status Effect Types
*** \brief Used to identify the various types of status (and elemental) effects
**/
enum GLOBAL_STATUS {
    GLOBAL_STATUS_INVALID           = -1,
    GLOBAL_STATUS_PHYS_ATK          =  0,
    GLOBAL_STATUS_MAG_ATK           =  1,
    GLOBAL_STATUS_PHYS_DEF          =  2,
    GLOBAL_STATUS_MAG_DEF           =  3,
    GLOBAL_STATUS_STAMINA           =  4,
    GLOBAL_STATUS_EVADE             =  5,
    GLOBAL_STATUS_HP                =  6,
    GLOBAL_STATUS_SP                =  7,
    GLOBAL_STATUS_PARALYSIS         =  8,
    GLOBAL_STATUS_FIRE              =  9,
    GLOBAL_STATUS_WATER             = 10,
    GLOBAL_STATUS_VOLT              = 11,
    GLOBAL_STATUS_EARTH             = 12,
    GLOBAL_STATUS_LIFE              = 13,
    GLOBAL_STATUS_DEATH             = 14,
    GLOBAL_STATUS_NEUTRAL           = 15,
    GLOBAL_STATUS_TOTAL             = 16
};

/** \name Effect Intensity Levels
*** \brief Used to reflect the potency of elemental and status effects
*** There are nine valid intensity levels. Four negative, four positive, and one neutral.
*** The neutral intensity level essentially equates to "no effect".
**/
enum GLOBAL_INTENSITY {
    GLOBAL_INTENSITY_INVALID       = -5,
    GLOBAL_INTENSITY_NEG_EXTREME   = -4,
    GLOBAL_INTENSITY_NEG_GREATER   = -3,
    GLOBAL_INTENSITY_NEG_MODERATE  = -2,
    GLOBAL_INTENSITY_NEG_LESSER    = -1,
    GLOBAL_INTENSITY_NEUTRAL       =  0,
    GLOBAL_INTENSITY_POS_LESSER    =  1,
    GLOBAL_INTENSITY_POS_MODERATE  =  2,
    GLOBAL_INTENSITY_POS_GREATER   =  3,
    GLOBAL_INTENSITY_POS_EXTREME   =  4,
    GLOBAL_INTENSITY_TOTAL         =  5
};

} // namespace vt_global

#endif // __STATUS_EFFECT_ENUMS_HEADER__
