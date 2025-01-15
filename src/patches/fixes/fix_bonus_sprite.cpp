#include "fix_bonus_sprite.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace fix_bonus_sprite {

TICKABLE_DEFINITION((
        .name = "fix-bonus-stage-sprite",
        .description = "Bonus stage sprite fix",
        .init_main_loop = init_main_loop, ))

static patch::Tramp<decltype(&mkb::create_hud_sprites)> s_create_hud_sprites_tramp;

void create_bonus_sprite(void) {
    // Check if we're on a bonus stage and if the sprite already exists
    mkb::Sprite *special_stage_text_sprite = mkb::get_sprite_with_unique_id(mkb::SPRITE_SPECIAL_STAGE);
      if ((special_stage_text_sprite == (mkb::Sprite *)0x0) &&
      ((mkb::mode_info.ball_mode & mkb::BALLMODE_ON_BONUS_STAGE) != mkb::BALLMODE_NONE)) {
        // Create the sprite
    mkb::Sprite *sprite;
    sprite = mkb::create_sprite();
    if (sprite != (mkb::Sprite*) 0x0) {
        sprite->unique_id = mkb::SPRITE_SPECIAL_STAGE;
        (sprite->pos).x = 500.0;
        (sprite->pos).y = 452.0;
        (sprite->mult_color).red = 0xff;
        (sprite->mult_color).green = 0x80;
        (sprite->mult_color).blue = '\0';
        sprite->font = mkb::FONT_ASC_72x64;
        sprite->alignment = mkb::ALIGN_CENTER;
        sprite->width = 0.3;
        sprite->height = 0.3;
        sprite->g_flags1 = sprite->g_flags1 | 0x1001000;
        sprite->widescreen_translation_x = 0x27f;
        mkb::strcpy(sprite->text, "BONUS STAGE");
    }
      }
}

void init_main_loop() {
    // Hook the function which creates HUD sprites and call our new sprite create function
    patch::hook_function(s_create_hud_sprites_tramp, mkb::create_hud_sprites, []() {
        create_bonus_sprite();
        s_create_hud_sprites_tramp.dest();
    });
}

}// namespace fix_bonus_sprite
