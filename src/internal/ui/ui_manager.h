#pragma once

#include "etl/memory.h"
#include "etl/vector.h"
#include "internal/ui/widget.h"
#include "mkb/mkb.h"

namespace ui {

// Manages all tickables
constexpr size_t WIDGET_CAPACITY = 16;

class WidgetManager {
public:
    typedef etl::list<etl::unique_ptr<Widget>, WIDGET_CAPACITY> WidgetVec;
    const WidgetVec& get_widgets() const { return m_widgets; };

    // Add child widget, and return a reference to the added child
    template<typename T>
    T& add(T* widget) {
        auto& ptr_ref = m_widgets.emplace_back(std::move(widget));
        return static_cast<T&>(*ptr_ref);
    }

    void tick() {
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

    void remove(Widget& widget) {
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

    void remove(const char* label) {
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

    void clear() {
        for (auto iter = m_widgets.begin(); iter != m_widgets.end();) {
            m_widgets.erase(iter++);
        }
    }

    Widget& find(const char* label) {
        for (auto& widget: m_widgets) {
            if (strcmp(label, widget->get_label().c_str()) == 0) {
                return *widget;
            }
        }
        MOD_ASSERT_MSG(false, "Failed to find widget!");
    }

private:
    WidgetVec m_widgets;
};

WidgetManager& get_widget_manager();

}// namespace ui
