#pragma once

#include "etl/delegate.h"
#include "etl/list.h"
#include "etl/string.h"
#include "internal/ui/modifier.h"
#include "log.h"
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
    Widget(){};
    Widget(Vec2d pos) : m_pos(pos){};
    Widget(Vec2d pos, Vec2d dimensions) : m_pos(pos), m_dimensions(dimensions){};
    Vec2d m_pos = Vec2d{0.f, 0.f};
    Vec2d m_dimensions = Vec2d{0.f, 0.f};
    Vec2d m_scale = Vec2d{1.f, 1.f};
    float m_depth = 0.1;
    int32_t m_z_rotation = 0;
    bool m_interactable = false;
    etl::string<8> m_label;

    static constexpr uint32_t WIDGET_MAX_CHILDREN = 12;
    etl::list<etl::unique_ptr<Widget>, WIDGET_MAX_CHILDREN> m_children;

    static constexpr uint32_t WIDGET_MAX_MODIFIERS = 4;
    etl::list<etl::unique_ptr<Modifier>, WIDGET_MAX_MODIFIERS> m_tick_modifier;

    uint32_t m_counter = 0;

    etl::delegate<void()> m_callback;

    bool m_visible = true;
    bool m_active = true;

public:
    virtual void tick();
    virtual void disp() = 0;
    virtual ~Widget(){/*LOG("Widget destructor called");*/};
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;

    // Add child widget, and return a reference to the added child
    template<typename T>
    T& add(T* widget) {
        MOD_ASSERT_MSG(m_children.size() < WIDGET_MAX_CHILDREN, "Tried to add more widget children than the capacity of the widget");
        widget->set_depth(m_depth - 0.005);
        auto& ptr_ref = m_children.emplace_back(std::move(widget));
        // LOG("Adding child with depth: %f", ptr_ref->get_depth());
        return static_cast<T&>(*ptr_ref);
    }

    // Remove child widget by reference
    void remove(Widget& widget) {
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

    // Remove all child widgets
    void clear() {
        for (auto iter = m_children.begin(); iter != m_children.end();) {
            m_children.erase(iter++);
        }
    }

    const Vec2d& get_pos() const { return m_pos; }
    void set_pos(const Vec2d& m_pos) { Widget::m_pos = m_pos; }

    const Vec2d& get_dimensions() const { return m_dimensions; }
    void set_dimensions(const Vec2d& m_dimensions) { Widget::m_dimensions = m_dimensions; }

    const Vec2d get_pos_center_point() const { return Vec2d{m_pos.x + (m_dimensions.x / 2), m_pos.y + (m_dimensions.y / 2)}; }

    const Vec2d& get_scale() const { return m_scale; }
    void set_scale(const Vec2d& m_scale) { Widget::m_scale = m_scale; }

    float get_depth() const { return m_depth; }
    void set_depth(float m_depth) { Widget::m_depth = m_depth; }

    s32 get_z_rotation() const { return m_z_rotation; }
    void set_z_rotation(s32 m_z_rotation) { Widget::m_z_rotation = m_z_rotation; }

    uint32_t get_counter() const { return m_counter; }
    void increment_counter() { m_counter++; }
    void reset_counter() { m_counter = 0; }

    uint32_t is_visible() const { return m_visible; }
    void set_visible(uint32_t is_visible) { Widget::m_visible = is_visible; }

    const bool is_interactable() const { return m_interactable; }

    const etl::string<8>& get_label() const { return m_label; }
    void set_label(const char* mLabel) { m_label = mLabel; }

    // Marks the widget as inactive (queued to be removed)
    static void set_inactive(Widget& widget) {
        // LOG("Setting widget and children as inactive");
        widget.m_active = false;
        for (auto iter = widget.m_children.begin(); iter != widget.m_children.end();) {
            Widget::set_inactive(**iter);
            ++iter;
        }
    }

    // Checks if the widget is active
    bool is_inactive() { return !m_active; }
};

}// namespace ui
