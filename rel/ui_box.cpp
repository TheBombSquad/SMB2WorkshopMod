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
                if (e->val->get_state() == UIBoxState::STATE_DESTROY) {
                    Element<UIBox>* next = e->next;
                    ui_boxes.remove_first(e->val);
                    e = next;
                }
                else {
                    e->val->disp();
                    e = e->next;
                }
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
        m_scale.x = 1.0;
        m_scale.y = 1.0;
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
                        case AnimType::MODIFIER_ZOOM:
                            modifier_zoom(mod);
                            break;
                        case AnimType::MODIFIER_LIFETIME:
                            modifier_lifetime(mod);
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
            Vec2d centered_pos;
            centered_pos.x = m_pos.x+(m_dimensions.x/2);
            centered_pos.y = m_pos.y+(m_dimensions.y/2);

            mkb::mtxa_from_identity();
            mkb::mtxa_translate_xyz(centered_pos.x, centered_pos.y, 0.0);
            mkb::mtxa_scale_xyz(m_scale.x, m_scale.y, 1);
            mkb::mtxa_rotate_z(m_rot_z);
            mkb::mtxa_translate_neg_xyz(centered_pos.x, centered_pos.y, 0.0);
            mkb::GXLoadPosMtxImm(reinterpret_cast<float(*)[4]>(mkb::mtxa), 0);
            mkb::init_ui_element_sprite_with_defaults();
            mkb::set_ui_element_sprite_pos(centered_pos.x, centered_pos.y);
            mkb::set_ui_element_sprite_scale(1, 1);
            mkb::set_ui_element_sprite_scale(m_dimensions.x/416, m_dimensions.y/176);
            mkb::set_ui_element_sprite_depth(0.10);
            draw_ui_box_ext(0x5);
            mkb::textdraw_reset();
            mkb::textdraw_set_font(mkb::FONT32_ASC_24x24);
            mkb::textdraw_set_flags(0x80000000);
            mkb::textdraw_set_drop_shadow();
            mkb::textdraw_set_mul_color(0xff8000);
            mkb::textdraw_set_scale(0.7, 1);
            mkb::textdraw_set_depth(0.01);
            mkb::textdraw_set_pos(centered_pos.x, centered_pos.y);
            mkb::textdraw_set_alignment(mkb::ALIGN_CENTER);
            mkb::textdraw_print(m_title);
        }
    }


    // Make the UI box rotate around its centerpoint. Angle is in standard mkb s16 angle format, period is in seconds
    // Max period is 60 seconds
    void UIBox::set_wiggle_modifier(const u16 &angle, const float &period)
    {
        modifiers.append(new UIBoxModifier {
            .type = AnimType::MODIFIER_WIGGLE,
            .int_param_1 = angle,
            .int_param_2 = static_cast<u16>(1092*period),
        });
    }

    void UIBox::set_zoom_modifier(const u32 &time, const ZoomType &zoom_type, const u32 &delay)
    {
        modifiers.append(new UIBoxModifier {
            .type = AnimType::MODIFIER_ZOOM,
            .int_param_1 = static_cast<s32>(delay),
            .int_param_2 = static_cast<s32>(time),
            .float_param_2 = (zoom_type == ZoomType::ZOOM_IN) ? 1.0f : -1.0f,
            .float_param_3 = (zoom_type == ZoomType::ZOOM_IN) ? 0.0f : 1.0f,
        });
    }

    void UIBox::set_lifetime_modifier(const u32 &time)
    {
        modifiers.append(new UIBoxModifier {
            .type = AnimType::MODIFIER_LIFETIME,
            .int_param_1 = static_cast<s32>(time),
        });
    }

    UIBoxState UIBox::get_state() const
    {
        return state;
    }

    void UIBox::set_title(char* title)
    {
        m_title = title;
    }

    void UIBox::set_subtitle(char* subtitle)
    {
        m_subtitle = subtitle;
    }

    void UIBox::set_message(char* message)
    {
        m_message = message;
    }

    void UIBox::set_state(const UIBoxState &state)
    {
        this->state = state;
    }

    void UIBox::set_scale(const float &x, const float &y)
    {
        m_scale.x = x;
        m_scale.y = y;
    }

    // Animation that rotates the UIBox back and forth along its center.
    // int_param_1 -> angle to rotate in s16
    // int_param_2 -> period of rotation in frames
    void UIBox::modifier_wiggle(UIBoxModifier* modifier)
    {
        if (modifier->counter >= 65535) modifier->counter = 0;

        m_rot_z = static_cast<s32>(modifier->int_param_1*mkb::math_sin(modifier->int_param_2*modifier->counter));
        modifier->counter++;
    }

    // Animation that adjusts the scale of the UIBox based on the counter.
    #define CURRENT_FRAME modifier->counter
    #define DELAY_FRAMES (u32)modifier->int_param_1
    #define DURATION (u32)modifier->int_param_2
    #define AMPLITUDE modifier->float_param_2
    #define OFFSET modifier->float_param_3
    void UIBox::modifier_zoom(UIBoxModifier* modifier)
    {

        MOD_ASSERT_MSG(DELAY_FRAMES >= 0, "Zoom modifier delay cannot be negative");
        MOD_ASSERT_MSG(DURATION > 0, "Zoom modifier duration cannot be negative or zero");

        if (CURRENT_FRAME < DELAY_FRAMES) {
            CURRENT_FRAME++;
            return;
        }

        if (CURRENT_FRAME-DELAY_FRAMES > DURATION) return;

        float scale = AMPLITUDE *
                      mkb::sin((float)(CURRENT_FRAME - DELAY_FRAMES) *
                               (3.14159/2.0) *
                               (1.0f/(float)(DURATION)))
                      + OFFSET;

        m_scale.x = scale;
        m_scale.y = scale;

        CURRENT_FRAME++;
    }

    // Destroys the UIBox after int_param_1 frames
    void UIBox::modifier_lifetime(UIBoxModifier *modifier)
    {
        if (modifier->counter < static_cast<u32>(modifier->int_param_1)) {
            modifier->counter++;
            return;
        }
        else {
            set_state(UIBoxState::STATE_DESTROY);
            return;
        }
    }
    #undef CURRENT_FRAME
    #undef DELAY_FRAMES
    #undef DURATION
    #undef AMPLITUDE
    #undef OFFSET

    // A re-implementation ~~and extension of~~ mkb::draw_ui_box.
    // The commented parts rotate the individual slices about the center of the fill.
    // However, it turns out you can just use GPU matrix stuff to do this so this ended up being pointless...
    // I'm keeping this around because it's more readable than the decomp which has compiler optimization stuff included,
    // in the event that I or someone might want to make changes or extensions to this later.
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

    float lerp(float f1, float f2, float t)
    {
        return (1-t)*f1+t*f2;
    }

}
