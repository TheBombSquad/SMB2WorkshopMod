#include "ui_box.h"
#include "mkb.h"
#include "log.h"
#include "heap.h"

namespace ui_box {
    UIBox* ui_box_list[UI_BOX_LEN];
    u8 ui_box_count = 0;

    // Init the ui_box_list as empty.
    void init()
    {
        for (int i = 0; i < UI_BOX_LEN; i++) {
            ui_box_list[i] = nullptr;
        }
    }

    // Iterate through the ui_box_list to find any UIBox elements, display if they exist. TODO: Add 'visible', 'invisible', 'to be destroyed', etc states
    void disp()
    {
        for (int i = 0; i < ui_box_count; i++) {
            if (ui_box_list[i] != nullptr) ui_box_list[i]->disp();
        }
    }

    // Basic UIBox constructor
    UIBox::UIBox(float x, float y, float width, float height)
    {
        m_pos.x = x;
        m_pos.y = y;
        m_dimensions.x = width;
        m_dimensions.y = height;
        m_anim_type_1 = AnimType::ANIM_NONE;
    }

    // Display a UIBox
    void UIBox::disp()
    {
        tick(m_anim_type_1);
        tick(m_anim_type_2);
        tick(m_anim_type_3);

        mkb::init_ui_element_sprite_with_defaults();
        mkb::set_ui_element_sprite_pos(m_pos.x+(m_dimensions.x/2), m_pos.y+(m_dimensions.y/2));
        mkb::set_ui_element_sprite_scale(1, 1);
        mkb::set_ui_element_sprite_scale(m_dimensions.x/416, m_dimensions.y/176);
        mkb::set_ui_element_sprite_depth(0.10);
        mkb::set_ui_element_sprite_rot_z(m_rot_z);
        //mkb::draw_ui_box(0x5);
        draw_ui_box_ext(0x5);
    }

    // Run a particular tick function (or several) based off the UIBox animation type.
    void UIBox::tick(AnimType type)
    {
        switch (type) {
            case AnimType::ANIM_WIGGLE:
                anim_wiggle();
                break;
            case AnimType::ANIM_NONE:
            default:
            break;
        }
    }

    // Animation that rotates the UIBox back and forth along its center.
    // This will not function properly without draw_ui_box_ext.
    void UIBox::anim_wiggle()
    {
        if (m_counter_1 >= 32767) m_counter_1 = 0;

        m_rot_z = static_cast<s32>(1024*mkb::math_sin(1024*m_counter_1));
        m_counter_1++;
        mkb::OSReport("wiggle val %d, ctr %d\n", m_rot_z, m_counter_1);
    }

    // Pushes a new UIBox to the list. It's really a stack I shouldn't call it a list
    void push(UIBox* box)
    {
        if (ui_box_count < UI_BOX_LEN) {
            mkb::OSReport("creating test uibox %d\n", ui_box_count);
            ui_box_list[ui_box_count] = box;
            ui_box_count++;
        }
    }

