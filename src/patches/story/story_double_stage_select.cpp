#include "story_double_stage_select.h"
#include "../internal/pad.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace story_double_stage_select {

TICKABLE_DEFINITION((
        .name = "double-stage-select",
        .description = "Story mode double stage select",
        .init_main_loop = init,
        .tick = tick))

static patch::Tramp<decltype(&mkb::check_pause_menu_input)> s_check_pause_menu_input_tramp;

// While we can easily edit a few values in the function which handles pausemenu selections to
// get most of the desired behavior, some of it has to be recreated manually

void pausemenu_handler(mkb::Sprite* pause_sprite) {
    if (mkb::main_game_mode == mkb::STORY_MODE) {
        // Stage select
        if (mkb::g_current_focused_pause_menu_entry == 5 && pad::button_pressed(mkb::PAD_BUTTON_A) && mkb::g_some_status_bitflag_maybe_pause_related != 00000000) {
            mkb::g_related_to_pause_menu_input = 0x3c;
            if (pause_sprite != (mkb::Sprite*) 0x0) {
                pause_sprite->para1 = 2;
            }
            // Fade the music out only if the music fix isn't enabled
            if (!tickable::get_tickable_manager().get_tickable_status("story-mode-music-fix")) {
                mkb::g_fade_track_volume(0x3b, '\x02');
            }
            mkb::g_screenfade_color = 0xffffff;
        }
        // Exit game
        else if (mkb::g_current_focused_pause_menu_entry == 3 && pad::button_pressed(mkb::PAD_BUTTON_A) && mkb::g_some_status_bitflag_maybe_pause_related != 00000000) {
            mkb::g_related_to_pause_menu_input = 0x3c;
            if (pause_sprite != (mkb::Sprite*) 0x0) {
                pause_sprite->para1 = 3;
            }
            mkb::g_fade_track_volume(0x3c, '\x02');
            mkb::g_load_stage_for_menu_bg(-1, 1);
        }
    }
}

void init() {
    // Edits behaviors in the function which handles pausemenu selections
    patch::write_nop(reinterpret_cast<void*>(0x802745a4));
    patch::write_nop(reinterpret_cast<void*>(0x8027457c));
    patch::write_word(reinterpret_cast<void*>(0x8027459c), 0x2c000003);// cmpwi r0, 3
    // Hook into the function which checks input in the pausemenu for our recreated behaviors
    patch::hook_function(s_check_pause_menu_input_tramp, mkb::check_pause_menu_input, [](mkb::Sprite* pause_sprite) {
        s_check_pause_menu_input_tramp.dest(pause_sprite);
        pausemenu_handler(pause_sprite);
    });
    // Text changes
    mkb::story_play_pausemenu_entries[18] = mkb::PAUSEMENU_EXIT_GAME_STRING;
    mkb::story_goal_pausemenu_entries[18] = mkb::PAUSEMENU_EXIT_GAME_STRING;
    mkb::story_play_pausemenu_entries[30] = mkb::PAUSEMENU_STAGE_SELECT_STRING;
    mkb::story_goal_pausemenu_entries[30] = mkb::PAUSEMENU_STAGE_SELECT_STRING;
}

void tick() {
    // Rids and rearranges some vanilla behaviors for pausemenu selections when in Story Mode
    if (mkb::main_game_mode == mkb::STORY_MODE && mkb::sub_mode != mkb::SMD_GAME_SCENARIO_MAIN) {
        patch::write_word(reinterpret_cast<void*>(0x8027513c), 0x3803fffd);
        patch::write_nop(reinterpret_cast<void*>(0x80273ad0));
    }
    else {
        // Original instructions
        patch::write_word(reinterpret_cast<void*>(0x8027513c), 0x3803ffff);
        patch::write_word(reinterpret_cast<void*>(0x80273ad0), 0x3803ffff);
    }
}
}// namespace story_double_stage_select