#include "widget_button.h"

#include "internal/pad.h"
#include "internal/ui/widget_text.h"

namespace ui {

Button::Button(const char* text, etl::delegate<void()> callback) : Text(text) {
    m_callback = callback;
    m_interactable = true;
}
Button::Button(const char* text, const Vec2d pos, etl::delegate<void()> callback) : Text(text, pos) {
    m_callback = callback;
    m_interactable = true;
}

Button::~Button() = default;

void Button::disp() {
    Text::disp();
}

void Button::tick() {
    if (m_is_active) {
        m_color = DEFAULT_YELLOW;

        if (pad::button_pressed(m_input)) {
            mkb::call_SoundReqID_arg_2(0x6e);
            m_callback();
        }
    }
    else {
        m_color = UNSELECTED_YELLOW;
    }

    Widget::tick();
}

}// namespace ui
