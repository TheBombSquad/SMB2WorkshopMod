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

private:
    u32 m_active_index = 0;

    // The layout of widgets within the container.
    // Vertical resizes child widgets to fit the height of the container, then stacks them on top of each other.
    // Horizontal resizes child widgets to fit the width of the container, then orders them left-to-right.
    ContainerLayout m_layout = ContainerLayout::VERTICAL;

    // Alignment of widgets within the container.
    // Specifies where the origin point of the widgets should be.
    mkb::SpriteAlignment m_layout_align = mkb::ALIGN_CENTER;

    // Spacing between widgets in the container.
    u16 m_layout_spacing = 5;
};

}// namespace ui
