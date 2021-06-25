#include "patch.h"
#include "assembly.h"
#include "heap.h"
#include "tetris.h"
#include "pad.h"
#include "config.h"
#include "main.h"
#include <mkb/mkb.h>
#include <gc/dvd.h>

#include <cstring>
#include <vector>


#define STREQ(x,y) (strcmp(x,y)==0)

namespace main
{

static void (*s_draw_debug_text_trampoline)();
static void (*s_process_inputs_trampoline)();
static void (*load_additional_rel_trampoline)(const char *rel_filepath, void *rel_buffer_ptrs);

// For dynamically choosing to run init/tick/disp functions based on a config file
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



void init()
{
    gc::OSReport("[mod] ApeSphere-Custom version 0.3.0 loaded\n");
    heap::init();
    perform_assembly_patches();

    // Load our config file
    config::parse_config();

    s_draw_debug_text_trampoline = patch::hook_function(
        mkb::draw_debugtext, []()
        {
            // Drawing hook for UI elements.
            // Gets run at the start of smb2's function which draws debug text windows,
            // which is called at the end of smb2's function which draws the UI in general.


            // Disp functions
            for (unsigned int i = 0; i < config::disp_funcs.size(); i++) {
                config::disp_funcs[i]();
            }

            if (config::tetris_enabled) {
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
            for (unsigned int i = 0; i < config::tick_funcs.size(); i++) {
                config::tick_funcs[i]();
            }

            pad::tick();
        });

    load_additional_rel_trampoline = patch::hook_function(
        mkb::load_additional_rel, [](const char *rel_filepath, void *rel_buffer_ptrs)
        {
            load_additional_rel_trampoline(rel_filepath, rel_buffer_ptrs);

            if (STREQ(rel_filepath, "mkb2.main_game.rel")) {
                for (unsigned int i = 0; i < config::main_game_init_funcs.size(); i++) {
                    config::main_game_init_funcs[i]();
                }
            }
            else if (STREQ(rel_filepath, "mkb2.sel_ngc.rel")) {
                for (unsigned int i = 0; i < config::sel_ngc_init_funcs.size(); i++) {
                    config::sel_ngc_init_funcs[i]();
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
