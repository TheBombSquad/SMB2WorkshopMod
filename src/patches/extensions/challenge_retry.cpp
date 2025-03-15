#include "challenge_retry.h"
#include "../internal/pad.h"
#include "death_counter.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace challenge_retry {

TICKABLE_DEFINITION((
        .name = "challenge-mode-retry",
        .description = "Challenge Mode retry",
        .init_main_loop = init))

static patch::Tramp<decltype(&mkb::pause_game)> s_pause_game_tramp;

int original_pausemenu_selection;

void pausemenu_handler() {
    int ball_index;
    mkb::Ball* ball;

    const int total_size = 30;// Adjust size based on gaps and strings
    const int gap_size = 5;   // Gap of nullptrs between each string (game expects these to be localized strings)

    // Array of pointers to strings
    static char* menu_options_1[total_size] = {nullptr};
    static char* menu_options_2[total_size] = {nullptr};

    // Strings to be stored in the array
    char* options_1[] = {
        // Menu strings during gameplay
        "Continue game",
        "Retry",
        "View stage",
        "How to play",
        "Exit game"};
    char* options_2[] = {
        // Menu strings during replay
        "Continue game",
        "Retry",
        "Save replay",
        "How to play",
        "Exit game"};

    // Fill the array with strings and gaps of nullptrs
    int current_index = 0;
    for (int i = 0; i < 5; ++i) {
        menu_options_1[current_index] = options_1[i];
        menu_options_2[current_index] = options_2[i];
        current_index += gap_size + 1;// Move index by 1 (string) + gap size
    }

    if (mkb::main_game_mode == mkb::CHALLENGE_MODE) {
        if (mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN ||
            mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN ||
            mkb::sub_mode == mkb::SMD_GAME_READY_INIT || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN ||
            mkb::mode_info.ball_mode == mkb::BALLMODE_ON_BONUS_STAGE || mkb::num_players != 1 ||
            mkb::balls[0].monkey_count < 2 || (mkb::mode_info.ball_mode & mkb::BALLMODE_GOALED)) {
            mkb::pausemenu_entry_counts[1] = 4;
            mkb::g_current_pause_menu_entry_count = 4;
            mkb::pausemenu_entry_pointers[9] = mkb::challenge_goal_pausemenu_entries;
            mkb::pausemenu_entry_pointers[1] = mkb::challenge_play_pausemenu_entries;
            patch::write_word(reinterpret_cast<void*>(0x80273cc8), 0x2c000004);// in the instruction which handles pausemenu input, Exit Game when option 4 is selected
        }
        else if ((mkb::sub_mode == mkb::SMD_GAME_READY_MAIN || mkb::sub_mode == mkb::SMD_GAME_READY_INIT) && mkb::mode_info.attempt_count > 1) {
            mkb::pausemenu_entry_counts[1] = 4;
            mkb::g_current_pause_menu_entry_count = 4;
            mkb::pausemenu_entry_pointers[1] = mkb::challenge_play_pausemenu_entries;
            patch::write_word(reinterpret_cast<void*>(0x80273cc8), 0x2c000004);// in the instruction which handles pausemenu input, Exit Game when option 4 is selected
        }
        else if (mkb::sub_mode == mkb::SMD_GAME_ROLL_INIT || mkb::sub_mode == mkb::SMD_GAME_ROLL_MAIN) {
            // The credits minigame uses the same main game mode as Challenge Mode, so we need a special case to set its respective menu
            mkb::pausemenu_type = 7;
        }
        else {
            // Okay, things get messy here...
            // The vanilla pausemenu function is messy in itself, so the idea is to fakematch each visual selection with its internal selection
            // The bitflag check on these has something to do with if we're in a subscreen of the pausemenu

            // View stage
            if (mkb::g_current_focused_pause_menu_entry == 2 && pad::button_pressed(mkb::PAD_BUTTON_A) && mkb::g_some_status_bitflag_maybe_pause_related != 00000000) {
                mkb::g_current_focused_pause_menu_entry = 1;
                original_pausemenu_selection = 2;
            }
            // How to play
            else if (mkb::g_current_focused_pause_menu_entry == 3 && pad::button_pressed(mkb::PAD_BUTTON_A) && mkb::g_some_status_bitflag_maybe_pause_related != 00000000) {
                mkb::g_current_focused_pause_menu_entry = 2;
                original_pausemenu_selection = 3;
            }
            // Retry
            else if (mkb::g_current_focused_pause_menu_entry == 1 && pad::button_pressed(mkb::PAD_BUTTON_A) && mkb::g_some_status_bitflag_maybe_pause_related != 00000000) {
                mkb::g_current_focused_pause_menu_entry = 5;
                // If death count is enabled, increment the death counter
                // Otherwise, decrement the life counter
                if (tickable::get_tickable_manager().get_tickable_status("challenge-mode-death-count")) {
                    death_counter::update_death_count();
                }
                else {
                    mkb::Ball* ball = mkb::balls;
                    for (int idx = 0; idx < 4; idx++) {
                        if (ball->status == mkb::STAT_NORMAL) {
                            ball->monkey_count--;
                        }
                        ball++;
                    }
                }
                mkb::mode_info.attempt_count = mkb::mode_info.attempt_count + 1;
                ball = mkb::balls;
                ball_index = mkb::curr_player_idx;
                for (ball_index = 0; ball_index < 8; ball_index = ball_index + 1) {
                    if (ball->status == mkb::STAT_NORMAL) {
                        ball->phys_flags = ball->phys_flags;
                        ball->g_effect_flags = ball->g_effect_flags | 0x80;
                    }
                    ball = ball + 1;
                }
                mkb::sub_mode_request = mkb::SMD_GAME_READY_INIT;
                mkb::g_fade_track_volume(100, '\n');
            }
            // Return our original visual selection once we exit a subscreen of the pausemenu
            if (mkb::g_some_pausemenu_var != 0xffffffff) {
                mkb::g_current_focused_pause_menu_entry = original_pausemenu_selection;
            }
            // Repoint menu entries to our expanded ones
            mkb::pausemenu_entry_pointers[1] = menu_options_1;
            mkb::pausemenu_entry_pointers[9] = menu_options_2;
            patch::write_word(reinterpret_cast<void*>(0x80273cc8), 0x2c000005);// in the instruction which handles pausemenu input, Exit Game when option 5 is selected
            // Expand entry count
            mkb::pausemenu_entry_counts[1] = 5;
            mkb::g_current_pause_menu_entry_count = 5;
            // Set the pausemenu type to 'normal' Challenge Mode's
            mkb::pausemenu_type = 1;
        }
    }
}

void init() {
    patch::hook_function(s_pause_game_tramp, mkb::pause_game, []() {
        s_pause_game_tramp.dest();
        pausemenu_handler();
    });
}

}// namespace challenge_retry