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
        .tick = tick, ))

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
}

// Assign the correct 'next screen' variables to redirect Story Mode to the
// character select screen. Also handle input to prevent Story Mode from not
// initializing if mode_cnt isn't set to 1.
void tick() {
    if (mkb::sub_mode == mkb::SMD_SEL_NGC_MAIN) {
        patch::write_word(reinterpret_cast<void*>(0x80921a20), 0x6000000);
        patch::write_word(reinterpret_cast<void*>(0x80920ba0), 0xC000000);
        if (mkb::g_currently_visible_menu_screen == 0x6) {
            if (pad::button_pressed(mkb::PAD_BUTTON_A)) {
                mkb::sel_menu_info.menu_stack_ptr = 1;
                mkb::sel_menu_info.menu_stack[1] = 7;
            }
            else if (pad::button_pressed(mkb::PAD_BUTTON_B) && mkb::g_character_selected != 1) {
                mkb::sel_menu_info.menu_stack_ptr = 2;
                mkb::sel_menu_info.menu_stack[1] = 7;
            }
        }
    }
    if (mkb::sub_mode == mkb::SMD_GAME_SCENARIO_MAIN) {
        mkb::sel_menu_info.menu_stack_ptr = 1;
        mkb::sel_menu_info.menu_stack[1] = 7;
    }
}

}// namespace story_char_select
