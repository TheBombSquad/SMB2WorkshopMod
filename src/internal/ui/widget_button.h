#pragma once

#include "ui/widget.h"
#include "ui/widget_text.h"

namespace ui {

class Button : public Text {
public:
    Button(const char* text, const Vec2d dimensions, etl::delegate<void()> callback) : Text(text, dimensions) { m_callback = callback; }
    Button(const char* text, const Vec2d pos, const Vec2d dimensions, etl::delegate<void()> callback) : Text(text, pos, dimensions) { m_callback = callback; }
    virtual void tick() override;
    virtual void disp() override;

    bool is_active() const {
        return m_is_active;
    }
    void set_active(bool is_active) {
        Button::m_is_active = is_active;
    }

private:
    bool m_is_active = false;
};

}// namespace ui
