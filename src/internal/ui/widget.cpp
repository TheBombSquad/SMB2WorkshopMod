#include "widget.h"

#include "widget_button.h"
#include "widget_container.h"
#include "widget_input.h"
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
    disp();

    // Tick and disp all of our children
    for (auto iter = m_children.begin(); iter != m_children.end();) {
        // Clean up children if they are marked as inactive
        if (iter->get()->is_inactive()) {
            m_children.erase(iter);
            // LOG("In widget, inactive widget/child found and will be erased");
            ++iter;
            continue;
        }
        iter->get()->tick();
        if (m_visible) iter->get()->disp();
        ++iter;
    }
}
// Add child widget, and return a reference to the added child
template<typename T>
T& Widget::add(T* widget) {
    MOD_ASSERT_MSG(m_children.size() < WIDGET_MAX_CHILDREN, "Tried to add more widget children than the capacity of the widget");
    widget->set_depth(m_depth - 0.005);
    auto& ptr_ref = m_children.emplace_back(std::move(widget));
    // LOG("Adding child with depth: %f", ptr_ref->get_depth());
    return static_cast<T&>(*ptr_ref);
}

// Template specializations to avoid vague linkage
// This is a ELF section count optimization, only really relevant when compiling with -Os
template Button& Widget::add<Button>(Button*);
template Container& Widget::add<Container>(Container*);
template Text& Widget::add<Text>(Text*);
template Input& Widget::add<Input>(Input*);
template Menu& Widget::add<Menu>(Menu*);
template Sprite& Widget::add<Sprite>(Sprite*);
template Window& Widget::add<Window>(Window*);

// Remove child widget by reference
void Widget::remove(Widget& widget) {
    for (auto iter = m_children.begin(); iter != m_children.end();) {
        if (&widget == iter->get()) {
            iter->get()->m_active = false;
            break;
        }
        else {
            ++iter;
        }
    }
}

// Remove child widget by label
void Widget::remove(const char* label) {
    for (auto& widget: m_children) {
        if (strcmp(label, widget->get_label().c_str()) == 0) {
            widget->m_active = false;
            break;
        }
    }
}

// Remove all child widgets
void Widget::clear() {
    for (auto iter = m_children.begin(); iter != m_children.end();) {
        m_children.erase(iter++);
    }
}

// Marks the widget as inactive (queued to be removed)
void Widget::set_inactive(Widget& widget) {
    // LOG("Setting widget and children as inactive");
    widget.m_active = false;
    for (auto iter = widget.m_children.begin(); iter != widget.m_children.end();) {
        Widget::set_inactive(**iter);
        ++iter;
    }
}

}// namespace ui
