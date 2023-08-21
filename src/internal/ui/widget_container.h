#pragma once

#include "widget.h"
namespace ui {

class Container : public Widget {
public:
    Container(const Vec2d pos, const Vec2d dimensions) : Widget(pos, dimensions) {}
    void disp() override;
    void tick() override;
private:
    u32 m_active_index = 0;
};

}// namespace ui
