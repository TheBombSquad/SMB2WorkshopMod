#include "widget_menu.h"

#include "internal/pad.h"
#include "widget_button.h"

namespace ui {
Menu::Menu(const Vec2d pos, const Vec2d dimensions) : Window(pos, dimensions) {}
Menu::~Menu() = default;

void Menu::tick() {
    Container::tick();

    // TODO: skip over/don't select non-interactable widgets
    const unsigned int child_count = m_children.size();
    if (pad::dir_pressed(pad::DIR_DOWN)) {
        mkb::call_SoundReqID_arg_2(0x6f);
        LOG_DEBUG("active idx: %d", m_active_index);
        if (m_active_index == child_count - 1) {
            m_active_index = 0;
        }
        else {
            m_active_index++;
        }
    }

    if (pad::dir_pressed(pad::DIR_UP)) {
        mkb::call_SoundReqID_arg_2(0x6f);
        LOG_DEBUG("active idx: %d", m_active_index);
        if (m_active_index == 0) {
            m_active_index = child_count - 1;
        }
        else {
            m_active_index--;
        }
    }

    unsigned int index = 0;
    for (const auto& child: m_children) {
        if (!child->is_interactable()) continue;
        auto& button_ref = static_cast<Button&>(*child);
        if (m_active_index == index) {
            button_ref.set_active(true);
        }
        else {
            button_ref.set_active(false);
        }
        index++;
    }
}
}// namespace ui