#pragma once

#include "mkb/mkb.h"
#include "modifier.h"
#include "widget.h"

namespace ui {

class LifetimeModifier : public Modifier {
    u32 m_frame_lifetime;
    void tick(Widget* widget) override {
        if (widget->get_counter() < m_frame_lifetime) {
            widget->increment_counter();
        }
        else {
            // TODO
        }
        return;
    }
};

}// namespace ui
