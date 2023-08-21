#include "widget_button.h"

#include "pad.h"
#include "internal/ui/widget_text.h"

namespace ui {

void Button::disp() {
    Text::disp();
}

void Button::tick() {
    if (m_is_active) {
        m_color = DEFAULT_YELLOW;

        if(pad::button_pressed(mkb::PAD_BUTTON_A)) {
            m_callback();
        }
    }
    else {
        m_color = UNSELECTED_YELLOW;
    }

    Widget::tick();
}

}
