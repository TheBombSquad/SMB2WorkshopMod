#include "ui_box.h"
#include "mkb.h"
#include "log.h"
#include "heap.h"
#include "vecutil.h"
#include "list.h"
#define SIN(x) mkb::math_sin(x)
#define COS(x) SIN(16384-x)

namespace ui_box {
    List<UIBox> ui_boxes = List<UIBox>();
    // Init the ui_box_stack as empty.
    void init()
    {
    }

    // Iterate through the ui_box_stack to find any UIBox elements, display if they exist. TODO: Handle 'visible', 'invisible', 'to be destroyed', etc states
    void disp_all()
    {
        if (!ui_boxes.empty()) {
            Element<UIBox>* e = ui_boxes.first;
            do {
                e->val->disp();
                e = e->next;
            }
            while (e != nullptr);
        }
    }

    // Basic UIBox constructor
    UIBox::UIBox(float x, float y, float width, float height)
    {
        m_pos.x = x;
        m_pos.y = y;
        m_dimensions.x = width;
        m_dimensions.y = height;
    }

    // Display a UIBox, also run all modifiers
    void UIBox::disp()
    {
        if (state != UIBoxState::STATE_VISIBLE_NO_TICK && state != UIBoxState::STATE_INVISIBLE_NO_TICK) {
            if (!modifiers.empty()) {
                Element<UIBoxModifier>* e = modifiers.first;
                do {
                    UIBoxModifier* mod = e->val;
                    switch (mod->type) {
                        case AnimType::MODIFIER_WIGGLE:
                            modifier_wiggle(mod);
                            break;
                        case AnimType::MODIFIER_NONE:
                        default:
                        break;
                    }
                    e = e->next;
                }
                while (e != nullptr);
            }
        }

        if (state != UIBoxState::STATE_INVISIBLE && state != UIBoxState::STATE_INVISIBLE_NO_TICK) {
            mkb::init_ui_element_sprite_with_defaults();
            mkb::set_ui_element_sprite_pos(m_pos.x+(m_dimensions.x/2), m_pos.y+(m_dimensions.y/2));
            mkb::set_ui_element_sprite_scale(1, 1);
            mkb::set_ui_element_sprite_scale(m_dimensions.x/416, m_dimensions.y/176);
            mkb::set_ui_element_sprite_depth(0.10);
            mkb::set_ui_element_sprite_rot_z(m_rot_z);
            draw_ui_box_ext(0x5);
        }
    }

    void UIBox::set_state(UIBoxState state)
    {
        this->state = state;
    }

    // Make the UI box rotate around its centerpoint. Angle is in standard mkb s16 angle format, period is in seconds
    // Max period is 60 seconds
    void UIBox::set_wiggle_modifier(u16 angle, float period)
    {
        // TODO: fix
        modifiers.append(new UIBoxModifier {
            .type = AnimType::MODIFIER_WIGGLE,
            .int_param_1 = angle,
            .int_param_2 = static_cast<u16>(1092*period),
        });
    }

    void UIBox::set_fade_in_zoom_modifier(float time)
    {
        modifiers.append(new UIBoxModifier {
            .type = AnimType::MODIFIER_FADE_IN_ZOOM,
            .int_param_1 = static_cast<u16>(1092*time),
        });
    }

    UIBoxState UIBox::getState() const
    {
        return state;
    }

    // Animation that rotates the UIBox back and forth along its center.
    // This will not function properly without draw_ui_box_ext.
    void UIBox::modifier_wiggle(UIBoxModifier* modifier)
    {
        if (modifier->counter >= 65535) modifier->counter = 0;

        m_rot_z = static_cast<s32>(modifier->int_param_1*mkb::math_sin(modifier->int_param_2*modifier->counter));
        modifier->counter++;
    }

    void UIBox::modifier_fade_in_zoom(UIBoxModifier* modifier)
    {

    }

