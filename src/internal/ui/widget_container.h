#pragma once

#include "internal/ui/widget.h"

namespace ui {

class Container : public Widget {
public:
    Container(const Vec2d pos, const Vec2d dimensions) : Widget(pos, dimensions) {}
    virtual void disp() override;
    virtual void tick() override;

private:
    u32 m_active_index = 0;
};

}// namespace ui
