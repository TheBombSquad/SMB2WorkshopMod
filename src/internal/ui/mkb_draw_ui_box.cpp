#include "mkb_draw_ui_box.h"

#include "mkb/mkb.h"

namespace ui {

// A re-implementation ~~and extension of~~ mkb::draw_ui_box.
// The commented parts rotate the individual slices about the center of the fill.
// However, it turns out you can just use GPU matrix stuff to do this so this ended up being pointless...
// I'm keeping this around because it's more readable than the decomp which has compiler optimization stuff included,
// in the event that I or someone might want to make changes or extensions to this later.
void draw_ui_box_ext(u32 texture_id) {
    u32 tex_id_shifted = texture_id >> 8;

    u32 fill_width_u32, fill_height_u32;
    double fill_width, fill_height, scale_x, scale_y;

    if (mkb::bmp_infos[tex_id_shifted].is_loaded == 0) {
        fill_width_u32 = 0;
    }
    else {
        fill_width_u32 =
            (u32) (mkb::bmp_infos[tex_id_shifted].tpl)->texture_headers[texture_id & 0xff].width;
    }
    fill_width = static_cast<double>(fill_width_u32);

    if (mkb::bmp_infos[tex_id_shifted].is_loaded == 0) {
        fill_height_u32 = 0;
    }
    else {
        fill_height_u32 =
            (u32) (mkb::bmp_infos[tex_id_shifted].tpl)->texture_headers[texture_id & 0xff].height;
    }
    fill_height = static_cast<double>(fill_height_u32);

    scale_x = (double) (float) (fill_width * (double) mkb::ui_sprite_draw_req.scale.x);
    scale_y = (double) (float) (fill_height * (double) mkb::ui_sprite_draw_req.scale.y);

    // Alignment flag stuff.
    // TODO: Figure out
    u8 g_align_flags_1 = mkb::ui_sprite_draw_req.flags & 3;
    if (g_align_flags_1 != 2) {
        if (g_align_flags_1 < 2) {
            if (g_align_flags_1 != 0) {
                mkb::ui_sprite_draw_req.pos.x = mkb::ui_sprite_draw_req.pos.x + (float) (scale_x * 0.5);
            }
        }
        else if (g_align_flags_1 < 4) {
            mkb::ui_sprite_draw_req.pos.x = mkb::ui_sprite_draw_req.pos.x - (float) (scale_x * 0.5);
        }
    }
    u8 g_align_flags_2 = (mkb::ui_sprite_draw_req.flags & 0xc) >> 2;
    if (g_align_flags_2 != 2) {
        if (g_align_flags_2 < 2) {
            if (g_align_flags_2 != 0) {
                mkb::ui_sprite_draw_req.pos.y = mkb::ui_sprite_draw_req.pos.y + (float) (scale_y * 0.5);
            }
        }
        else if (g_align_flags_2 < 4) {
            mkb::ui_sprite_draw_req.pos.y = mkb::ui_sprite_draw_req.pos.y - (float) (scale_y * 0.5);
        }
    }

    mkb::ui_sprite_draw_req.flags = (mkb::ui_sprite_draw_req.flags & 0xffffff00) | 10;

    mkb::SpriteDrawRequest req;

    Vec orig_pos = mkb::ui_sprite_draw_req.pos;
    /*
    float dist_from_orig;
    Vec corner_pos;
    */

    // Fill
    mkb::ui_sprite_draw_req.id = texture_id;
    req = mkb::ui_sprite_draw_req;

    req.u1 = (6.0 / fill_width);
    req.v1 = (6.0 / fill_height);
    req.u2 = (1.0 - 6.0 / fill_width);
    req.v2 = (1.0 - 6.0 / fill_height);
    req.scale.x = (scale_x - 12.0) / fill_width;
    req.scale.y = (scale_y - 12.0) / fill_height;
    mkb::draw_sprite_draw_request(&req);


    // Top
    req = mkb::ui_sprite_draw_req;

    req.pos.y = ((orig_pos.y - scale_y * 0.5) + 3.0);

    /*
    dist_from_orig = orig_pos.y - ((orig_pos.y - scale_y * 0.5) + 3.0);
    req.pos.x -= dist_from_orig*SIN(req.rot_z);
    req.pos.y -= dist_from_orig*COS(req.rot_z);
    */

    req.u1 = (6.0 / fill_width);
    req.v1 = 0.0;
    req.u2 = (1.0 - 6.0 / fill_width);
    req.v2 = (6.0 / fill_height);
    req.scale.x = (scale_x - 12.0) / fill_width;
    req.scale.y = (6.0 / fill_height);
    mkb::draw_sprite_draw_request(&req);


    // Bottom
    req = mkb::ui_sprite_draw_req;

    req.pos.y = ((orig_pos.y + scale_y * 0.5) - 3.0);
    /*
    dist_from_orig = orig_pos.y - ((orig_pos.y + scale_y * 0.5) - 3.0);
    req.pos.x -= dist_from_orig*SIN(req.rot_z);
    req.pos.y -= dist_from_orig*COS(req.rot_z);
    */

    req.u1 = (6.0 / fill_width);
    req.v1 = (1.0 - 6.0 / fill_height);
    req.u2 = (1.0 - 6.0 / fill_width);
    req.v2 = 1.0;
    req.scale.x = (scale_x - 12.0) / fill_width;
    req.scale.y = (6.0 / fill_height);
    mkb::draw_sprite_draw_request(&req);


    // Left
    req = mkb::ui_sprite_draw_req;

    req.pos.x = ((orig_pos.x - scale_x * 0.5) + 3.0);
    /*
    dist_from_orig = orig_pos.x - ((orig_pos.x - scale_x * 0.5) + 3.0);
    req.pos.x -= dist_from_orig*COS(req.rot_z);
    req.pos.y += dist_from_orig*SIN(req.rot_z);
    */

    req.u1 = 0.0;
    req.v1 = (6.0 / fill_height);
    req.u2 = (6.0 / fill_width);
    req.v2 = (1.0 - 6.0 / fill_height);
    req.scale.x = (6.0 / fill_width);
    req.scale.y = (scale_y - 12.0) / fill_height;
    mkb::draw_sprite_draw_request(&req);


    // Right
    req = mkb::ui_sprite_draw_req;

    req.pos.x = ((orig_pos.x + scale_x * 0.5) - 3.0);
    /*
    dist_from_orig = orig_pos.x - ((orig_pos.x + scale_x * 0.5) - 3.0);
    req.pos.x -= dist_from_orig*COS(req.rot_z);
    req.pos.y += dist_from_orig*SIN(req.rot_z);
    */

    req.u1 = (1.0 - 6.0 / fill_width);
    req.v1 = (6.0 / fill_height);
    req.u2 = 1.0;
    req.v2 = (1.0 - 6.0 / fill_height);
    req.scale.x = (6.0 / fill_width);
    req.scale.y = (scale_y - 12.0) / fill_height;
    mkb::draw_sprite_draw_request(&req);


    // Top Left
    req = mkb::ui_sprite_draw_req;

    req.pos.x = (mkb::ui_sprite_draw_req.pos.x - scale_x * 0.5) + 3.0;
    req.pos.y = (mkb::ui_sprite_draw_req.pos.y - scale_y * 0.5) + 3.0;

    /*
    corner_pos = VEC_SUB(req.pos, orig_pos);

    req.pos.x = orig_pos.x + (corner_pos.x*COS(req.rot_z) + corner_pos.y*SIN(req.rot_z));
    req.pos.y = orig_pos.y + (corner_pos.y*COS(req.rot_z) - corner_pos.x*SIN(req.rot_z));
    */

    req.u1 = 0.0;
    req.v1 = 0.0;
    req.u2 = (6.0 / fill_width);
    req.v2 = (6.0 / fill_height);
    req.scale.x = (6.0 / fill_width);
    req.scale.y = (6.0 / fill_height);
    mkb::draw_sprite_draw_request(&req);

    // Top Right
    req = mkb::ui_sprite_draw_req;

    req.pos.x = (mkb::ui_sprite_draw_req.pos.x + scale_x * 0.5) - 3.0;
    req.pos.y = (mkb::ui_sprite_draw_req.pos.y - scale_y * 0.5) + 3.0;

    /*
    corner_pos = VEC_SUB(req.pos, orig_pos);

    req.pos.x = orig_pos.x + (corner_pos.x*COS(req.rot_z) + corner_pos.y*SIN(req.rot_z));
    req.pos.y = orig_pos.y + (corner_pos.y*COS(req.rot_z) - corner_pos.x*SIN(req.rot_z));
    */

    req.u1 = (1.0 - 6.0 / fill_width);
    req.v1 = 0.0;
    req.u2 = 1.0;
    req.v2 = (6.0 / fill_height);
    req.scale.x = (6.0 / fill_width);
    req.scale.y = (6.0 / fill_height);
    mkb::draw_sprite_draw_request(&req);

    // Bottom Left
    req = mkb::ui_sprite_draw_req;

    req.pos.x = (mkb::ui_sprite_draw_req.pos.x - scale_x * 0.5) + 3.0;
    req.pos.y = (mkb::ui_sprite_draw_req.pos.y + scale_y * 0.5) - 3.0;

    /*
    corner_pos = VEC_SUB(req.pos, orig_pos);

    req.pos.x = orig_pos.x + (corner_pos.x*COS(req.rot_z) + corner_pos.y*SIN(req.rot_z));
    req.pos.y = orig_pos.y + (corner_pos.y*COS(req.rot_z) - corner_pos.x*SIN(req.rot_z));
    */

    req.u1 = 0.0;
    req.v1 = (1.0 - 6.0 / fill_height);
    req.u2 = (6.0 / fill_width);
    req.v2 = 1.0;
    req.scale.x = (6.0 / fill_width);
    req.scale.y = (6.0 / fill_height);
    mkb::draw_sprite_draw_request(&req);

    // Bottom Right
    req = mkb::ui_sprite_draw_req;

    req.pos.x = (mkb::ui_sprite_draw_req.pos.x + scale_x * 0.5) - 3.0;
    req.pos.y = (mkb::ui_sprite_draw_req.pos.y + scale_y * 0.5) - 3.0;

    /*
    corner_pos = VEC_SUB(req.pos, orig_pos);

    req.pos.x = orig_pos.x + (corner_pos.x*COS(req.rot_z) + corner_pos.y*SIN(req.rot_z));
    req.pos.y = orig_pos.y + (corner_pos.y*COS(req.rot_z) - corner_pos.x*SIN(req.rot_z));
    */

    req.u1 = (1.0 - 6.0 / fill_width);
    req.v1 = (1.0 - 6.0 / fill_height);
    req.u2 = 1.0;
    req.v2 = 1.0;
    req.scale.x = (6.0 / fill_width);
    req.scale.y = (6.0 / fill_height);
    mkb::draw_sprite_draw_request(&req);
}

}// namespace ui