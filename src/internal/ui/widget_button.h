#pragma once

#include "ui/widget.h"
#include "ui/widget_text.h"

namespace ui {

class Button : public Text {
public:
    Button(const char* text, etl::delegate<void()> callback) : Text(text) { m_callback = callback; }
    Button(const char* text, const Vec2d pos, etl::delegate<void()> callback) : Text(text, pos) { m_callback = callback; }
    virtual void tick() override;
    virtual void disp() override;

    bool is_active() const {
        return m_is_active;
    }
    void set_active(bool is_active) {
        Button::m_is_active = is_active;
    }
    mkb::PadDigitalInput get_input() const {
        return m_input;
    }
    void set_input(mkb::PadDigitalInput input) {
        m_input = input;
    }

private:
    bool m_is_active = false;
    mkb::PadDigitalInput m_input = mkb::PAD_BUTTON_A;
};

}// namespace ui
