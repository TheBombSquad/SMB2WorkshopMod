#include "widget_window.h"

#include "internal/ui/mkb_draw_ui_box.h"

namespace ui {

Window::Window(const Vec2d pos, const Vec2d dimensions) : Container(pos, dimensions) {
    m_margin = 8;// Bigger default margin for window borders
}

Window::~Window() = default;

void ui::Window::disp() {
    Vec2d centered_pos;
    centered_pos.x = m_pos.x + (m_dimensions.x / 2);
    centered_pos.y = m_pos.y + (m_dimensions.y / 2);

    mkb::mtxa_from_identity();
    mkb::mtxa_translate_xyz(centered_pos.x, centered_pos.y, 0.0);
    mkb::mtxa_scale_xyz(m_scale.x, m_scale.y, 1);
    mkb::mtxa_rotate_z(m_z_rotation);
    mkb::mtxa_translate_neg_xyz(centered_pos.x, centered_pos.y, 0.0);
    mkb::GXLoadPosMtxImm(reinterpret_cast<float(*)[4]>(mkb::mtxa), 0);
    mkb::init_ui_element_sprite_with_defaults();
    mkb::set_ui_element_sprite_pos(centered_pos.x, centered_pos.y);
    mkb::set_ui_element_sprite_scale(1, 1);
    mkb::set_ui_element_sprite_scale(m_dimensions.x / 416, m_dimensions.y / 176);
    mkb::set_ui_element_sprite_depth(m_depth);
    draw_ui_box_ext(0x5);
}

}// namespace ui