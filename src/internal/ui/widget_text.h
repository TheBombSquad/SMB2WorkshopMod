#pragma once

#include "internal/ui/widget.h"

namespace ui {

enum TextAlignment {
    LEFT,
    CENTER,
    RIGHT
};

constexpr mkb::Rgb24 DEFAULT_YELLOW = {0xFF, 0xFF, 0x00};
constexpr mkb::Rgb24 UNSELECTED_YELLOW = {0x80, 0x80, 0x00};
constexpr mkb::Rgb24 DEFAULT_ORANGE = {0xFF, 0x80, 0x00};

class Text : public Widget {
public:
    Text(const char* text) : Widget(), m_text(text) {
        // Calculate dimensions
        mkb::textdraw_reset();
        mkb::textdraw_set_font(mkb::FONT32_ASC_24x24);
        mkb::textdraw_set_flags(0x80000000);
        mkb::textdraw_set_drop_shadow();
        mkb::textdraw_set_font_style(m_font_style);
        mkb::textdraw_set_depth(m_depth);
        mkb::textdraw_set_pos(m_pos.x, m_pos.y);
        mkb::textdraw_set_alignment(m_alignment);
        mkb::textdraw_set_scale(m_scale.x, m_scale.y);
        m_dimensions.x = mkb::textdraw_get_pixel_width_of_string(const_cast<char*>(m_text));
        m_dimensions.y = mkb::textdraw_get_pixel_height_of_string(const_cast<char*>(m_text));
    }
    Text(const char* text, const Vec2d pos) : Widget(pos), m_text(text) {
        // Calculate dimensions
        mkb::textdraw_reset();
        mkb::textdraw_set_font(mkb::FONT32_ASC_24x24);
        mkb::textdraw_set_flags(0x80000000);
        mkb::textdraw_set_drop_shadow();
        mkb::textdraw_set_font_style(m_font_style);
        mkb::textdraw_set_depth(m_depth);
        mkb::textdraw_set_pos(m_pos.x, m_pos.y);
        mkb::textdraw_set_alignment(m_alignment);
        mkb::textdraw_set_scale(m_scale.x, m_scale.y);
        m_dimensions.x = mkb::textdraw_get_pixel_width_of_string(const_cast<char*>(m_text));
        m_dimensions.y = mkb::textdraw_get_pixel_height_of_string(const_cast<char*>(m_text));
    }
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
    void set_alignment(ui::TextAlignment alignment) {
        switch (alignment) {
            case LEFT:
                // Yes, I know this looks wrong, but with the way the UI coordinate system is set up, it's correct
                m_alignment = mkb::ALIGN_LOWER_RIGHT;
                break;
            case CENTER:
                // Ditto
                m_alignment = mkb::ALIGN_LOWER_CENTER;
                break;
            case RIGHT:
                // Ditto
                m_alignment = mkb::ALIGN_LOWER_LEFT;
                break;
        }
        m_alignment = alignment;
    }
};

}// namespace ui
