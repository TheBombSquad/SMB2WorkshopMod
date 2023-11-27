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

typedef etl::delegate<void(Widget&, void*)> WidgetCallback;

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
    float m_child_depth_step = 0.005; // When we add a child sprite, by how many units should it be 'in front' of the parent?
    int32_t m_z_rotation = 0;
    bool m_interactable = false;
    etl::string<8> m_label;

    static constexpr uint32_t WIDGET_MAX_CHILDREN = 12;
    etl::list<etl::unique_ptr<Widget>, WIDGET_MAX_CHILDREN> m_children;

    static constexpr uint32_t WIDGET_MAX_MODIFIERS = 4;
    etl::list<etl::unique_ptr<Modifier>, WIDGET_MAX_MODIFIERS> m_tick_modifier;

    uint32_t m_counter = 0;

    WidgetCallback m_callback;

    bool m_visible = true;
    bool m_active = true;

    void* m_user_data = nullptr;

public:
    virtual void tick();
    virtual void disp() = 0;
    virtual ~Widget(){/*LOG("Widget destructor called");*/};
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;

    // Add child widget, and return a reference to the added child
    template<typename T>
    T& add(T* widget);
    void remove(Widget& widget);
    void remove(const char* label);
    void clear();

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
    static void set_inactive(Widget& widget);

    // Checks if the widget is active (not queued to be removed
    bool is_inactive() { return !m_active; }

    void set_callback(WidgetCallback callback) { m_callback = callback; }
    void set_callback(WidgetCallback callback, void* user_data) {
        m_callback = callback;
        m_user_data = user_data;
    }
    const WidgetCallback& get_callback() const {
        return m_callback;
    }

    void do_callback() {
        m_callback(*this, m_user_data);
    }

    void* get_user_data() const {
        return m_user_data;
    }
    void set_user_data(void* user_data) {
        m_user_data = user_data;
    }
};

}// namespace ui
