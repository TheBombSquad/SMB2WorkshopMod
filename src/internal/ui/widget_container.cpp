#include "widget_container.h"

#include "internal/log.h"
#include "internal/pad.h"
#include "internal/ui/widget_button.h"

namespace ui {

void Container::tick() {
    const auto child_count = m_children.size();
    auto child_iterator = m_children.begin();
    u32 index = 0;

    // TODO: Menu stuff - should be moved out!!
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

    // Dimensional constraints for widgets
    float slice_dim;
    if (m_layout == ContainerLayout::VERTICAL) {
        slice_dim = m_dimensions.y / (child_count + 1);
    }
    else {
        slice_dim = m_dimensions.x / (child_count + 1);
    }

    // Origin constraints for widgets
    Vec2d widget_origin;
    if (m_layout == ContainerLayout::VERTICAL) {
        if (m_layout_align == mkb::ALIGN_CENTER) {
            widget_origin.x = get_pos_center_point().x;
        }
        else {
            // TODO
            widget_origin.x = get_pos_center_point().x;
        }

        widget_origin.y = m_pos.y;
    }
    else {
        if (m_layout_align == mkb::ALIGN_CENTER) {
            widget_origin.y = get_pos_center_point().y;
        }
        else {
            // TODO
            widget_origin.y = get_pos_center_point().y;
        }

        widget_origin.x = m_pos.x;
    }

    while (child_iterator != m_children.end()) {
        auto& child = *child_iterator;
        uint16_t child_dimension_scale;
        child_dimension_scale = (slice_dim) * (index + 1);

        if (m_layout == ContainerLayout::VERTICAL) {
            child->set_pos(Vec2d{widget_origin.x, widget_origin.y + child_dimension_scale});
        }
        else {
            child->set_pos(Vec2d{widget_origin.x + child_dimension_scale, widget_origin.y});
        }

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
