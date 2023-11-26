#include "widget_menu.h"

#include "internal/pad.h"
#include "widget_text.h"

namespace ui {
Menu::Menu(const Vec2d pos, const Vec2d dimensions) : Window(pos, dimensions) {}
Menu::~Menu() = default;

void Menu::tick() {
    Container::tick();

    if (!m_active_index) {
        size_t index = 0;
        for (const auto& child : m_children) {
            if (child->get_callback() && child->is_visible()) {
                if (!m_first_valid_index) {
                    m_first_valid_index = index;
                    m_active_index = m_first_valid_index;
                }
                m_last_valid_index = index;
            }
            index++;
        }
    }
    LOG("first: %d last: %d current: %d", *m_first_valid_index, *m_last_valid_index, *m_active_index);

    if (pad::dir_pressed(pad::DIR_DOWN)) {
        mkb::call_SoundReqID_arg_1(0x6f);
        LOG_DEBUG("down: active idx: %d", *m_active_index);
        if (*m_active_index == *m_last_valid_index) {
            *m_active_index = *m_first_valid_index;
        }
        else {
            (*m_active_index)++;
        }
    }

    else if (pad::dir_pressed(pad::DIR_UP)) {
        mkb::call_SoundReqID_arg_1(0x6f);
        LOG_DEBUG("up: active idx: %d", *m_active_index);
        if (*m_active_index == *m_first_valid_index) {
            *m_active_index = *m_last_valid_index;
        }
        else {
            (*m_active_index)--;
        }
    }

    else if (pad::button_pressed(mkb::PAD_BUTTON_A)) {
        mkb::call_SoundReqID_arg_1(0x6f);
        auto iter = m_children.begin();
        etl::advance(iter, *m_active_index);
        auto& button_ref = static_cast<Text&>(**iter);
        button_ref.do_callback();
    }

    size_t index = 0;
    for (auto& child: m_children) {
        if (!(child->get_callback() && child->is_visible())) {
            index++;
            continue;
        }

        auto& button_ref = static_cast<Text&>(*child);
        if (*m_active_index == index) {
            button_ref.set_color(ui::DEFAULT_YELLOW);
        }
        else {
            button_ref.set_color(ui::UNSELECTED_YELLOW);
        }
        index++;
    }
}
}// namespace ui