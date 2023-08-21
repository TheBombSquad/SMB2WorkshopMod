#pragma once

#include "mkb/mkb.h"
#include "etl/memory.h"
#include "etl/vector.h"
#include "internal/ui/widget.h"

namespace ui {

// Manages all tickables
constexpr size_t WIDGET_CAPACITY = 16;

class WidgetManager {
public:
    typedef etl::vector<etl::unique_ptr<Widget>, WIDGET_CAPACITY> WidgetVec;
    const WidgetVec& get_widgets() const { return m_widgets; };
    void push(etl::unique_ptr<Widget>&& widget) { m_widgets.emplace_back(std::move(widget)); }
    void tick() {
        for (const auto& widget : m_widgets) {
            widget->tick();
        }
    }

private:
    WidgetVec m_widgets;
};

WidgetManager& get_widget_manager();

}// namespace ui
