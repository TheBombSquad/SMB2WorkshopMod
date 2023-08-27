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
    ContainerLayout m_layout = ContainerLayout::VERTICAL;
    mkb::SpriteAlignment m_layout_align = mkb::ALIGN_CENTER;
    bool m_layout_justify = true;
    u16 m_layout_spacing = 5;
};

}// namespace ui
