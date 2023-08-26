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
        for (const auto& widget: m_widgets) {
            widget->tick();
        }
    }

    void remove(Widget& widget) {
        for (auto iter = m_widgets.begin(); iter != m_widgets.end();) {
            if (&widget == iter->get()) {
                m_widgets.erase(iter);
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

private:
    WidgetVec m_widgets;
};

WidgetManager& get_widget_manager();

}// namespace ui
