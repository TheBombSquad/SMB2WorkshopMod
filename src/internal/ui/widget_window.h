#pragma once

#include "internal/ui/widget.h"

namespace ui {

class Window : public Widget {
public:
    Window(const Vec2d pos, const Vec2d dimensions) : Widget(pos, dimensions) {}
    virtual void disp() override;
};

}// namespace ui