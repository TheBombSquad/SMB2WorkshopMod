#pragma once

#include "internal/ui/widget.h"

namespace ui {

enum class ContainerLayout {
    HORIZONTAL,
    VERTICAL,
};

class Container : public Widget {
public:
    Container(const Vec2d pos, const Vec2d dimensions) : Widget(pos, dimensions) {}
    virtual void tick() override;
    virtual void disp() override{};

    ContainerLayout get_layout() const {
        return m_layout;
    }
    void set_layout(ContainerLayout layout) {
        m_layout = layout;
    }
    u16 get_layout_spacing() const {
        return m_layout_spacing;
    }
    void set_layout_spacing(u16 layout_spacing) {
        m_layout_spacing = layout_spacing;
    }
    mkb::SpriteAlignment get_alignment() const {
        return m_alignment;
    }
    void set_alignment(mkb::SpriteAlignment alignment) {
        m_alignment = alignment;
    }

private:
    u32 m_active_index = 0;

    // The layout of widgets within the container.
    // Vertical resizes child widgets to fit the height of the container, then stacks them on top of each other.
    // Horizontal resizes child widgets to fit the width of the container, then orders them left-to-right.
    ContainerLayout m_layout = ContainerLayout::VERTICAL;

    // Spacing between widgets in the container.
    u16 m_layout_spacing = 5;

    // Alignment of widgets in the container relative to the origin point of the container
    mkb::SpriteAlignment m_alignment = mkb::ALIGN_UPPER_LEFT;
};

}// namespace ui
