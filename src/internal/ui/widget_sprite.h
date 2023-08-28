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
    mkb::Rgb24 m_mult_color = {0xff, 0xff, 0xff};
    mkb::Rgb24 m_add_color = {0x0, 0x0, 0x0};
};
}

