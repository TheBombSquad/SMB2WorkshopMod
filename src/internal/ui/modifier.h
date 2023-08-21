#pragma once

namespace ui {

class Widget;
class Modifier {
public:
    virtual void tick(Widget* widget) = 0;
    virtual ~Modifier();
};

}// namespace ui
