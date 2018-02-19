////////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2011 by The Allacrost Project
//            Copyright (C) 2012-2018 by Bertram (Valyria Tear)
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_STAT_HEADER__
#define __GLOBAL_STAT_HEADER__

namespace vt_global
{

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

} // namespace vt_global

#endif // __GLOBAL_STAT_HEADER__
