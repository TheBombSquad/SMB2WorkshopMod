#include "widget_text.h"

namespace ui {
void Text::disp() {
    Vec2d centered_pos;
    centered_pos.x = m_pos.x + (m_dimensions.x / 2);
    centered_pos.y = m_pos.y + (m_dimensions.y / 2);
    mkb::textdraw_reset();
    mkb::textdraw_set_font(mkb::FONT32_ASC_24x24);
    mkb::textdraw_set_flags(0x80000000);
    mkb::textdraw_set_drop_shadow();
    mkb::textdraw_set_mul_color((m_color.red << 16) + (m_color.green << 8) + m_color.blue);
    mkb::textdraw_set_font_style(m_font_style);
    mkb::textdraw_set_scale(m_scale.x, m_scale.y);
    mkb::textdraw_set_depth(0.01);
    mkb::textdraw_set_pos(centered_pos.x, centered_pos.y);
    mkb::textdraw_set_alignment(mkb::ALIGN_CENTER);
    mkb::textdraw_print(const_cast<char*>(m_text));
}

}// namespace ui
