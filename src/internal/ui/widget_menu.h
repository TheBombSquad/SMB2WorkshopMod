#pragma once

#include "internal/ui/widget.h"
#include "internal/ui/widget_window.h"

namespace ui {

class Menu : public Window {
public:
    Menu(const Vec2d pos, const Vec2d dimensions);
    ~Menu();
    virtual void tick() override;
};

}// namespace ui
