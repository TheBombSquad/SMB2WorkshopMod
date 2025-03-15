#include "story_char_select.h"

#include "internal/assembly.h"
#include "internal/pad.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace story_char_select {

TICKABLE_DEFINITION((
        .name = "story-mode-char-select",
        .description = "Story mode character select",
        .init_main_loop = init_main_loop,
        .init_main_game = init_main_game,
        .init_sel_ngc = init_sel_ngc, ))

static mkb::undefined4* AIAI[] = {&mkb::CHAR_A, &mkb::CHAR_I, &mkb::CHAR_A, &mkb::CHAR_I, &mkb::CHAR_SPACE,
                                  &mkb::CHAR_SPACE};
static mkb::undefined4* MEEMEE[] = {&mkb::CHAR_M, &mkb::CHAR_E, &mkb::CHAR_E, &mkb::CHAR_M, &mkb::CHAR_E,
                                    &mkb::CHAR_E};
static mkb::undefined4* BABY[] = {&mkb::CHAR_B, &mkb::CHAR_A, &mkb::CHAR_B, &mkb::CHAR_Y, &mkb::CHAR_SPACE,
                                  &mkb::CHAR_SPACE};
static mkb::undefined4* GONGON[] = {&mkb::CHAR_G, &mkb::CHAR_O, &mkb::CHAR_N, &mkb::CHAR_G, &mkb::CHAR_O,
                                    &mkb::CHAR_N};
static mkb::undefined4* HIHI[] = {&mkb::CHAR_H, &mkb::CHAR_I, &mkb::CHAR_H, &mkb::CHAR_I, &mkb::CHAR_SPACE,
                                  &mkb::CHAR_SPACE};
static mkb::undefined4** monkey_name_lookup[] = {AIAI, MEEMEE, BABY, GONGON, HIHI};

static bool is_entering_story = false;

// Overrides the return value of certain functions to force the chosen monkey to be
// preloaded in place of AiAi
void init_main_loop() {
    patch::write_branch_bl(reinterpret_cast<void*>(0x803daffc),
                           reinterpret_cast<void*>(main::get_monkey_id_hook));
}

// Sets the storymode filename to the name of the selected monkey, when no name is provided.
void set_nameentry_filename() {
    for (int i = 0; i < 6; i++) {
        mkb::story_file_name[i] = reinterpret_cast<char*>(monkey_name_lookup[mkb::active_monkey_id[0]][i]);
    }
    mkb::g_some_nameentry_length = 0x6;
}

// Overrides the return value of certain functions to force the chosen monkey to be
// preloaded in place of AiAi.
// Also calls the function to set the default filename to the name of the selected
// monkey, rather than deafulting to 'AIAI'.
void init_main_game() {
    patch::write_branch_bl(reinterpret_cast<void*>(0x808fcac4),
                           reinterpret_cast<void*>(main::get_monkey_id_hook));
    patch::write_branch_bl(reinterpret_cast<void*>(0x808ff120),
                           reinterpret_cast<void*>(main::get_monkey_id_hook));
    patch::write_branch_bl(reinterpret_cast<void*>(0x80908894),
                           reinterpret_cast<void*>(main::get_monkey_id_hook));

    patch::write_branch_bl(reinterpret_cast<void*>(0x80906368),
                           reinterpret_cast<void*>(set_nameentry_filename));
    patch::write_nop(reinterpret_cast<void*>(0x8090636c));
    patch::write_nop(reinterpret_cast<void*>(0x80906370));
    patch::write_nop(reinterpret_cast<void*>(0x80906374));
    patch::write_nop(reinterpret_cast<void*>(0x80906378));

    // Lets the sel_ngc portion of the patch know we aren't entering story anymore
    // Also sets menu_stack_ptr to 1, ensuring we return to the Mode Select screen
    if (is_entering_story) {
        is_entering_story = false;
        mkb::sel_menu_info.menu_stack_ptr = 1;
    }
}

// Assign the correct 'next screen' variables to redirect Story Mode to the
// character select screen. Also handle input to prevent Story Mode from not
// initializing if mode_cnt isn't set to 1.
void init_sel_ngc() {
    // Patches the 'next screen ID' for the 'STORY MODE' button in the Mode Select screen
    mkb::menu_main_game_select_entries[0].next_screen_id = mkb::MENUSCREEN_CHARACTER_SELECT_2;
    for (int i = 0; i < 4; i++) {
        // Patches each of the character button's 'next screen ID' in the character select screen
        mkb::menu_character_select_2_entries[i].next_screen_id = mkb::MENUSCREEN_STORY_MODE_SELECTED;
    }
    static patch::Tramp<decltype(&mkb::menu_character_select_tick)> s_character_select_tick;
    patch::hook_function(
        s_character_select_tick,
        mkb::menu_character_select_tick, []() {
            if (mkb::g_currently_visible_menu_screen == mkb::MENUSCREEN_CHARACTER_SELECT_2) {
                // If we press A, then we are going to start the process of entering story mode
                // We need to update our current position on the menu stack with the character select menu
                constexpr size_t ORIGINAL_STACK_INDEX = 2;

                if (pad::button_pressed(mkb::PAD_BUTTON_A)) {
                    mkb::sel_menu_info.menu_stack[ORIGINAL_STACK_INDEX] = mkb::MENUSCREEN_CHARACTER_SELECT_2;
                    is_entering_story = true;
                }
                // If we press B, then we are cancelling the process of entering story mode
                // We need to clean up since the stack pointer is changed after calling the tick function
                // So, we restore the pointer to the original '2'
                if (pad::button_pressed(mkb::PAD_BUTTON_B)) {
                    if (is_entering_story) {
                        mkb::sel_menu_info.menu_stack_ptr = ORIGINAL_STACK_INDEX;
                        mkb::g_next_menu_screen = mkb::MENUSCREEN_MAIN_GAME_SELECT;
                    }
                    is_entering_story = false;
                }
            }

            s_character_select_tick.dest();

            if (mkb::g_currently_visible_menu_screen == mkb::MENUSCREEN_CHARACTER_SELECT_2) {
                // We have to do this hacky nonsense to get the game to think we are entering story mode
                // Otherwise, it will display (or rather, attempt to display) the Practice Mode stage select screen
                if (is_entering_story) mkb::sel_menu_info.menu_stack_ptr = 1;
            }
        });
}

}// namespace story_char_select