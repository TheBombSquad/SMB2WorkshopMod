#include "widget.h"

namespace ui {

void Widget::tick() {
    // Tick all of our modifiers
    for (const auto& modifier: m_tick_modifier) {
        modifier->tick(this);
    }

    // Display the widget
    disp();

    // Tick and disp all of our children
    for (auto iter = m_children.begin(); iter != m_children.end();) {
        // Clean up children if they are marked as inactive
        if (iter->get()->is_inactive()) {
            m_children.erase(iter);
            LOG("In widget, inactive widget/child found and will be erased");
            ++iter;
            continue;
        }
        iter->get()->tick();
        if (m_visible) iter->get()->disp();
        ++iter;
    }
}

}// namespace ui
