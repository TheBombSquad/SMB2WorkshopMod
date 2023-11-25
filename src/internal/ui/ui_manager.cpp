#include "ui_manager.h"

namespace ui {

// Add child widget, and return a reference to the added child
template<typename T>
T& WidgetManager::add(T* widget) {
    auto& ptr_ref = m_widgets.emplace_back(std::move(widget));
    return static_cast<T&>(*ptr_ref);
}

void WidgetManager::tick() {
    for (auto iter = m_widgets.begin(); iter != m_widgets.end();) {
        if (iter->get()->is_inactive()) {
            // LOG("In manager, inactive widget %s found and will be erased", iter->get()->get_label().c_str());
            m_widgets.erase(iter);
            ++iter;
            continue;
        }
        iter->get()->tick();
        ++iter;
    }
}

void WidgetManager::remove(Widget& widget) {
    for (auto iter = m_widgets.begin(); iter != m_widgets.end();) {
        if (&widget == iter->get()) {
            Widget::set_inactive(*iter->get());
            break;
        }
        else {
            ++iter;
        }
    }
}

void WidgetManager::remove(const char* label) {
    for (auto iter = m_widgets.begin(); iter != m_widgets.end();) {
        if (strcmp(label, iter->get()->get_label().c_str()) == 0) {
            Widget::set_inactive(*iter->get());
            break;
        }
        else {
            ++iter;
        }
    }
}

void WidgetManager::clear() {
    for (auto iter = m_widgets.begin(); iter != m_widgets.end();) {
        m_widgets.erase(iter++);
    }
}

Widget& WidgetManager::find(const char* label) {
    for (auto& widget: m_widgets) {
        if (strcmp(label, widget->get_label().c_str()) == 0) {
            return *widget;
        }
    }
    MOD_ASSERT_MSG(false, "Failed to find widget!");
}
WidgetManager& get_widget_manager() {
    static uint8_t s_widget_manager[sizeof(WidgetManager)];
    static bool s_buf_init = false;

    if (!s_buf_init) {
        new (s_widget_manager) WidgetManager();
        s_buf_init = true;
    }
    auto ptr = reinterpret_cast<WidgetManager*>(s_widget_manager);
    return *ptr;
}

}// namespace ui