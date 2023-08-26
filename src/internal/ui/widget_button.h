#pragma once

#include "ui/widget.h"
#include "ui/widget_text.h"

namespace ui {

class Button : public Text {
public:
    Button(const char* text, etl::delegate<void()> callback) : Text(text) { m_callback = callback; }
    Button(const Vec2d pos, const char* text, etl::delegate<void()> callback) : Text(pos, text) { m_callback = callback; }
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
