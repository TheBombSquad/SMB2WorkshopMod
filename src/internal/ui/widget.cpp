#include "widget.h"

namespace ui {

void Widget::tick() {
    // Don't do anything if we're invisible
    if (!m_visible) return;

    // Tick all of our modifiers
    for (const auto& modifier: m_tick_modifier) {
        modifier->tick(this);
    }

    // Display the widget
    disp();

    // Tick and disp all of our children
    for (const auto& child: m_children) {
        child->tick();
        child->disp();
    }
}

}// namespace ui
