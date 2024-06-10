#pragma once

#include "internal/ui/widget_container.h"

namespace ui {

enum class LayoutArrangement {
    HORIZONTAL,// Newly added widgets are placed in a horizontal row
    VERTICAL,  // Newly added widgets are placed in a vertical column
    NONE,      // Newly added widgets are not organized or auto-scaled
};


// Widget for neatly organizing other widgets
class Layout : public Container {
public:
    Layout(const Vec2d pos, const Vec2d dimensions);
    virtual void tick() override;
    virtual void disp() override{};
    ~Layout();

    LayoutArrangement get_arrangement() const {
        return m_layout;
    }
    void set_layout(LayoutArrangement layout) {
        m_layout = layout;
    }
    u16 get_spacing() const {
        return m_spacing;
    }
    void set_spacing(u16 layout_spacing) {
        m_spacing = layout_spacing;
    }
    mkb::SpriteAlignment get_alignment() const {
        return m_alignment;
    }
    void set_alignment(mkb::SpriteAlignment alignment) {
        m_alignment = alignment;
    }
    u16 get_margin() const {
        return m_margin;
    }
    void set_margin(u16 margin) {
        m_margin = margin;
    }

protected:
    // The layout of widgets within the container.
    // Vertical resizes child widgets to fit the height of the container, then stacks them on top of each other.
    // Horizontal resizes child widgets to fit the width of the container, then orders them left-to-right.
    alignas(4) LayoutArrangement m_layout = LayoutArrangement::VERTICAL;

    // Spacing between widgets in the container.
    u16 m_spacing = 5;

    // Margin around the container
    u16 m_margin = 5;

    // Alignment of widgets in the container relative to the origin point of the container
    alignas(4) mkb::SpriteAlignment m_alignment = mkb::ALIGN_UPPER_LEFT;
};

}// namespace ui
