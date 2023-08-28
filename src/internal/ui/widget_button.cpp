#include "widget_button.h"

#include "internal/pad.h"
#include "internal/ui/widget_text.h"

namespace ui {

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
