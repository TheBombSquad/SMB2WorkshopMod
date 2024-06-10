#include "widget_container.h"
#include "widget_input.h"
#include "widget_layout.h"
#include "widget_menu.h"
#include "widget_sprite.h"
#include "widget_text.h"

namespace ui {

void Container::tick() {
    Widget::tick();
    // Tick and disp all of our children
    for (auto iter = m_children.begin(); iter != m_children.end();) {
        // Clean up children if they are marked as inactive
        if (iter->get()->is_inactive()) {
            iter = m_children.erase(iter);
            // LOG("In widget, inactive widget/child found and will be erased");
            continue;
        }
        iter->get()->tick();
        ++iter;
    }
}

// Add child widget, and return a reference to the added child
template<typename T>
T& Container::add(T* widget) {
    MOD_ASSERT_MSG(m_children.size() < WIDGET_MAX_CHILDREN, "Tried to add more widget children than the capacity of the widget");
    widget->set_depth(m_depth - m_child_depth_step);
    auto& ptr_ref = m_children.emplace_back(std::move(widget));
    // LOG("Adding child with depth: %f", ptr_ref->get_depth());
    return static_cast<T&>(*ptr_ref);
}

// Template specializations to avoid vague linkage
// This is a ELF section count optimization, only really relevant when compiling with -Os
template Layout& Container::add<Layout>(Layout*);
template Text& Container::add<Text>(Text*);
template Input& Container::add<Input>(Input*);
template Menu& Container::add<Menu>(Menu*);
template Sprite& Container::add<Sprite>(Sprite*);
template Window& Container::add<Window>(Window*);

// Remove child widget by reference
void Container::remove(Widget& widget) {
    for (auto iter = m_children.begin(); iter != m_children.end();) {
        if (&widget == iter->get()) {
            iter->get()->set_inactive();
            break;
        }
        else {
            ++iter;
        }
    }
}

// Remove child widget by label
void Container::remove(const char* label) {
    for (auto& widget: m_children) {
        if (strcmp(label, widget->get_label()) == 0) {
            widget->set_inactive();
            break;
        }
    }
}

// Remove all child widgets
void Container::clear() {
    m_children.clear();
}

void Container::free_inactive() {
    for (auto iter = m_children.begin(); iter != m_children.end();) {
        // Clean up children if they are marked as inactive
        if (iter->get()->is_inactive()) {
            iter = m_children.erase(iter);
            // LOG("In widget, inactive widget/child found and will be erased");
            continue;
        }
        ++iter;
    }
}

}// namespace ui