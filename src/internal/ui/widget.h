#pragma once

#include "etl/delegate.h"
#include "etl/vector.h"
#include "etl/string.h"
#include "etl/bitset.h"
#include "internal/ui/modifier.h"
#include "log.h"
#include "mkb/mkb.h"

namespace ui {

typedef etl::delegate<void(Widget&, void*)> WidgetCallback;

class Modifier;

// Base class for a widget.
// Before optimization pass, the size was 444 bytes.
class __attribute((aligned(4))) Widget {
protected:
    explicit Widget(Vec2d pos = Vec2d{0.f, 0.f}, Vec2d dimensions = Vec2d{0.f, 0.f}) : m_pos(pos), m_dimensions(dimensions){
        m_flags.set(WIDGET_FLAG_VISIBLE, true);
        m_flags.set(WIDGET_FLAG_ACTIVE, true);
        m_flags.set(WIDGET_FLAG_SORT, true);
    };
    Vec2d m_pos = Vec2d{0.f, 0.f}; // Position of the widget
    Vec2d m_dimensions = Vec2d{0.f, 0.f}; // Dimensions - important for containers
    Vec2d m_scale = Vec2d{1.f, 1.f}; // Scale - the dimensions are generally multiplied by this value
    float m_depth = 0.1; // How 'deep' the widget is on the screen. Widgets with a depth lower than another widget are 'in front' of the deeper widget.
    float m_child_depth_step = 0.005; // When we add a child sprite, by how many units should it be 'in front' of the parent?
    int32_t m_z_rotation = 0;
    etl::string<8> m_label; // Not as horribly inefficient as you might think

    static constexpr uint32_t WIDGET_MAX_MODIFIERS = 1;
    etl::vector<etl::unique_ptr<Modifier>, WIDGET_MAX_MODIFIERS> m_tick_modifier;

    WidgetCallback m_callback;

    etl::bitset<8, uint8_t> m_flags;

    enum WidgetFlags {
        WIDGET_FLAG_VISIBLE, // Whether the widget should be drawn - should the disp() func be called?
        WIDGET_FLAG_ACTIVE, // Whether the widget is active - inactive widgets are queued to be freed
        WIDGET_FLAG_SORT // Whether the widget should be sorted if it is in a container.
    };

    void* m_user_data = nullptr;

public:
    virtual void tick();
    virtual void disp() = 0;
    virtual ~Widget(){/*LOG("Widget destructor called");*/};
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;


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

    uint32_t is_visible() const { return m_flags[WIDGET_FLAG_VISIBLE]; }
    void set_visible(bool is_visible) { m_flags.set(WIDGET_FLAG_VISIBLE, is_visible); }

    const etl::string<8>& get_label() const { return m_label; }
    void set_label(const char* mLabel) { m_label = mLabel; }

    // Marks the widget as inactive (queued to be removed)
    void set_inactive();

    // Checks if the widget is active (not queued to be removed
    bool is_inactive() { return !m_flags[WIDGET_FLAG_ACTIVE]; }

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

    bool is_sort() const {
        return m_flags[WIDGET_FLAG_SORT];
    }
    void set_sort(bool sort) {
        m_flags.set(WIDGET_FLAG_SORT, sort);
    }
};

}// namespace ui
