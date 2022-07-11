#include "scratch.h"
#include "pad.h"

namespace scratch
{

char* msg = "Achievement unlocked!";
char* msg1 = "Banana mania";
char* msg2_f = "Collect at least 15 bananas. (%d/15)";
char msg2[48];
u32 flash_color;
u8 fc;
mkb::Sprite* sprite_banana = nullptr;
bool sent = false;

void sprite_disp(mkb::Sprite* sprite) {

    mkb::sprintf(msg2, msg2_f, mkb::balls[0].banana_count);

    fc = static_cast<u8>((mkb::math_sin(sprite->para1)+1.0)*127.0);
    flash_color = fc + (fc<<8) + (fc<<16);

    sprite->para1 += 546*2;
    if (sprite->para2 < 16384) {
        sprite->pos.x = -100.0 + mkb::math_sin(sprite->para2)*225;
    }

    sprite->para2 += 546;

    if (sprite->para2 > 65536*2) {
        sprite->pos.x -= mkb::math_sin(sprite->para2)*100;
    }

    if (sprite->para2 > (65536*2)+16384) {
        mkb::destroy_sprite_with_unique_id(0x420);
        mkb::destroy_sprite_with_unique_id(0x421);
        return;
    }

    mkb::init_ui_box_sprite_with_defaults();
    mkb::set_ui_box_sprite_pos(sprite->pos.x+10, 100.0);
    mkb::set_ui_box_sprite_scale(0.6, 0.5);
    mkb::draw_ui_box(0x5);

    mkb::init_global_font_sprite_vars_with_defaults();
    mkb::set_global_font_sprite_type(sprite->font);
    mkb::set_global_font_sprite_alignment(sprite->alignment);
    mkb::set_global_font_sprite_flags(0x80000000);
    mkb::set_global_font_sprite_dimensions(0.60, 0.75);
    mkb::g_set_global_font_style(mkb::STYLE_BOLD_ITALIC);
    mkb::g_set_global_font_sprite_drop_shadow_flag();
    mkb::g_set_global_font_sprite_mult_color(0x00ffff00);
    mkb::set_global_font_sprite_add_color(flash_color);
    mkb::set_global_font_sprite_pos(sprite->pos.x+10, 75.0);
    mkb::draw_text_sprite_string((mkb::byte*)msg);

    mkb::init_global_font_sprite_vars_with_defaults();
    mkb::set_global_font_sprite_type(sprite->font);
    mkb::set_global_font_sprite_alignment(sprite->alignment);
    mkb::set_global_font_sprite_flags(0x80000000);
    mkb::set_global_font_sprite_dimensions(0.60, 0.75);
    mkb::g_set_global_font_sprite_drop_shadow_flag();
    mkb::g_set_global_font_sprite_mult_color(0x00ffff00);
    mkb::set_global_font_sprite_pos(sprite->pos.x+10, 100.0);
    mkb::draw_text_sprite_string((mkb::byte*)msg1);

    mkb::init_global_font_sprite_vars_with_defaults();
    mkb::set_global_font_sprite_type(sprite->font);
    mkb::set_global_font_sprite_alignment(sprite->alignment);
    mkb::set_global_font_sprite_flags(0x80000000);
    mkb::set_global_font_sprite_dimensions(0.45, 0.60);
    mkb::g_set_global_font_sprite_drop_shadow_flag();
    mkb::g_set_global_font_sprite_mult_color(0x00ffff00);
    mkb::set_global_font_sprite_pos(sprite->pos.x+10, 125.0);
    mkb::draw_text_sprite_string((mkb::byte*)msg2);

    if (sprite_banana != nullptr) {
        sprite_banana->pos.x = sprite->pos.x-60;
    }
}

void sprite_init() {
    mkb::Sprite* sprite = mkb::create_sprite();
    if (sprite != nullptr) {
        sprite_banana->unique_id = 0x421;
        (sprite->pos).x = 0;
        (sprite->pos).y = 0;
        sprite->alignment = mkb::ALIGN_CENTER;
        sprite->font = mkb::FONT32_ASC_24x24;
        sprite->disp_func = sprite_disp;
    }

    sprite_banana = mkb::create_sprite();
    if (sprite_banana != nullptr) {
        sprite_banana->unique_id = 0x420;
        sprite_banana->type = mkb::SPRT_BMP;
        sprite_banana->bmp = 0x501;
        sprite_banana->g_flags1 = sprite_banana->g_flags1 | 0x1000000;
        (sprite_banana->pos).x = 0;
        (sprite_banana->pos).y = 100;
        sprite_banana->alignment = mkb::ALIGN_CENTER;
    }

}

void init() {

}

void tick() {
    if (!sent && mkb::balls[0].banana_count >= 15) {
        sent = true;
        mkb::call_SoundReqID_arg_2(10);
        sprite_init();
    }
}

void disp() {}

}
