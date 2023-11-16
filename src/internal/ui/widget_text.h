#pragma once

#include "internal/ui/widget.h"

namespace ui {

constexpr mkb::Rgb24 DEFAULT_YELLOW = {0xFF, 0xFF, 0x00};
constexpr mkb::Rgb24 UNSELECTED_YELLOW = {0x80, 0x80, 0x00};
constexpr mkb::Rgb24 DEFAULT_ORANGE = {0xFF, 0x80, 0x00};

class Text : public Widget {
public:
    Text(const char* text) : Widget(), m_text(text) {}
    Text(const char* text, const Vec2d pos) : Widget(pos), m_text(text) {}
    virtual void disp() override;

protected:
    const char* m_text;
    mkb::FontStyle m_font_style = 0x0;
    int m_spacing = 1;
    mkb::Rgb24 m_color = DEFAULT_ORANGE;
    bool m_drop_shadow = true;
    mkb::SpriteAlignment m_alignment = mkb::ALIGN_LOWER_CENTER;

public:
    mkb::FontStyle get_m_font_style() const {
        return m_font_style;
    }
    void set_font_style(mkb::FontStyle m_font_style) {
        Text::m_font_style = m_font_style;
    }
    int get_spacing() const {
        return m_spacing;
    }
    void set_spacing(int spacing) {
        m_spacing = spacing;
    }
    const mkb::Rgb24& get_color() const {
        return m_color;
    }
    void set_color(const mkb::Rgb24& color) {
        m_color = color;
    }
    bool has_drop_shadow() const {
        return m_drop_shadow;
    }
    void set_drop_shadow(bool drop_shadow) {
        m_drop_shadow = drop_shadow;
    }
    mkb::SpriteAlignment get_alignment() const {
        return m_alignment;
    }
    void set_alignment(mkb::SpriteAlignment alignment) {
        m_alignment = alignment;
    }
};

}// namespace ui
