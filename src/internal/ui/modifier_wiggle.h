#pragma once

#include "mkb/mkb.h"
#include "modifier.h"
#include "widget.h"

namespace ui {

// Make the widget rotate around its centerpoint.
class WiggleModifier : public Modifier {
private:
    u16 m_angle;
    u16 m_period;

public:
    // Angle is in standard mkb s16 angle format, period is in seconds
    // Max period is 60 seconds
    WiggleModifier(s16 angle, s16 period) : m_angle(angle), m_period(static_cast<u16>(1092 * period)){};
    void tick(Widget* widget) override {
        const auto counter = widget->get_counter();
        if (counter >= 65535) widget->reset_counter();

        s32 new_rotation = static_cast<s32>(m_angle * mkb::math_sin(m_period * counter));
        widget->increment_counter();
    }
};

}// namespace ui
