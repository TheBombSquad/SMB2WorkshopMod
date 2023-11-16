#include "widget_container.h"

#include "internal/log.h"
#include "internal/pad.h"
#include "internal/ui/widget_button.h"

namespace ui {

void Container::tick() {
    const unsigned int child_count = m_children.size();
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

    // Origin constraints for widgets
    Vec2d widget_origin;

    switch(m_alignment) {
        case mkb::ALIGN_UPPER_LEFT:
            widget_origin = m_pos;
            break;
        case mkb::ALIGN_UPPER_CENTER:
            widget_origin = Vec2d{get_pos_center_point().x, m_pos.y};
            break;
        case mkb::ALIGN_UPPER_RIGHT:
            widget_origin = Vec2d{m_dimensions.x, m_pos.y};
            break;
        case mkb::ALIGN_CENTER_LEFT:
            widget_origin = Vec2d{m_pos.x, get_pos_center_point().y};
            break;
        case mkb::ALIGN_CENTER:
            widget_origin = get_pos_center_point();
            break;
        case mkb::ALIGN_CENTER_RIGHT:
            widget_origin = Vec2d{m_dimensions.x, get_pos_center_point().y};
            break;
        case mkb::ALIGN_LOWER_LEFT:
            widget_origin = Vec2d{m_pos.x, m_dimensions.y};
            break;
        case mkb::ALIGN_LOWER_CENTER:
            widget_origin = Vec2d{get_pos_center_point().x, m_dimensions.y};
            break;
        case mkb::ALIGN_LOWER_RIGHT:
            widget_origin = m_dimensions;
            break;
        default:
            widget_origin = m_pos;
            MOD_ASSERT_MSG(false, "how did we get here?");
    }

    // Calculate the total dimensions of widgets once we add them
    Vec2d total_child_dimensions = {0.0f, 0.0f};
    auto child_iterator = m_children.begin();
    size_t child_index = 0;
    while (child_iterator != m_children.end()) {
        const auto& child = *child_iterator;
        const auto& dimensions = child->get_dimensions();
        LOG("child %d: pos: %f, %f dim %f, %f", child_index, child->get_pos().x, child->get_pos().y, dimensions.x, dimensions.y);
        if (m_layout == ContainerLayout::VERTICAL) {
            if (total_child_dimensions.x > dimensions.x) total_child_dimensions.x = dimensions.x;
            total_child_dimensions.y += dimensions.y;
            if (child_index == child_count-1) total_child_dimensions.y += m_layout_spacing; // Add spacing, unless last element
        }
        else {
            total_child_dimensions.x += dimensions.x;
            if (child_index == child_count-1) total_child_dimensions.x += m_layout_spacing; // Add spacing, unless last element
            if (total_child_dimensions.y > dimensions.y) total_child_dimensions.y = dimensions.y;
        }
        child_iterator++;
        child_index++;
    }

    // Scaling factor to fit all the widgets in the container space
    Vec2d child_scale = {1.0f, 1.0f};
    if (total_child_dimensions.x > m_dimensions.x) child_scale.x = m_dimensions.x / total_child_dimensions.x;
    if (total_child_dimensions.y > m_dimensions.y) child_scale.y = m_dimensions.y / total_child_dimensions.y;

    mkb::OSReport("total dimensions: %f, %f / scale: %f, %f\n", total_child_dimensions.x, total_child_dimensions.y, child_scale.x, child_scale.y);

    // Lays out the widgets in the container
    child_iterator = m_children.begin();
    while (child_iterator != m_children.end()) {
        auto& child = *child_iterator;
        child->set_pos(Vec2d{widget_origin.x, widget_origin.y});
        child->set_scale({child->get_scale().x * child_scale.x, child->get_scale().y * child_scale.y});

        if (m_layout == ContainerLayout::VERTICAL) {
            widget_origin.y += child->get_dimensions().y * child_scale.y;
        }
        else {
            widget_origin.x += child->get_dimensions().x * child_scale.x;
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
    // Determines the max size a single widget can take up and still fit the container dimensions
    /*
    float max_slice_size;
    if (m_layout == ContainerLayout::VERTICAL) {
        max_slice_size = m_dimensions.y / (child_count + 1);
    }
    else {
        max_slice_size = m_dimensions.x / (child_count + 1);
    }
    
    // Lays out the widgets in the container
    while (child_iterator != m_children.end()) {
        auto& child = *child_iterator;
        // TODO: padding
        uint16_t padding_size = m_layout_spacing;

        if (m_layout == ContainerLayout::VERTICAL) {
            child->set_pos(Vec2d{widget_origin.x, widget_origin.y + child_dimension_scale});
        }
        else {
            LOG_DEBUG("index: %d, child_dimension_scale: %d, x pos: %f, y pos: %f", index, child_dimension_scale, (widget_origin.x + child_dimension_scale), (widget_origin.y));
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
     */


    Widget::tick();
}

}// namespace ui
