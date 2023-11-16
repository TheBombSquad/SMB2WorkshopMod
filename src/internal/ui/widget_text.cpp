#include "widget_text.h"
#include "internal/log.h"

namespace ui {
void Text::disp() {
    mkb::textdraw_reset();
    mkb::textdraw_set_font(mkb::FONT32_ASC_24x24);
    mkb::textdraw_set_flags(0x80000000);
    mkb::textdraw_set_drop_shadow();
    mkb::textdraw_set_mul_color((m_color.red << 16) + (m_color.green << 8) + m_color.blue);
    mkb::textdraw_set_font_style(m_font_style);
    mkb::textdraw_set_depth(m_depth);
    mkb::textdraw_set_pos(m_pos.x, m_pos.y);
    mkb::textdraw_set_alignment(m_alignment);
    m_drop_shadow ? mkb::textdraw_set_drop_shadow() : mkb::textdraw_clear_drop_shadow();

    // Vertically fit text to dimensions
    /*
    const float str_height = mkb::textdraw_get_pixel_height_of_string(const_cast<char*>(m_text));
    const float str_height_scaled = str_height * m_scale.y;

    if (str_height_scaled > m_dimensions.y) {
        m_scale.y = m_dimensions.y / str_height_scaled;
    }
     */

    mkb::textdraw_set_scale(m_scale.x, m_scale.y);
    m_dimensions.x = mkb::textdraw_get_pixel_width_of_string(const_cast<char*>(m_text));
    m_dimensions.y = mkb::textdraw_get_pixel_height_of_string(const_cast<char*>(m_text));
    mkb::textdraw_print(const_cast<char*>(m_text));
    // mkb::textdraw_print_and_fit_to_width(m_dimensions.x, const_cast<char*>(m_text));
}

}// namespace ui
