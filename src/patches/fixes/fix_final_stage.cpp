#include "fix_final_stage.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace fix_final_stage {

TICKABLE_DEFINITION((
        .name = "fix-final-stage",
        .description = "Final stage fixes",
        .init_main_loop = init_main_loop,
        .init_main_game = init_main_game,
        .tick = tick))


void tick() {
    // Fix FINAL STAGE sprite appearing if Story Mode is played following a one-stage difficulty
    if (mkb::main_game_mode == mkb::STORY_MODE) {
        mkb::strcpy(mkb::LOADIN_TEXT_FINAL_STAGE, "");
        mkb::strcpy(mkb::SPRITE_TEXT_FINAL_STAGE, "");
    }
    else {
        mkb::strcpy(mkb::LOADIN_TEXT_FINAL_STAGE, "FINAL STAGE");
        mkb::strcpy(mkb::SPRITE_TEXT_FINAL_STAGE, "FINAL STAGE");
    }
    // Stuff for if a FINAL and BONUS stage overlap
    if (((mkb::mode_info.ball_mode & mkb::BALLMODE_ON_BONUS_STAGE | mkb::BALLMODE_ON_FINAL_STAGE) == (mkb::BALLMODE_ON_BONUS_STAGE | mkb::BALLMODE_ON_FINAL_STAGE))) {
        // Fade the FINAL STAGE sprite ourselves when a replay starts
        if (mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT) {
            mkb::Sprite* sprite;
            sprite = mkb::get_sprite_with_unique_id(0xf);
            if (sprite != (mkb::Sprite*) 0x0) {
                sprite->para1 = 0xf;
            }
        }
        // Initiate PERFECT if the perfect bonus patch is enabled
        if (tickable::get_tickable_manager().get_tickable_status("perfect-bonus-completion")) {
            if (mkb::mode_info.bananas_remaining == 0) {
                if ((mkb::mode_info.ball_mode & mkb::BALLMODE_ON_FINAL_STAGE) == (mkb::BALLMODE_ON_FINAL_STAGE)) {
                    if (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN) {
                        mkb::mode_info.ball_mode |= mkb::BALLMODE_CLEARED_BONUS_PERFECT;
                    }
                    else {
                        mkb::mode_info.ball_mode &= ~mkb::BALLMODE_CLEARED_BONUS_PERFECT;
                    }
                }
            }
        }
    }
}
void fix_final_stage_sprite_position() {
    if ((mkb::mode_info.ball_mode & mkb::BALLMODE_ON_BONUS_STAGE | mkb::BALLMODE_ON_FINAL_STAGE) == (mkb::BALLMODE_ON_BONUS_STAGE | mkb::BALLMODE_ON_FINAL_STAGE)) {
        // Repoint sprite positions if we're on a FINAL and BONUS stage
        // Any values written here overwrite unused portions in memory
        patch::write_word(reinterpret_cast<void*>(0x8032bf10), 0xc01f012c);
        patch::write_word(reinterpret_cast<void*>(0x803e7abc), 0x43a50000);
        patch::write_word(reinterpret_cast<void*>(0x8032d1e4), 0xc01f0244);
        patch::write_word(reinterpret_cast<void*>(0x803e7bd4), 0x43eb0000);
        patch::write_word(reinterpret_cast<void*>(0x8032d268), 0xc87f00b8);
        patch::write_word(reinterpret_cast<void*>(0x803e7a48), 0x4073e000);
        patch::write_word(reinterpret_cast<void*>(0x803e7a4c), 0x00000000);
        // By default, the function to fade the FINAL or BONUS stage sprite
        // only fades one at a time, so overwrite the special ID of the FINAL
        // stage one to something else so we can fade it ourselves
        // This special ID is used by a different sprite at other times, so we have to
        // reset it later
        patch::write_word(reinterpret_cast<void*>(0x8032bf00), 0x3800000f);
    }
    else {
        // Undo repoints
        patch::write_word(reinterpret_cast<void*>(0x8032bf10), 0xc01f00f0);
        patch::write_word(reinterpret_cast<void*>(0x8032d1e4), 0xc01f023c);
        patch::write_word(reinterpret_cast<void*>(0x8032d268), 0xc87f0248);
        // Reset special ID
        patch::write_word(reinterpret_cast<void*>(0x8032bf00), 0x3800000e);
    }
}
void handle_final_bonus() {
    // If the final stage is a bonus and we get a Bonus Finish or Time Over, do end of difficulty or extra stuff
    if (((mkb::mode_info.ball_mode & mkb::BALLMODE_ON_BONUS_STAGE | mkb::BALLMODE_ON_FINAL_STAGE) == (mkb::BALLMODE_ON_BONUS_STAGE | mkb::BALLMODE_ON_FINAL_STAGE)) && (mkb::in_practice_mode == false)) {
        bool start_extras = false;
        if (((mkb::mode_flags & mkb::MF_0x1) != mkb::MF_NONE) && (mkb::mode_info.field13_0x2a == 0)) {
            start_extras = true;
        }
        if (((start_extras) && ((mkb::mode_flags & mkb::MF_PLAYING_EXTRA_COURSE) != mkb::MF_NONE)) &&
            (mkb::curr_difficulty != mkb::DIFF_EXPERT)) {
            start_extras = false;
        }
        if (((start_extras) && ((mkb::mode_flags & mkb::MF_PLAYING_MASTER_NOEX_COURSE) != mkb::MF_NONE)) &&
            (mkb::curr_difficulty != mkb::DIFF_EXPERT)) {
            start_extras = false;
        }
        if ((start_extras) && ((mkb::mode_flags & mkb::MF_PLAYING_MASTER_EX_COURSE) != mkb::MF_NONE)) {
            start_extras = false;
        }
        if (mkb::mode_info.ball_mode & mkb::BALLMODE_ON_FINAL_STAGE) {
            if (mkb::mode_info.ball_mode & mkb::BALLMODE_OUT_OF_TIME_RINGOUT | mkb::BALLMODE_FALLEN_OUT | mkb::BALLMODE_CLEARED_BONUS_PERFECT) {
                if (!start_extras || tickable::get_tickable_manager().get_tickable_status("remove-extras")) {
                    if (mkb::sub_mode_frame_counter == 0x3c) {
                        mkb::fade_screen_to_color(0x101, 0, 0x3d);
                        mkb::g_fade_track_volume(0x3c, '\x02');
                    }
                    if (mkb::sub_mode_frame_counter < 1) {
                        mkb::g_smth_with_ending_course();
                        mkb::g_smth_with_ending_course_2();
                        mkb::g_change_music_volume(0xc, 0x3c, '\0');
                        mkb::main_mode_request = mkb::MD_AUTHOR;
                        mkb::sub_mode_request = mkb::SMD_AUTHOR_PLAY_ENDING_INIT;
                        mkb::mode_flags = mkb::mode_flags | 0x100040;
                    }
                }
                else {
                    if (mkb::sub_mode_frame_counter == 0x3c) {
                        mkb::fade_screen_to_color(0x101, 0, 0x3d);
                        mkb::g_fade_track_volume(0x3c, '\x02');
                    }
                    if (mkb::sub_mode_frame_counter < 1) {
                        mkb::sub_mode_request = mkb::SMD_GAME_EXTRA_INIT;
                    }
                }
            }
        }
    }
}

