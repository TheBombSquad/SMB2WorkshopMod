#pragma once

#include "internal/ui/widget.h"
#include "internal/ui/widget_container.h"

namespace ui {

class Window : public Container {
public:
    Window(const Vec2d pos, const Vec2d dimensions) : Container(pos, dimensions) {}
    virtual void disp() override;
};

}// namespace ui