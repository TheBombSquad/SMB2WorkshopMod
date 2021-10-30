#include "scratch.h"
#include "mkb.h"
#include "patch.h"
#include "pad.h"

namespace scratch
{

int test;
int frame_counter = 0;
int lerp_duration;
int lerp_value;
bool has_init;

void init() {
    patch::hook_function(mkb::sprite_score_tick, sprite_tick);
    test = 0;
    has_init = false;
}

void sprite_tick(u8* status, mkb::Sprite* sprite) {
    float score = (float)mkb::balls[mkb::curr_player_idx].score;
    float old_score = sprite->g_lerp_value;
    float delta = score-old_score;
    float displayed;
    int final_score;

    if (mkb::sub_mode != mkb::SMD_GAME_GOAL_REPLAY_MAIN) {
        lerp_duration = 30;
    }
    else {
        lerp_duration = 180;
    }

    if (score > old_score) {
        if (frame_counter < lerp_duration) {
            displayed = old_score + frame_counter*(delta/lerp_duration);
            frame_counter++;
            if (mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN && (frame_counter % 4 == 0)) {
                mkb::call_SoundReqID_arg_2(46);
            }
        }
        else {
            sprite->g_lerp_value = score;
            frame_counter = 0;
        }
    }
    else {
        displayed = score;
    }

    final_score = mkb::__cvt_fp2unsigned((double)displayed);
    mkb::sprintf(sprite->text, "%d", final_score);
    return;
}

void best_score_tick(u8* status, mkb::Sprite* sprite) {
    float score = (float)mkb::balls[mkb::curr_player_idx].score;
    float old_score = sprite->g_lerp_value;
    float delta = score-old_score;
    float displayed;
    int final_score;

    if (mkb::sub_mode != mkb::SMD_GAME_GOAL_REPLAY_MAIN) {
        lerp_duration = 30;
    }
    else {
        lerp_duration = 180;
    }

    if (score > old_score) {
        if (frame_counter < lerp_duration) {
            displayed = old_score + frame_counter*(delta/lerp_duration);
            frame_counter++;
            if (mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN && (frame_counter % 4 == 0)) {
                mkb::call_SoundReqID_arg_2(46);
            }
        }
        else {
            sprite->g_lerp_value = score;
            frame_counter = 0;
        }
    }
    else {
        displayed = score;
    }

    final_score = mkb::__cvt_fp2unsigned((double)displayed);
    mkb::sprintf(sprite->text, "%d", final_score);
    return;
}

void best_score_disp(mkb::Sprite* sprite) {
  float fVar1;
  float fVar2;
  u32 uVar3;
  int iVar4;
  double dVar5;
  double dVar6;
  double uVar7;

  iVar4 = 0;
  mkb::g_reset_font_drawing_settings();
  mkb::g_set_font_type((u32)sprite->g_font2);
  mkb::g_set_something_else_with_font_drawing(1.0,0.0);
  mkb::g_set_smth_with_font_drawing9(0.5);
  mkb::g_mask_smth_with_font_drawing(0);
  uVar3 = 0;
  mkb::g_set_smth_with_font_drawing_depth(sprite->g_depth + 0.01);
  fVar1 = (sprite->prev_sprite->pos).x + (float(iVar4 + 8)) + 2.0;
  uVar7 = 2.0;

  fVar2 = (sprite->prev_sprite->pos).y + (sprite->pos).y + 2.0;
  dVar5 = static_cast<double>(fVar1);
  dVar6 = (double)fVar2;

  mkb::g_set_smth_with_font_drawing1(fVar1,fVar2);
  mkb::g_printf_draw_with_font(dVar5,dVar6,uVar7,0.0, 0.0,0.0, 0.0, 0.0,sprite->text,uVar3,0,0,0,0,0,0);
  mkb::g_set_smth_with_font_drawing9(1.0);
  mkb::g_mask_smth_with_font_drawing(0xffffff);
  uVar3 = 0;

  mkb::g_set_smth_with_font_drawing_depth(sprite->g_depth);
  fVar1 = (sprite->prev_sprite->pos).x + (float)(iVar4 + 8);
  fVar2 = (sprite->prev_sprite->pos).y + (sprite->pos).y;
  dVar5 = (double)fVar1;
  dVar6 = (double)fVar2;
  mkb::g_set_smth_with_font_drawing1(fVar1,fVar2);
  mkb::g_printf_draw_with_font(dVar5,dVar6,uVar7,0.0,0.0,0.0,0.0,0.0,sprite->text,uVar3,0,0,0,0,0,0);
  mkb::g_reset_font_drawing_settings();
  return;
}

void create_best_score() {
  mkb::Sprite *sprite = mkb::create_sprite();
  if (sprite != (mkb::Sprite *)0x0) {
    sprite->g_visible = '\x01';
    (sprite->pos).x = 16.0f;
    (sprite->pos).y = 48.0f;
    sprite->g_texture_id = 0x504;
    sprite->g_flags = sprite->g_flags | 0x1000000;
    sprite->field_0x12 = 0x60;
    mkb::sprintf(sprite->text,"game_best_score");
    sprite = mkb::create_linked_sprite(sprite);
    if (sprite != (mkb::Sprite *)0x0) {
      (sprite->pos).y = 1.0;
      sprite->g_font2 = mkb::FONT_NUM_NML_SCORE;
      sprite->g_flags = sprite->g_flags | 0x1000000;
      sprite->field_0x12 = 0x60;
      sprite->tick_func = best_score_tick;
      sprite->disp_func = best_score_disp;
    }
  }
}
void tick() {
    if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT && !has_init) {
        create_best_score();
    }

    if (pad::button_pressed(mkb::PAD_BUTTON_Y)) {
        mkb::call_SoundReqID_arg_0(test);
        mkb::OSReport("played test: %d\n", test);
        test++;
    }
}

void disp() {}

}
