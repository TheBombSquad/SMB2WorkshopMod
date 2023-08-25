#include "widget_container.h"

#include "internal/pad.h"
#include "internal/ui/widget_button.h"

namespace ui {

void Container::disp() {
    // TODO: logic..
}

void Container::tick() {
    auto container_size = m_children.size();
    auto child_iterator = m_children.begin();
    u32 index = 0;

    if (pad::dir_pressed(pad::DIR_DOWN)) {
        mkb::call_SoundReqID_arg_2(0x6f);
        mkb::OSReport("active idx: %d\n", m_active_index);
        if (m_active_index == container_size - 1) {
            m_active_index = 0;
        }
        else {
            m_active_index++;
        }
    }

    if (pad::dir_pressed(pad::DIR_UP)) {
        mkb::call_SoundReqID_arg_2(0x6f);
        mkb::OSReport("active idx: %d\n", m_active_index);
        if (m_active_index == 0) {
            m_active_index = container_size - 1;
        }
        else {
            m_active_index--;
        }
    }

    while (child_iterator != m_children.end()) {
        auto& button_ref = static_cast<Button&>(**child_iterator);
        if (m_active_index == index) {
            button_ref.set_active(true);
        }
        else {
            button_ref.set_active(false);
        }
        child_iterator++;
        index++;
    }

    Widget::tick();
}

}// namespace ui
