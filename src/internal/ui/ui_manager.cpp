#include "ui_manager.h"

#include "widget_input.h"
#include "widget_layout.h"
#include "widget_menu.h"
#include "widget_sprite.h"
#include "widget_text.h"
#include "widget_window.h"

namespace ui {

// Add child widget, and return a reference to the added child
template<typename T>
T& WidgetManager::add(T* widget) {
    auto& ptr_ref = m_widgets.emplace_back(std::move(widget));
    return static_cast<T&>(*ptr_ref);
}
// Template specializations to avoid vague linkage
// This is a ELF section count optimization, only really relevant when compiling with -Os
template Layout& WidgetManager::add<Layout>(Layout*);
template Text& WidgetManager::add<Text>(Text*);
template Input& WidgetManager::add<Input>(Input*);
template Menu& WidgetManager::add<Menu>(Menu*);
template Sprite& WidgetManager::add<Sprite>(Sprite*);
template Window& WidgetManager::add<Window>(Window*);

void WidgetManager::tick() {
    for (auto iter = m_widgets.begin(); iter != m_widgets.end();) {
        if (iter->get()->is_inactive()) {
            // LOG("In manager, inactive widget %s found and will be erased", iter->get()->get_label().c_str());
            iter = m_widgets.erase(iter);
            continue;
        }
        iter->get()->tick();
        ++iter;
    }
}

void WidgetManager::remove(Widget& widget) {
    for (auto iter = m_widgets.begin(); iter != m_widgets.end();) {
        if (&widget == iter->get()) {
            iter->get()->set_inactive();
            break;
        }
        else {
            ++iter;
        }
    }
}

void WidgetManager::remove(const char* label) {
    for (auto iter = m_widgets.begin(); iter != m_widgets.end();) {
        if (strcmp(label, iter->get()->get_label()) == 0) {
            iter->get()->set_inactive();
            break;
        }
        else {
            ++iter;
        }
    }
}

void WidgetManager::clear() {
    for (auto iter = m_widgets.begin(); iter != m_widgets.end();) {
        iter = m_widgets.erase(iter);
    }
}

Widget& WidgetManager::find(const char* label) {
    for (auto& widget: m_widgets) {
        if (strcmp(label, widget->get_label()) == 0) {
            return *widget;
        }
    }
    LOG("Failed to find: %s\n", label);
    MOD_ASSERT_MSG(false, "Failed to find widget!");
}
WidgetManager& get_widget_manager() {
    alignas(4) static uint8_t s_widget_manager[sizeof(WidgetManager)];
    alignas(4) static bool s_buf_init = false;

    if (!s_buf_init) {
        new (s_widget_manager) WidgetManager();
        s_buf_init = true;
    }
    auto ptr = reinterpret_cast<WidgetManager*>(s_widget_manager);
    return *ptr;
}

}// namespace ui