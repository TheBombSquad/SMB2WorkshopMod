#include "widget_menu.h"

#include "internal/pad.h"
#include "widget_text.h"

namespace ui {
Menu::Menu(const Vec2d pos, const Vec2d dimensions) : Window(pos, dimensions) {}
Menu::~Menu() = default;

void Menu::tick() {
    Container::tick();

    // TODO: skip over/don't select non-interactable widgets
    const unsigned int child_count = m_children.size();
    if (pad::dir_pressed(pad::DIR_DOWN)) {
        mkb::call_SoundReqID_arg_1(0x6f);
        LOG_DEBUG("active idx: %d", m_active_index);
        if (m_active_index == child_count - 1) {
            m_active_index = 0;
        }
        else {
            m_active_index++;
        }
    }

    if (pad::dir_pressed(pad::DIR_UP)) {
        mkb::call_SoundReqID_arg_1(0x6f);
        LOG_DEBUG("active idx: %d", m_active_index);
        if (m_active_index == 0) {
            m_active_index = child_count - 1;
        }
        else {
            m_active_index--;
        }
    }

    if (pad::button_pressed(mkb::PAD_BUTTON_A)) {
        mkb::call_SoundReqID_arg_1(0x6f);
        auto iter = m_children.begin();
        etl::advance(iter, m_active_index);
        auto& button_ref = static_cast<Text&>(**iter);
        button_ref.do_callback();
    }

    unsigned int index = 0;
    for (auto& child: m_children) {
        if (!child->get_callback()) continue;
        auto& button_ref = static_cast<Text&>(*child);
        if (m_active_index == index) {
            button_ref.set_color(ui::DEFAULT_YELLOW);
        }
        else {
            button_ref.set_color(ui::UNSELECTED_YELLOW);
        }
        index++;
    }
}
}// namespace ui