    // Pops a UIBox from the stack and also frees up the memory used by it.
    void pop()
    {
        if (ui_box_count > 0) {
            mkb::OSReport("destroying test uibox %d\n", ui_box_count-1);
            UIBox* test = ui_box_list[ui_box_count-1];
            ui_box_list[ui_box_count-1] = nullptr;
            ui_box_count--;
            heap::free_to_heap(test);
        }
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

      mkb::ui_sprite_draw_req.id = texture_id;
      req = mkb::ui_sprite_draw_req;

      req.u1 = (6.0 / fill_width);
      req.v1 = (6.0 / fill_height);
      req.u2 = (1.0 - 6.0 / fill_width);
      req.v2 = (1.0 - 6.0 / fill_height);
      req.scale.x = (scale_x - 12.0) / fill_width;
      req.scale.y = (scale_y - 12.0) / fill_height;
      mkb::draw_sprite_draw_request(&req);

      req = mkb::ui_sprite_draw_req;

      req.pos.y = (mkb::ui_sprite_draw_req.pos.y - scale_y * 0.5) + 3.0;
      req.u1 = (6.0 / fill_width);
      req.v1 = 0.0;
      req.u2 = (1.0 - 6.0 / fill_width);
      req.v2 = (6.0 / fill_height);
      req.scale.x = (scale_x - 12.0) / fill_width;
      req.scale.y = (6.0 / fill_height);
      mkb::draw_sprite_draw_request(&req);

      req = mkb::ui_sprite_draw_req;

      req.pos.y = (mkb::ui_sprite_draw_req.pos.y + scale_y * 0.5) - 3.0;
      req.u1 = (6.0 / fill_width);
      req.v1 = (1.0 - 6.0 / fill_height);
      req.u2 = (1.0 - 6.0 / fill_width);
      req.v2 = 1.0;
      req.scale.x = (scale_x - 12.0) / fill_width;
      req.scale.y = (6.0 / fill_height);
      mkb::draw_sprite_draw_request(&req);

      req = mkb::ui_sprite_draw_req;

      req.pos.x = (mkb::ui_sprite_draw_req.pos.x - scale_x * 0.5) + 3.0;
      req.u1 = 0.0;
      req.v1 = (6.0 / fill_height);
      req.u2 = (6.0 / fill_width);
      req.v2 = (1.0 - 6.0 / fill_height);
      req.scale.x = (6.0 / fill_width);
      req.scale.y = (scale_y - 12.0) / fill_height;
      mkb::draw_sprite_draw_request(&req);

      req = mkb::ui_sprite_draw_req;

      req.pos.x = (mkb::ui_sprite_draw_req.pos.x + scale_x * 0.5) - 3.0;
      req.u1 = (1.0 - 6.0 / fill_width);
      req.v1 = (6.0 / fill_height);
      req.u2 = 1.0;
      req.v2 = (1.0 - 6.0 / fill_height);
      req.scale.x = (6.0 / fill_width);
      req.scale.y = (scale_y - 12.0) / fill_height;
      mkb::draw_sprite_draw_request(&req);

      req = mkb::ui_sprite_draw_req;

      req.pos.x = (mkb::ui_sprite_draw_req.pos.x - scale_x * 0.5) + 3.0;
      req.pos.y = (mkb::ui_sprite_draw_req.pos.y - scale_y * 0.5) + 3.0;
      req.u1 = 0.0;
      req.v1 = 0.0;
      req.u2 = (6.0 / fill_width);
      req.v2 = (6.0 / fill_height);
      req.scale.x = (6.0 / fill_width);
      req.scale.y = (6.0 / fill_height);
      mkb::draw_sprite_draw_request(&req);

      req = mkb::ui_sprite_draw_req;

      req.pos.x = (mkb::ui_sprite_draw_req.pos.x + scale_x * 0.5) - 3.0;
      req.pos.y = (mkb::ui_sprite_draw_req.pos.y - scale_y * 0.5) + 3.0;
      req.u1 = (1.0 - 6.0 / fill_width);
      req.v1 = 0.0;
      req.u2 = 1.0;
      req.v2 = (6.0 / fill_height);
      req.scale.x = (6.0 / fill_width);
      req.scale.y = (6.0 / fill_height);
      mkb::draw_sprite_draw_request(&req);

      req = mkb::ui_sprite_draw_req;

      req.pos.x = (mkb::ui_sprite_draw_req.pos.x - scale_x * 0.5) + 3.0;
      req.pos.y = (mkb::ui_sprite_draw_req.pos.y + scale_y * 0.5) - 3.0;
      req.u1 = 0.0;
      req.v1 = (1.0 - 6.0 / fill_height);
      req.u2 = (6.0 / fill_width);
      req.v2 = 1.0;
      req.scale.x = (6.0 / fill_width);
      req.scale.y = (6.0 / fill_height);
      mkb::draw_sprite_draw_request(&req);

      req = mkb::ui_sprite_draw_req;

      req.pos.x = (mkb::ui_sprite_draw_req.pos.x + scale_x * 0.5) - 3.0;
      req.pos.y = (mkb::ui_sprite_draw_req.pos.y + scale_y * 0.5) - 3.0;
      req.u1 = (1.0 - 6.0 / fill_width);
      req.v1 = (1.0 - 6.0 / fill_height);
      req.u2 = 1.0;
      req.v2 = 1.0;
      req.scale.x = (6.0 / fill_width);
      req.scale.y = (6.0 / fill_height);
      mkb::draw_sprite_draw_request(&req);
    }

}
