#pragma once

#include "etl/delegate.h"
#include "etl/list.h"
#include "internal/ui/modifier.h"
#include "mkb/mkb.h"

namespace ui {

enum class WidgetState {
    VISIBLE,
    VISIBLE_NO_TICK,
    INVISIBLE,
    INVISIBLE_NO_TICK,
    DESTROY,
};

class Modifier;

class Widget {
protected:
    Widget(Vec2d pos) : m_pos(pos){};
    Widget(Vec2d pos, Vec2d dimensions) : m_pos(pos), m_dimensions(dimensions){};
    uint32_t m_id = 0;
    alignas(4) bool m_visible = true;
    Vec2d m_pos = Vec2d{0.f, 0.f};
    Vec2d m_dimensions = Vec2d{0.f, 0.f};
    Vec2d m_scale = Vec2d{1.f, 1.f};
    int32_t m_depth = 0;
    int32_t m_z_rotation = 0;

    static constexpr uint32_t WIDGET_MAX_CHILDREN = 8;
    etl::list<etl::unique_ptr<Widget>, WIDGET_MAX_CHILDREN> m_children;

    static constexpr uint32_t WIDGET_MAX_MODIFIERS = 4;
    etl::list<etl::unique_ptr<Modifier>, WIDGET_MAX_MODIFIERS> m_tick_modifier;

    uint32_t m_counter = 0;

    etl::delegate<void()> m_callback;

public:
    virtual void tick();
    virtual void disp() = 0;
    virtual ~Widget() { mkb::OSReport("destroy\n"); };
    Widget (const Widget&) = delete;
    Widget& operator = (const Widget&) = delete;

    // Add child widget, and return a reference to the added child
    template<typename T>
    T& add(T* widget) {
        auto& ptr_ref = m_children.emplace_back(std::move(widget));
        return static_cast<T&>(*ptr_ref);
    }
    uint32_t get_m_id() const {
        return m_id;
    }
    const Vec2d& get_pos() const {
        return m_pos;
    }
    void set_pos(const Vec2d& m_pos) {
        Widget::m_pos = m_pos;
    }
    const Vec2d& get_dimensions() const {
        return m_dimensions;
    }
    void set_dimensions(const Vec2d& m_dimensions) {
        Widget::m_dimensions = m_dimensions;
    }
    const Vec2d& get_scale() const {
        return m_scale;
    }
    void set_scale(const Vec2d& m_scale) {
        Widget::m_scale = m_scale;
    }
    int get_depth() const {
        return m_depth;
    }
    void set_depth(int m_depth) {
        Widget::m_depth = m_depth;
    }
    s32 get_z_rotation() const {
        return m_z_rotation;
    }
    void set_z_rotation(s32 m_z_rotation) {
        Widget::m_z_rotation = m_z_rotation;
    }
    uint32_t get_counter() const {
        return m_counter;
    }
    void increment_counter() {
        m_counter++;
    }
    void reset_counter() {
        m_counter = 0;
    }
    uint32_t is_visible() const {
        return m_visible;
    }
    void set_visible(uint32_t is_visible) {
        Widget::m_visible = is_visible;
    }
};

}// namespace ui
