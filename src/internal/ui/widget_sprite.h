#pragma once

#include "internal/ui/widget.h"

namespace ui {
class Sprite : public Widget {
public:
    Sprite(uint32_t sprite_id, const Vec2d dimensions) : Widget(), m_sprite_id(sprite_id) { m_dimensions = dimensions; }
    Sprite(uint32_t sprite_id, const Vec2d pos, const Vec2d dimensions) : Widget(pos), m_sprite_id(sprite_id) { m_dimensions = dimensions; }
    virtual void disp() override;

protected:
    uint32_t m_sprite_id;
    float m_alpha = 1.0f;

protected:
    mkb::Rgb24 m_mult_color = {0xff, 0xff, 0xff};
    mkb::Rgb24 m_add_color = {0x0, 0x0, 0x0};
    bool m_mirror = false;

public:
    bool is_mirror() const {
        return m_mirror;
    }
    void set_mirror(bool mirror) {
        m_mirror = mirror;
    }
    float get_alpha() const {
        return m_alpha;
    }
    void set_alpha(float alpha) {
        m_alpha = alpha;
    }
    const mkb::Rgb24& get_mult_color() const {
        return m_mult_color;
    }
    void set_mult_color(const mkb::Rgb24& mult_color) {
        m_mult_color = mult_color;
    }
    const mkb::Rgb24& get_add_color() const {
        return m_add_color;
    }
    void set_add_color(const mkb::Rgb24& add_color) {
        m_add_color = add_color;
    }
};
}// namespace ui
