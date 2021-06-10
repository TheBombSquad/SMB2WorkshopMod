#include "patch.h"
#include "assembly.h"
#include "heap.h"
#include "savestate.h"
#include "tetris.h"
#include "draw.h"
#include "timer.h"
#include "iw.h"
#include "pad.h"
#include "menu_impl.h"
#include "jump.h"
#include "scratch.h"
#include "relpatches.h"
#include "ppcutil.h"
#include <mkb/mkb.h>
#include <gc/dvd.h>

#include <cstring>
#include <cstdlib>
#include <vector>

#define STREQ(x,y) (strcmp(x,y)==0)
#define KEY_ENABLED(x) (STREQ(key, x) && STREQ(value, "enabled"))

namespace main
{

static void (*s_draw_debug_text_trampoline)();
static void (*s_process_inputs_trampoline)();
static void (*load_additional_rel_trampoline)(const char *rel_filepath, void *rel_buffer_ptrs);

static int config_file_length;
static gc::DVDFileInfo config_file_info;
static char* config_file_buf;
static char config_file_path[] = "/config.txt";

// For dynamically choosing to run init/tick/disp functions based on a config file
static std::vector<void(*)()> main_loop_init_funcs;
static std::vector<void(*)()> main_game_init_funcs;
static std::vector<void(*)()> sel_ngc_init_funcs;
static std::vector<void(*)()> disp_funcs;
static std::vector<void(*)()> tick_funcs;

static bool tetris_enabled = false;
bool debug_mode_enabled = false;

static void perform_assembly_patches()
{
    constexpr u32 offset = 0x600;
    // Inject the run function at the start of the main game loop
    patch::write_branch_bl(reinterpret_cast<void *>(reinterpret_cast<u32>(
                                                        heap::heap_data.main_loop_rel_location) + offset),
                           reinterpret_cast<void *>(start_main_loop_assembly));

    /* Remove OSReport call ``PERF : event is still open for CPU!``
    since it reports every frame, and thus clutters the console */
    // Only needs to be applied to the US version
    patch::write_nop(reinterpret_cast<void *>(0x80033E9C));

    // Nop the conditional that guards `draw_debugtext`, enabling it even when debug mode is disabled
    patch::write_nop(reinterpret_cast<void *>(0x80299f54));

    /* Titlescreen patches
    */
}

static void unlock_everything()
{
    // Don't yet know how to unlock the staff credits game from a fresh save...
    mkb::unlock_info.master_unlocked = true;
    mkb::unlock_info.monkeys = 99;
    mkb::unlock_info.staff_credits_game_unlocked = true;
    mkb::unlock_info.play_points = 99999;
    mkb::unlock_info.newest_play_point_record = 99999;
    mkb::unlock_info.movies = 0x0fff;
    mkb::unlock_info.party_games = 0x0001b600;
    mkb::unlock_info.movies_watched = 0x0fff;
    memset(mkb::cm_unlock_entries, 0xff, sizeof(mkb::cm_unlock_entries));
    memset(mkb::storymode_unlock_entries, 0xff, sizeof(mkb::storymode_unlock_entries));
}

u16* parse_stageid_list(char* buf, u16* array) {
    buf = strchr(buf, '\n')+1;

    char *end_of_section;
    char key[64] = {0};
    char value[64] = {0};
    end_of_section = strchr(buf, '}');
    do {
        char *key_start, *key_end, *end_of_line;
        key_start = strchr(buf, 'E')+2;
        key_end = strchr(buf, ':');
        end_of_line = strchr(buf, '\n');
        strncpy(key, key_start, (key_end-key_start));
        strncpy(value, key_end+2, (end_of_line-key_end)-2);
        int key_idx = atoi(key);
        u16 value_short = (u16)atoi(value);
        array[key_idx] = value_short;

        buf = end_of_line+1;
        memset(key, '\0', 64);
        memset(value, '\0', 64);
    }
    while (buf < end_of_section);
    return array;
}

void parse_function_toggles(char* buf) {
    // Enters from section parsing, so skip to the next line
    buf = strchr(buf, '\n')+1;

    char *end_of_section;
    char key[64] = {0};
    char value[64] = {0};
    end_of_section = strchr(buf, '}');
    do {
        char *key_start, *key_end, *end_of_line;
        key_start = strchr(buf, '\t')+1;
        key_end = strchr(buf, ':');
        end_of_line = strchr(buf, '\n');
        strncpy(key, key_start, (key_end-key_start));
        strncpy(value, key_end+2, (end_of_line-key_end)-2);

        if KEY_ENABLED("perfect-bonus-completion") {
            tick_funcs.push_back(&relpatches::perfect_bonus::tick);
            gc::OSReport("[mod]  Perfect bonus completion enabled!\n");
        }
        else if KEY_ENABLED("remove-desert-haze") {
            relpatches::remove_desert_haze::init();
            gc::OSReport("[mod]  Desert haze removal enabled!\n");
        }
        else if KEY_ENABLED("story-mode-music-fix") {
            relpatches::story_continuous_music::init();
            gc::OSReport("[mod]  Continuous story mode music enabled!\n");
        }
        else if KEY_ENABLED("no-music-vol-decrease-on-pause") {
            relpatches::no_music_vol_decrease_on_pause::init();
            gc::OSReport("[mod] No music volume decrease on pause enabled!\n");
        }
        else if KEY_ENABLED("story-mode-char-select") {
            relpatches::story_mode_char_select::init_main_loop();
            main_game_init_funcs.push_back(&relpatches::story_mode_char_select::init_main_game);
            tick_funcs.push_back(&relpatches::story_mode_char_select::tick);
            gc::OSReport("[mod]  Story mode character select enabled!\n");
        }
        else if KEY_ENABLED("no-hurry-up-music") {
            main_game_init_funcs.push_back(relpatches::no_hurry_up_music::init_main_game);
            tick_funcs.push_back(&relpatches::no_hurry_up_music::tick);
            gc::OSReport("[mod]  Hurry up music removal enabled!\n");
        }
        else if KEY_ENABLED("fix-revolution-slot") {
            relpatches::fix_revolution_slot::init();
            gc::OSReport("[mod]  Revolution stage slot fix enabled!\n");
        }
        else if KEY_ENABLED("fix-labyrinth-camera") {
            relpatches::fix_labyrinth_camera::init();
            gc::OSReport("[mod]  Labyrinth stage slot fix enabled!\n");
        }
        else if KEY_ENABLED("fix-wormhole-surfaces") {
            relpatches::fix_wormhole_surfaces::init();
            gc::OSReport("[mod]  Party game stage slot fix enabled!\n");
        }
        else if KEY_ENABLED("challenge-mode-death-count") {
            main_game_init_funcs.push_back(relpatches::challenge_death_count::init_main_game);
            gc::OSReport("[mod]  Challenge mode death count enabled!\n");
        }
        else if KEY_ENABLED("disable-how-to-play-screen") {
            relpatches::disable_tutorial::init();
            gc::OSReport("[mod]  Tutorial sequence disabled!\n");
        }
        else if KEY_ENABLED("fix-stage-object-reflection") {
            relpatches::fix_stobj_reflection::init_main_loop();
            main_game_init_funcs.push_back(relpatches::fix_stobj_reflection::init_main_game);
            gc::OSReport("[mod]  Stobj reflection flag support enabled!\n");
        }
        else if KEY_ENABLED("enhance-reflective-surfaces") {
            relpatches::extend_reflections::init();
            gc::OSReport("[mod]  Reflective surface enhancements enabled!\n");
        }
        else if KEY_ENABLED("custom-music-id") {
            relpatches::music_id_per_stage::init();
            gc::OSReport("[mod]  Custom music ID patch enabled!\n");
        }
        else if KEY_ENABLED("custom-theme-id") {
            relpatches::theme_id_per_stage::init();
            gc::OSReport("[mod]  Custom theme ID patch enabled!\n");
        }
        else if KEY_ENABLED("skip-intro-movie") {
            relpatches::skip_intro_movie::init();
            gc::OSReport("[mod]  Skipping intro movie!\n");
        }
        else if KEY_ENABLED("smb1-camera-toggle") {
            relpatches::smb1_camera_toggle::init();
            tick_funcs.push_back(relpatches::smb1_camera_toggle::tick);
            gc::OSReport("[mod]  SMB1 camera toggle enabled!\n");
        }
        else if KEY_ENABLED("fix-missing-w") {
            main_game_init_funcs.push_back(relpatches::fix_missing_w::init_main_game);
            gc::OSReport("[mod]  Missing 'w' restored!");
        }
        else if KEY_ENABLED("apesphere-practice") {
            strcpy(reinterpret_cast<char *>(0x8047f4ec), "APESPHERE PRACTICE MOD");
            patch::write_branch(reinterpret_cast<void *>(0x8032ad0c),
                                reinterpret_cast<void *>(main::custom_titlescreen_text_color));

            draw::init();
            Tetris::get_instance().init();
            tetris_enabled = true;
            savestate::init();
            timer::init();
            iw::init();
            scratch::init();

            tick_funcs.push_back(&unlock_everything);
            tick_funcs.push_back(&timer::tick);
            tick_funcs.push_back(&iw::tick);
            tick_funcs.push_back(&savestate::tick);
            tick_funcs.push_back(&menu::tick);
            tick_funcs.push_back(&jump::tick);
            tick_funcs.push_back(&scratch::tick);

            disp_funcs.push_back(draw::predraw);
            disp_funcs.push_back(draw::disp);
            disp_funcs.push_back(timer::disp);
            disp_funcs.push_back(iw::disp);
            disp_funcs.push_back(menu::disp);
            gc::OSReport("[mod]  ApeSphere practice mod enabled!\n");
        }
        else {
            gc::OSReport("[mod]  Unknown option: %s\n", key);
        }
        buf = end_of_line+1;
        memset(key, '\0', 64);
        memset(value, '\0', 64);
    }
    while (buf < end_of_section);
}

void init()
{
    gc::OSReport("[mod] ApeSphere-Custom version 0.3.0 loaded\n");
    heap::init();
    perform_assembly_patches();

    patch::write_word(reinterpret_cast<void*>(0x803dd448), PPC_INSTR_LI(PPC_R3, 12));

    // Load our config file
    config_file_length = gc::DVDOpen(config_file_path, &config_file_info);
    if (config_file_length != 0) {
        // Round the length of the config file to a multiple of 32, necessary for DVDReadAsyncPrio
        config_file_length = (config_file_info.length + 0x1f) & 0xffffffe0;
        config_file_buf = static_cast<char*>(heap::alloc_from_heap(config_file_length));
        config_file_length = gc::read_entire_file_using_dvdread_prio_async(&config_file_info, config_file_buf, config_file_length, 0);
        char* eof = config_file_buf + config_file_info.length;

        if (config_file_length != 0) {
            gc::OSReport("[mod] Now parsing config file...\n");
            char section[64] = {0};
            char *file = config_file_buf;
            do {
                char *section_start, *section_end;
                // Parse the start of a section of the config starting with # and ending with {
                // Example: # Section {
                section_start = strchr(file, '#');
                section_end = strchr(file, '{');
                if (section_start != nullptr && section_end != nullptr) {
                    // Strip out the '# ' at the start of string, strip out the ' ' at the end
                    section_start += 2;
                    section_end -= 1;

                    strncpy(section, section_start, (section_end-section_start));

                    gc::OSReport("[mod] Now parsing category %s...\n", section);

                    // Parsing function toggles
                    if (STREQ(section, "REL Patches") || STREQ(section, "ApeSphere")) {
                        parse_function_toggles(section_end);
                    }

                    else if (STREQ(section, "Theme IDs")) {
                        parse_stageid_list(section_end, theme_id_lookup);
                        gc::OSReport("[mod]  Theme ID list loaded at: 0x%X\n", &theme_id_lookup);
                    }

                    else if (STREQ(section, "Difficulty Layout")) {
                        gc::OSReport("%s\n", section);
                    }

                    else if (STREQ(section, "Music IDs")) {
                        parse_stageid_list(section_end, bgm_id_lookup);
                        gc::OSReport("[mod]  Music ID list loaded at: 0x%X\n", &bgm_id_lookup);
                    }

                    else {
                        gc::OSReport("[mod]  Unknown category %s found in config!\n", section);
                    }

                    file = strchr(section_end, '\n')+1;
                    memset(section, '\0', 64);
                }
                else {
                    break;
                }
            }
            while (file <= eof);

        }
        gc::DVDClose(&config_file_info);
        heap::free_to_heap(config_file_buf);
    }


    relpatches::party_game_toggle::init();
    s_draw_debug_text_trampoline = patch::hook_function(
        mkb::draw_debugtext, []()
        {
            // Drawing hook for UI elements.
            // Gets run at the start of smb2's function which draws debug text windows,
            // which is called at the end of smb2's function which draws the UI in general.


            // Disp functions
            for (unsigned int i = 0; i < disp_funcs.size(); i++) {
                disp_funcs[i]();
            }

            if (tetris_enabled) {
                Tetris::get_instance().disp();
            }

            s_draw_debug_text_trampoline();
        });

    s_process_inputs_trampoline = patch::hook_function(
        mkb::process_inputs, []()
        {
            s_process_inputs_trampoline();

            // These run after all controller inputs have been processed on the current frame,
            // to ensure lowest input delay
            // Tick functions
            for (unsigned int i = 0; i < tick_funcs.size(); i++) {
                tick_funcs[i]();
            }

            pad::tick();
        });

    load_additional_rel_trampoline = patch::hook_function(
        mkb::load_additional_rel, [](const char *rel_filepath, void *rel_buffer_ptrs)
        {
            load_additional_rel_trampoline(rel_filepath, rel_buffer_ptrs);

            if (STREQ(rel_filepath, "mkb2.main_game.rel")) {
                relpatches::skip_cutscenes::init_main_game();
                for (unsigned int i = 0; i < main_game_init_funcs.size(); i++) {
                    main_game_init_funcs[i]();
                }
            }
            else if (STREQ(rel_filepath, "mkb2.sel_ngc.rel")) {
                relpatches::party_game_toggle::sel_ngc_init();
                for (unsigned int i = 0; i < sel_ngc_init_funcs.size(); i++) {
                    sel_ngc_init_funcs[i]();
                }
            }
        });
}

/*
 * This runs at the very start of the main game loop. Most per-frame code runs after
 * controller inputs have been read and processed however, to ensure the lowest input delay.
 */
void tick()
{
    /*
    if (debug_mode_enabled)
    {
        mkb::dip_switches |= mkb::DIP_DEBUG | mkb::DIP_DISP;
    }
    else
    {
        mkb::dip_switches &= ~(mkb::DIP_DEBUG | mkb::DIP_DISP);
    }*/
    pad::on_frame_start();
}

}
