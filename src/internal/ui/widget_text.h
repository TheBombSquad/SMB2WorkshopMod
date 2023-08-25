#pragma once

#include "internal/ui/widget.h"

namespace ui {

constexpr mkb::Rgb24 DEFAULT_YELLOW = {0xFF, 0xFF, 0x00};
constexpr mkb::Rgb24 UNSELECTED_YELLOW = {0x80, 0x80, 0x00};
constexpr mkb::Rgb24 DEFAULT_ORANGE = {0xFF, 0x80, 0x00};

class Text : public Widget {
public:
    Text(const Vec2d pos, const char* text) : Widget(pos), m_text(text) { m_scale = Vec2d{0.7, 1.0}; }
    virtual void disp() override;

protected:
    const char* m_text;
    mkb::FontStyle m_font_style = 0x0;
    mkb::Rgb24 m_color = DEFAULT_ORANGE;

public:
    mkb::FontStyle get_m_font_style() const {
        return m_font_style;
    }
    void set_m_font_style(mkb::FontStyle m_font_style) {
        Text::m_font_style = m_font_style;
    }
};

}// namespace ui
