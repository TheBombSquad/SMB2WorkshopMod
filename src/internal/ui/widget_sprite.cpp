#include "widget_sprite.h"
#include "log.h"

namespace ui {
void Sprite::disp() {
    mkb::SpriteDrawRequest req;
    mkb::GXSetZMode_cached('\x01', mkb::GX_LEQUAL, '\x01');
    req.id = m_sprite_id;
    req.pos.x = m_pos.x;
    req.pos.y = m_pos.y;
    req.pos.z = m_depth;
    req.scale.x = m_scale.x;
    req.scale.y = m_scale.y;
    req.mult_color = (m_mult_color.red << 16) + (m_mult_color.green << 8) + m_mult_color.blue;
    req.add_color = (m_add_color.red << 16) + (m_add_color.green << 8) + m_add_color.blue;
    req.u1 = 0.0f;
    req.v1 = 0.0f;
    req.u2 = 1.0f;
    req.v2 = 1.0f;
    req.rot_z = m_z_rotation;
    req.alpha = m_alpha;
    req.g_unk1 = -1;

    if (!m_mirror) {
        req.flags = (0x20000) & 0xfffffffff0 | 10;
    }
    else {
        req.flags = (0x20000) & 0xfffffffff0 | 0x8000a;
    }

    mkb::GXSetZMode_cached('\x01', mkb::GX_ALWAYS, '\x01');
    mkb::draw_sprite_draw_request(&req);
}
}// namespace ui