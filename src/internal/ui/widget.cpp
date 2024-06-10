#include "widget.h"

#include "widget_input.h"
#include "widget_layout.h"
#include "widget_menu.h"
#include "widget_sprite.h"
#include "widget_text.h"
#include "widget_window.h"

namespace ui {

void Widget::tick() {
    // Tick all of our modifiers
    for (const auto& modifier: m_tick_modifier) {
        modifier->tick(this);
    }

    // Display the widget
    if (is_visible()) disp();
}

// Marks the widget as inactive (queued to be removed)
void Widget::set_inactive() {
    // LOG("Setting widget and children as inactive");
    m_flags.set(WIDGET_FLAG_ACTIVE, false);

    /*
    for (auto iter = this.m_children.begin(); iter != this.m_children.end();) {
        Widget::set_inactive(**iter);
        ++iter;
    }
     */
}

}// namespace ui
