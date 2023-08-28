#include "widget_container.h"

#include "internal/log.h"
#include "internal/pad.h"
#include "internal/ui/widget_button.h"

namespace ui {

void Container::tick() {
    const auto child_count = m_children.size();
    auto child_iterator = m_children.begin();
    u32 index = 0;

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

    // Vertical stacking
    const auto bottom_bounds = m_dimensions.y;
    const auto slice_dim = bottom_bounds / (child_count + 1);

    const auto center_point = get_pos_center_point();
    while (child_iterator != m_children.end()) {
        auto& child = *child_iterator;
        uint16_t child_vertical_pos;
        if (m_layout_justify) {
            child_vertical_pos = (slice_dim) * (index + 1);
        }
        else {
            child_vertical_pos = (child->get_dimensions().y + m_layout_spacing * 2) * (index + 1);
        }

        child->set_pos(Vec2d{center_point.x, m_pos.y + child_vertical_pos});

        // TODO: move out of this
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
