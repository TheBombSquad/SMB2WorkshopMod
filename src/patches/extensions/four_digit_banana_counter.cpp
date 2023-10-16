#include "four_digit_banana_counter.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace four_digit_banana_counter {

TICKABLE_DEFINITION((.name = "four-digit-banana-counter",
                     .description = "Four-digit banana counter",
                     .init_main_loop = init_main_loop))

// Assigns our own parameters to the banana counter sprites to adapt their
// positions for the extra digit and scale the wooden board underneath the
// counter.
void create_new_banana_counter_sprites(u8* status, mkb::Sprite* pSVar1) {
    pSVar1 = mkb::create_sprite();
    if (pSVar1 != (mkb::Sprite*) 0x0) {
        pSVar1->type = mkb::SPRT_BMP;
        pSVar1->width = 1.23;
        if (mkb::main_game_mode == mkb::CHALLENGE_MODE) {
            (pSVar1->pos).x = 498.0;
        }
        else {
            (pSVar1->pos).x = 527.0;
        }
        (pSVar1->pos).y = 24.0;
        pSVar1->bmp = 0x502;
        pSVar1->depth = 0.11;
        pSVar1->g_flags1 = pSVar1->g_flags1 | 0x1000000;
        pSVar1->widescreen_translation_x = 0x220;
        mkb::sprintf(pSVar1->text, "game_kiwaku_banana");
    }
    pSVar1 = mkb::create_sprite();
    if (pSVar1 != (mkb::Sprite*) 0x0) {
        pSVar1->unique_id = mkb::SPRITE_BANANA_ICON;
        pSVar1->type = mkb::SPRT_BMP;
        if (mkb::main_game_mode == mkb::CHALLENGE_MODE) {
            (pSVar1->pos).x = 496.0;
        }
        else {
            (pSVar1->pos).x = 525.0;
        }
        (pSVar1->pos).y = 22.0;
        pSVar1->bmp = 0x501;
        pSVar1->depth = 0.09;
        pSVar1->g_counter = 0;
        pSVar1->tick_func = mkb::sprite_banana_icon_tick;
        pSVar1->g_flags1 = pSVar1->g_flags1 | 0x1000000;
        pSVar1->widescreen_translation_x = 0x220;
        mkb::sprintf(pSVar1->text, "game_banana");
        mkb::g_banana_disp_efc_req_count = '\0';
        pSVar1 = mkb::create_linked_sprite(pSVar1);
        if (pSVar1 != (mkb::Sprite*) 0x0) {
            pSVar1->type = mkb::SPRT_BMP;
            (pSVar1->pos).x = -22.0;
            (pSVar1->pos).y = 2.0;
            pSVar1->bmp = 0x501;
            pSVar1->mult_color.red = '\0';
            pSVar1->mult_color.green = '\0';
            pSVar1->mult_color.blue = '\0';
            pSVar1->alpha = 0.5;
            pSVar1->g_flags1 = pSVar1->g_flags1 | 0x1000000;
            pSVar1->widescreen_translation_x = 0x220;
            pSVar1->tick_func = mkb::sprite_banana_icon_shadow_tick;
            mkb::sprintf(pSVar1->text, "game_banana_kage");
        }
    }
    pSVar1 = mkb::create_sprite();
    if (pSVar1 != (mkb::Sprite*) 0x0) {
        if (mkb::main_game_mode == mkb::CHALLENGE_MODE) {
            (pSVar1->pos).x = 521.0;
        }
        else {
            (pSVar1->pos).x = 550.0;
        }
        (pSVar1->pos).y = 25.0;
        pSVar1->font = mkb::FONT_NUM_NML_SCORE;
        pSVar1->depth = 0.09;
        pSVar1->g_flags1 = pSVar1->g_flags1 | 0x1000000;
        pSVar1->widescreen_translation_x = 0x220;
        pSVar1->tick_func = mkb::sprite_banana_count_tick;
    }
    pSVar1 = mkb::create_sprite();
    if (pSVar1 != (mkb::Sprite*) 0x0) {
        if (mkb::main_game_mode == mkb::CHALLENGE_MODE) {
            (pSVar1->pos).x = 523.0;
        }
        else {
            (pSVar1->pos).x = 552.0;
        }
        (pSVar1->pos).y = 27.0;
        pSVar1->font = mkb::FONT_NUM_NML_SCORE;
        pSVar1->mult_color.red = '\0';
        pSVar1->mult_color.green = '\0';
        pSVar1->mult_color.blue = '\0';
        pSVar1->alpha = 0.5;
        pSVar1->g_flags1 = pSVar1->g_flags1 | 0x1000000;
        pSVar1->widescreen_translation_x = 0x220;
        pSVar1->tick_func = mkb::sprite_banana_count_tick;
    }
}

// Expands the max amount of bananas that can be collected, then overwrites
// the amount of digits to display with 4. Then, hooks into the banana counter
// sprite create function calling our sprite create function instead.
void init_main_loop() {
    // In add_bananas, return if the current count after adding is less than 9999 instead of 999
    patch::write_word(reinterpret_cast<void*>(0x802b8284), 0x2c00270f);// cmpwi r0, 9999

    // In add_bananas, cap the max banana count to 9999 instead of 999
    patch::write_word(reinterpret_cast<void*>(0x802b828c), PPC_INSTR_LI(PPC_R0, 9999));

    // Change format string
    mkb::strcpy(mkb::sprite_banana_count_fmt_string, "%04d");

    patch::write_branch(reinterpret_cast<void*>(mkb::create_banana_counter_sprites), reinterpret_cast<void*>(create_new_banana_counter_sprites));
}

}// namespace four_digit_banana_counter