static patch::Tramp<decltype(&mkb::smd_game_ringout_tick)> s_smd_game_ringout_tick_tramp;
static patch::Tramp<decltype(&mkb::smd_game_timeover_tick)> s_smd_game_timeover_tick_tramp;
static patch::Tramp<decltype(&mkb::smd_game_bonus_clear_tick)> s_smd_game_bonus_clear_tick_tramp;
static patch::Tramp<decltype(&mkb::create_stage_loadin_text_sprites)> s_create_stage_loadin_text_sprites_tramp;

void init_main_loop() {
    patch::hook_function(s_create_stage_loadin_text_sprites_tramp, mkb::create_stage_loadin_text_sprites, []() {
        fix_final_stage_sprite_position();
        s_create_stage_loadin_text_sprites_tramp.dest();
    });
}

void init_main_game() {
    patch::hook_function(s_smd_game_ringout_tick_tramp, mkb::smd_game_ringout_tick, []() {
        s_smd_game_ringout_tick_tramp.dest();
        handle_final_bonus();
    });
    patch::hook_function(s_smd_game_timeover_tick_tramp, mkb::smd_game_timeover_tick, []() {
        s_smd_game_timeover_tick_tramp.dest();
        handle_final_bonus();
    });
    patch::hook_function(s_smd_game_bonus_clear_tick_tramp, mkb::smd_game_bonus_clear_tick, []() {
        s_smd_game_bonus_clear_tick_tramp.dest();
        handle_final_bonus();
    });
}

}// namespace fix_final_stage
