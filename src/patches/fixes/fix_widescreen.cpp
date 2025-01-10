#include "fix_widescreen.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace fix_widescreen {

TICKABLE_DEFINITION((
        .name = "fix-widescreen",
        .description = "Widescreen fixes",
        .init_main_loop = init_main_loop,
        .tick = tick, ))

// Fixed create sprite functions. Setting the second digit of the sprite's flag
// to 1 makes the sprite scale in widescreen
void create_new_bonus_sprite(u8* status, mkb::Sprite* sprite) {

    mkb::Sprite* pSVar1;

    pSVar1 = mkb::create_sprite();
    if (pSVar1 != (mkb::Sprite*) 0x0) {
        (pSVar1->pos).x = 320.0;
        (pSVar1->pos).y = 240.0;
        pSVar1->depth = 0.05;
        pSVar1->font = mkb::FONT_ASC_72x64;
        pSVar1->alignment = mkb::ALIGN_CENTER;
        pSVar1->g_counter = 0;
        pSVar1->para1 = 1;
        pSVar1->widescreen_translation_x = 0x140;
        pSVar1->g_flags1 = pSVar1->g_flags1 | 0x1000000;
        pSVar1->tick_func = mkb::sprite_bonus_finish_or_perfect_tick;
        mkb::strcpy(pSVar1->text, "PERFECT");
    }
    return;
}
void create_new_bonus_finish_sprite(u8* status, mkb::Sprite* sprite) {

    mkb::Sprite* pSVar1;

    pSVar1 = mkb::create_sprite();
    if (pSVar1 != (mkb::Sprite*) 0x0) {
        (pSVar1->pos).x = 320.0;
        (pSVar1->pos).y = 240.0;
        pSVar1->depth = 0.05;
        pSVar1->font = mkb::FONT_ASC_72x64;
        pSVar1->alignment = mkb::ALIGN_CENTER;
        pSVar1->g_counter = 0;
        pSVar1->para1 = 0;
        pSVar1->mult_color.red = 0xff;
        pSVar1->mult_color.green = 0x80;
        pSVar1->mult_color.blue = 0;
        pSVar1->width = 0.8;
        pSVar1->height = 0.8;
        pSVar1->widescreen_translation_x = 0x140;
        pSVar1->g_flags1 = pSVar1->g_flags1 | 0x1001000;
        pSVar1->tick_func = mkb::sprite_bonus_finish_or_perfect_tick;
        mkb::strcpy(pSVar1->text, "BONUS FINISH");
    }
    return;
}
void create_new_pausemenu_sprite(u8* status, mkb::Sprite* sprite) {

    sprite = mkb::create_sprite();
    if (sprite != (mkb::Sprite*) 0x0) {
        sprite->unique_id = 0x4;
        (sprite->pos).x = 315.0;
        (sprite->pos).y = -128.0;
        sprite->type = mkb::SPRT_BMP;
        sprite->font = mkb::FONT_ASC_24x24;
        sprite->alignment = mkb::ALIGN_CENTER;
        sprite->depth = 0.005;
        sprite->para1 = 1;
        sprite->g_flags1 = 0x1000000;
        sprite->widescreen_translation_x = 0x140;
        sprite->fpara1 = 0.0;
        sprite->alpha = 0.5;
        sprite->mult_color.red = '\0';
        sprite->mult_color.green = '\0';
        sprite->mult_color.blue = '\0';
        sprite->width = 0.0;
        sprite->height = 0.0;
        sprite->g_counter = 1;
        sprite->disp_func = mkb::sprite_pausemenu_disp;
        mkb::strcpy(sprite->text, "pause menu");
    }
    return;
}
// The fallout check allows the sprite to properly align with the GOAL or FALL OUT
// sprite when in widescreen, as their positions are different compared to playing
// in 4:3
void create_new_replay_sprite(u8* status, mkb::Sprite* sprite) {

    sprite = mkb::create_sprite();
    if (sprite != (mkb::Sprite*) 0x0) {
        (sprite->pos).x = 760.0;
        (sprite->pos).y = 460.0;
        sprite->font = mkb::FONT_ASC_16x16;
        sprite->alignment = mkb::ALIGN_UPPER_LEFT;
        sprite->depth = 0.09;
        sprite->width = 1.25;
        sprite->height = 1.25;
        sprite->g_counter = 222;
        sprite->g_flags1 = 0x1000000;
        if (mkb::did_ball_fallout(mkb::current_ball) == true) {
            sprite->widescreen_translation_x = 0x280;
        }
        else {
            sprite->widescreen_translation_x = 0x215;
        }
        sprite->tick_func = mkb::sprite_replay_tick;
        mkb::strcpy(sprite->text, "REPLAY");
    }
    sprite = mkb::get_sprite_with_unique_id(0xe);
    if (sprite != (mkb::Sprite*) 0x0) {
        sprite->para1 = 0xf;
    }
    return;
}
void create_new_1up_sprite(u8* status, mkb::Sprite* sprite) {

    sprite = mkb::create_sprite();
    if (sprite != (mkb::Sprite*) 0x0) {
        sprite->unique_id = 0x10;
        (sprite->pos).x = 320.0;
        (sprite->pos).y = 300.0;
        sprite->font = mkb::FONT_ASC_72x64;
        sprite->alignment = mkb::ALIGN_CENTER;
        sprite->alpha = 0.0;
        sprite->g_counter = 150;
        sprite->g_flags1 = 0x1000000;
        sprite->widescreen_translation_x = 0x140;
        sprite->para1 = 120;
        sprite->tick_func = mkb::sprite_1up_tick;
        mkb::strcpy(sprite->text, "1UP");
    }
    return;
}
// The main_game_mode check prevents the sprite from appearing in Story Mode
// after a one-stage difficulty is played
void create_new_final_stage_sprite(u8* status, mkb::Sprite* sprite) {
    sprite = mkb::create_sprite();
    if (sprite != (mkb::Sprite*) 0x0) {
        sprite->unique_id = 14;
        (sprite->pos).x = 320.0;
        (sprite->pos).y = 300.0;
        sprite->font = mkb::FONT_ASC_72x64;
        sprite->alignment = mkb::ALIGN_CENTER;
        sprite->width = 0.5;
        sprite->height = 0.5;
        sprite->g_counter = 0x96;
        sprite->g_flags1 = sprite->g_flags1 | 0x1001000;
        sprite->widescreen_translation_x = 0x140;
        sprite->tick_func = mkb::sprite_final_stage_tick;
        sprite->disp_func = mkb::sprite_final_stage_disp;
        if (mkb::main_game_mode == mkb::STORY_MODE) {
            mkb::strcpy(sprite->text, "");
        }
        else {
            mkb::strcpy(sprite->text, mkb::LOADIN_TEXT_FINAL_STAGE);
        }
    }
}

