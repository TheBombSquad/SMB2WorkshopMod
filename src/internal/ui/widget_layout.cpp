#include "widget_layout.h"

#include "internal/log.h"

namespace ui {

Layout::Layout(const Vec2d pos, const Vec2d dimensions) : Container(pos, dimensions) {}
Layout::~Layout() = default;

void Layout::tick() {
    if (m_layout != LayoutArrangement::NONE) {
        const unsigned int child_count = m_children.size();

        // Origin constraints for widgets
        Vec2d widget_origin;

        switch (m_alignment) {
            case mkb::ALIGN_UPPER_LEFT:
                widget_origin = m_pos;
                widget_origin.x += m_margin;
                widget_origin.y += m_margin;
                break;
            case mkb::ALIGN_UPPER_CENTER:
                widget_origin = Vec2d{get_pos_center_point().x, m_pos.y};
                widget_origin.y += m_margin;
                break;
            case mkb::ALIGN_UPPER_RIGHT:
                widget_origin = Vec2d{m_dimensions.x, m_pos.y};
                widget_origin.x -= m_margin;
                widget_origin.y += m_margin;
                break;
            case mkb::ALIGN_CENTER_LEFT:
                widget_origin = Vec2d{m_pos.x, get_pos_center_point().y};
                widget_origin.x += m_margin;
                break;
            case mkb::ALIGN_CENTER:
                widget_origin = get_pos_center_point();
                break;
            case mkb::ALIGN_CENTER_RIGHT:
                widget_origin = Vec2d{m_dimensions.x, get_pos_center_point().y};
                widget_origin.x -= m_margin;
                break;
            case mkb::ALIGN_LOWER_LEFT:
                widget_origin = Vec2d{m_pos.x, m_dimensions.y};
                widget_origin.x += m_margin;
                widget_origin.y -= m_margin;
                break;
            case mkb::ALIGN_LOWER_CENTER:
                widget_origin = Vec2d{get_pos_center_point().x, m_dimensions.y};
                widget_origin.y -= m_margin;
                break;
            case mkb::ALIGN_LOWER_RIGHT:
                widget_origin = m_dimensions;
                widget_origin.x -= m_margin;
                widget_origin.y -= m_margin;
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
            const auto& dimensions = Vec2d(child->get_dimensions().x * child->get_scale().x, child->get_dimensions().y * child->get_scale().y);
            // LOG("child %d: pos: %f, %f dim %f, %f", child_index, child->get_pos().x, child->get_pos().y, dimensions.x, dimensions.y);
            if (m_layout == LayoutArrangement::VERTICAL) {
                if (dimensions.x > total_child_dimensions.x) total_child_dimensions.x = dimensions.x;
                total_child_dimensions.y += dimensions.y;
                if (child_index != child_count - 1) total_child_dimensions.y += m_spacing;// Add spacing, unless last element
            }
            else {
                if (dimensions.y > total_child_dimensions.y) total_child_dimensions.y = dimensions.y;
                total_child_dimensions.x += dimensions.x;
                if (child_index != child_count - 1) total_child_dimensions.x += m_spacing;// Add spacing, unless last element
            }
            child_iterator++;
            child_index++;
        }

        // Scaling factor to fit all the widgets in the container space
        Vec2d child_scale = {1.0f, 1.0f};
        // LOG("Calc dims vs m_dims: %f, %f / %f, %f", total_child_dimensions.x, total_child_dimensions.y, m_dimensions.x, m_dimensions.y);
        Vec2d calculated_dimensions = m_dimensions;
        calculated_dimensions.x = calculated_dimensions.x - 2 * m_margin;
        calculated_dimensions.y = calculated_dimensions.y - 2 * m_margin;
        if (total_child_dimensions.x > calculated_dimensions.x) child_scale.x = calculated_dimensions.x / total_child_dimensions.x;
        if (total_child_dimensions.y > calculated_dimensions.y) child_scale.y = calculated_dimensions.y / total_child_dimensions.y;

        // mkb::OSReport("total dimensions: %f, %f / scale: %f, %f\n", total_child_dimensions.x, total_child_dimensions.y, child_scale.x, child_scale.y);

        // Lays out the widgets in the container
        child_iterator = m_children.begin();
        while (child_iterator != m_children.end()) {
            auto& child = *child_iterator;
            child->set_pos(Vec2d{widget_origin.x, widget_origin.y});
            // LOG("scale for child: %f, %f", child->get_scale().x * child_scale.x, child->get_scale().y * child_scale.y);
            child->set_scale({child->get_scale().x * child_scale.x, child->get_scale().y * child_scale.y});

            if (child->is_sort()) {
                if (m_layout == LayoutArrangement::VERTICAL) {
                    widget_origin.y += child->get_dimensions().y * child_scale.y;
                    widget_origin.y += m_spacing;
                }
                else {
                    widget_origin.x += child->get_dimensions().x * child_scale.x;
                    widget_origin.x += m_spacing;
                }
            }

            child_iterator++;
        }
    }
    Container::tick();
}

}// namespace ui
