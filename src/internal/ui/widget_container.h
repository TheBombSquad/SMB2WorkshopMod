#pragma once

#include "internal/ui/widget.h"

namespace ui {

class Container : public Widget {
protected:
    static constexpr uint32_t WIDGET_MAX_CHILDREN = 12;
    etl::vector<etl::unique_ptr<Widget>, WIDGET_MAX_CHILDREN> m_children;

    float m_child_depth_step = 0.005;

    // When we add a child sprite, by how many units should it be 'in front' of the parent?
    Container(const Vec2d pos, const Vec2d dimensions) : Widget(pos, dimensions) {}

public:
    virtual void tick();
    virtual void disp() = 0;
    virtual ~Container() {};
    Container(const Container&) = delete;
    Container& operator=(const Container&) = delete;

    // Add child widget, and return a reference to the added child
    template<typename T>
    T& add(T* widget);
    void remove(Widget& widget);
    void remove(const char* label);
    void clear();
    void free_inactive();

    float get_child_depth_step() const {
        return m_child_depth_step;
    }
    void set_child_depth_step(float child_depth_step) {
        m_child_depth_step = child_depth_step;
    }
};

}// namespace ui