    // A re-implementation and extension of mkb::draw_ui_box.
    // This re-implementation allows for a box to rotate about its center.
    // The original implementation would rotate the box's slice sprites about their own centers,
    // rather than the center of the entire box.
    void draw_ui_box_ext(u32 texture_id)
    {
      u32 tex_id_shifted = texture_id >> 8;

      u32 fill_width_u32, fill_height_u32;
      double fill_width, fill_height, scale_x, scale_y;

      if (mkb::bmp_infos[tex_id_shifted].is_loaded == 0) {
        fill_width_u32 = 0;
      }
      else {
        fill_width_u32 =
             (u32)(mkb::bmp_infos[tex_id_shifted].tpl)->texture_headers[texture_id & 0xff].width;
      }
      fill_width = static_cast<double>(fill_width_u32);

      if (mkb::bmp_infos[tex_id_shifted].is_loaded == 0) {
        fill_height_u32 = 0;
      }
      else {
        fill_height_u32 =
             (u32)(mkb::bmp_infos[tex_id_shifted].tpl)->texture_headers[texture_id & 0xff].height;
      }
      fill_height = static_cast<double>(fill_height_u32);

      scale_x = (double)(float)(fill_width * (double)mkb::ui_sprite_draw_req.scale.x);
      scale_y = (double)(float)(fill_height * (double)mkb::ui_sprite_draw_req.scale.y);

      // Alignment flag stuff.
      // TODO: Figure out
      u8 g_align_flags_1 = mkb::ui_sprite_draw_req.flags & 3;
      if (g_align_flags_1 != 2) {
        if (g_align_flags_1 < 2) {
          if (g_align_flags_1 != 0) {
            mkb::ui_sprite_draw_req.pos.x = mkb::ui_sprite_draw_req.pos.x + (float)(scale_x * 0.5);
          }
        }
        else if (g_align_flags_1 < 4) {
          mkb::ui_sprite_draw_req.pos.x = mkb::ui_sprite_draw_req.pos.x - (float)(scale_x * 0.5);
        }
      }
      u8 g_align_flags_2 = (mkb::ui_sprite_draw_req.flags & 0xc) >> 2;
      if (g_align_flags_2 != 2) {
        if (g_align_flags_2 < 2) {
          if (g_align_flags_2 != 0) {
            mkb::ui_sprite_draw_req.pos.y = mkb::ui_sprite_draw_req.pos.y + (float)(scale_y * 0.5);
          }
        }
        else if (g_align_flags_2 < 4) {
          mkb::ui_sprite_draw_req.pos.y = mkb::ui_sprite_draw_req.pos.y - (float)(scale_y * 0.5);
        }
      }

      mkb::ui_sprite_draw_req.flags = (mkb::ui_sprite_draw_req.flags & 0xffffff00) | 10;

      mkb::SpriteDrawRequest req;
      Vec orig_pos = mkb::ui_sprite_draw_req.pos;
      float dist_from_orig;
      Vec corner_pos;

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

      dist_from_orig = orig_pos.y - ((orig_pos.y - scale_y * 0.5) + 3.0);
      req.pos.x -= dist_from_orig*SIN(req.rot_z);
      req.pos.y -= dist_from_orig*COS(req.rot_z);

      req.u1 = (6.0 / fill_width);
      req.v1 = 0.0;
      req.u2 = (1.0 - 6.0 / fill_width);
      req.v2 = (6.0 / fill_height);
      req.scale.x = (scale_x - 12.0) / fill_width;
      req.scale.y = (6.0 / fill_height);
      mkb::draw_sprite_draw_request(&req);


      // Bottom
      req = mkb::ui_sprite_draw_req;

      dist_from_orig = orig_pos.y - ((orig_pos.y + scale_y * 0.5) - 3.0);
      req.pos.x -= dist_from_orig*SIN(req.rot_z);
      req.pos.y -= dist_from_orig*COS(req.rot_z);

      req.u1 = (6.0 / fill_width);
      req.v1 = (1.0 - 6.0 / fill_height);
      req.u2 = (1.0 - 6.0 / fill_width);
      req.v2 = 1.0;
      req.scale.x = (scale_x - 12.0) / fill_width;
      req.scale.y = (6.0 / fill_height);
      mkb::draw_sprite_draw_request(&req);


      // Left
      req = mkb::ui_sprite_draw_req;

      dist_from_orig = orig_pos.x - ((orig_pos.x - scale_x * 0.5) + 3.0);
      req.pos.x -= dist_from_orig*COS(req.rot_z);
      req.pos.y += dist_from_orig*SIN(req.rot_z);

      req.u1 = 0.0;
      req.v1 = (6.0 / fill_height);
      req.u2 = (6.0 / fill_width);
      req.v2 = (1.0 - 6.0 / fill_height);
      req.scale.x = (6.0 / fill_width);
      req.scale.y = (scale_y - 12.0) / fill_height;
      mkb::draw_sprite_draw_request(&req);


      // Right
      req = mkb::ui_sprite_draw_req;

      dist_from_orig = orig_pos.x - ((orig_pos.x + scale_x * 0.5) - 3.0);
      req.pos.x -= dist_from_orig*COS(req.rot_z);
      req.pos.y += dist_from_orig*SIN(req.rot_z);

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

      corner_pos = VEC_SUB(req.pos, orig_pos);

      req.pos.x = orig_pos.x + (corner_pos.x*COS(req.rot_z) + corner_pos.y*SIN(req.rot_z));
      req.pos.y = orig_pos.y + (corner_pos.y*COS(req.rot_z) - corner_pos.x*SIN(req.rot_z));

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

      corner_pos = VEC_SUB(req.pos, orig_pos);

      req.pos.x = orig_pos.x + (corner_pos.x*COS(req.rot_z) + corner_pos.y*SIN(req.rot_z));
      req.pos.y = orig_pos.y + (corner_pos.y*COS(req.rot_z) - corner_pos.x*SIN(req.rot_z));

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

      corner_pos = VEC_SUB(req.pos, orig_pos);

      req.pos.x = orig_pos.x + (corner_pos.x*COS(req.rot_z) + corner_pos.y*SIN(req.rot_z));
      req.pos.y = orig_pos.y + (corner_pos.y*COS(req.rot_z) - corner_pos.x*SIN(req.rot_z));

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

      corner_pos = VEC_SUB(req.pos, orig_pos);

      req.pos.x = orig_pos.x + (corner_pos.x*COS(req.rot_z) + corner_pos.y*SIN(req.rot_z));
      req.pos.y = orig_pos.y + (corner_pos.y*COS(req.rot_z) - corner_pos.x*SIN(req.rot_z));

      req.u1 = (1.0 - 6.0 / fill_width);
      req.v1 = (1.0 - 6.0 / fill_height);
      req.u2 = 1.0;
      req.v2 = 1.0;
      req.scale.x = (6.0 / fill_width);
      req.scale.y = (6.0 / fill_height);
      mkb::draw_sprite_draw_request(&req);
    }

    float lerp(float f1, float f2, float t)
    {
        return (1-t)*f1+t*f2;
    }

}