// Call our fixed create sprite functions in place of the originals, then
// enlargen the pausemenu's dim area. Then, handle sprite type changes for
// the "JUMP TO STAGE" sprite and continues remaining sprite
void init_main_loop() {
    patch::write_branch(
        reinterpret_cast<void*>(mkb::create_bonus_finish_or_perfect_sprite),
        reinterpret_cast<void*>(create_new_bonus_sprite));
    patch::write_branch(
        reinterpret_cast<void*>(mkb::create_pausemenu_sprite),
        reinterpret_cast<void*>(create_new_pausemenu_sprite));
    patch::write_branch(
        reinterpret_cast<void*>(mkb::create_replay_sprite),
        reinterpret_cast<void*>(create_new_replay_sprite));
    patch::write_branch(
        reinterpret_cast<void*>(mkb::create_1up_sprite),
        reinterpret_cast<void*>(create_new_1up_sprite));
    patch::write_branch(
        reinterpret_cast<void*>(mkb::create_bonus_finish_sprite),
        reinterpret_cast<void*>(create_new_bonus_finish_sprite));
    patch::write_branch(
        reinterpret_cast<void*>(mkb::create_final_stage_sprite),
        reinterpret_cast<void*>(create_new_final_stage_sprite));
    patch::write_word(reinterpret_cast<void*>(0x803e7a28), 0x43b40000);
    patch::write_word(reinterpret_cast<void*>(0x8032e0e4), (0x6400a100));
    patch::write_word(reinterpret_cast<void*>(0x8032e798), (0x6400a100));
    patch::write_word(reinterpret_cast<void*>(0x8032f268), (0x6400a100));
}


// The SEL_NGC check fixes less being visible vertically when playing in widescreen.
// It only activates when we're not in menus as calibration breaks visually otherwise.
// The MD_GAME check fixes the View Stage aspect ratio when in widescreen. The STORY_MODE
// check nulls out the FINAL STAGE text for the load-in to prevent it from displaying
// when one-stage difficulties are present and the rest changes the JUMP TO STAGE and
// continues remaining sprite position pointers and values in widescreen since there's
// not enough space in their create functions to add a widescreen translation field

void tick() {
    if (mkb::sub_mode == mkb::SMD_SEL_NGC_MAIN) {
        patch::write_word(reinterpret_cast<void*>(0x80287cf8), 0x418200a8); // original instruction
    }
    else {
        patch::write_nop(reinterpret_cast<void*>(0x80287cf8)); // nops a branch to the FOV-modifying code
    }
    if (mkb::main_mode == mkb::MD_GAME) {
        if (mkb::widescreen_mode == 0) {
            mkb::view_stage_aspect_ratio = 1.333333333f;
        }
        else {
            mkb::view_stage_aspect_ratio = 1.777777777f;
        }
    }
    if (mkb::widescreen_mode == 0) {
        patch::write_word(reinterpret_cast<void*>(0x8032e048), (0xc01f0038));
        patch::write_word(reinterpret_cast<void*>(0x8032e6b0), (0xc01f0038));
        patch::write_word(reinterpret_cast<void*>(0x803e7cf4), (0x43f50000));
    }
    else {
        patch::write_word(reinterpret_cast<void*>(0x8032e048), (0xc01f00ac));
        patch::write_word(reinterpret_cast<void*>(0x803e7a3c), (0x43d98000));
        patch::write_word(reinterpret_cast<void*>(0x8032e6b0), (0xc01f00ac));
        patch::write_word(reinterpret_cast<void*>(0x803e7cf4), (0x4414c000));
    }
}

}// namespace fix_widescreen