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

    const WidgetManager::WidgetVec& get_widgets() const {
        return m_widgets;
    };

    // Add child widget, and return a reference to the added child
    template<typename T>
    T& add(T* widget);
    void tick();
    void remove(Widget& widget);
    void remove(const char* label);
    void clear();
    Widget& find(const char* label);

private:
    WidgetVec m_widgets;
};

WidgetManager& get_widget_manager();

}// namespace ui
