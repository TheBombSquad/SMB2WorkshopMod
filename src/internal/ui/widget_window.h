#pragma once

#include "internal/ui/widget.h"
#include "internal/ui/widget_layout.h"

namespace ui {

class Window : public Layout {
public:
    Window(const Vec2d pos, const Vec2d dimensions);
    ~Window();
    virtual void disp() override;
};

}// namespace ui