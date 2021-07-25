#include "patch.h"
#include "assembly.h"
#include "heap.h"
#include "tetris.h"
#include "pad.h"
#include "config.h"
#include <mkb.h>

#include <cstring>

#define STREQ(x,y) (mkb::strcmp(const_cast<char*>(x),const_cast<char*>(y))==0)

namespace main
{

static void (*s_draw_debug_text_trampoline)();
static void (*s_process_inputs_trampoline)();
static void (*load_additional_rel_trampoline)(char *rel_filepath, mkb::RelBufferInfo *rel_buffer_ptrs);

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
    mkb::OSReport("[mod] ApeSphere-Custom version 0.3.0 loaded\n");
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

            // Disp functions (REL patches)
            for (unsigned int i = 0; i < relpatches::PATCH_COUNT; i++) {
                if (relpatches::patches[i].enabled && relpatches::patches[i].disp_func != nullptr) {
                    relpatches::patches[i].disp_func();
                }
            }

            // Disp functions (ApeSphere)
            for (unsigned int i = 0; i < config::APESPHERE_TICKABLE_COUNT; i++) {
                if (config::apesphere_tickables[i].enabled && config::apesphere_tickables[i].disp_func != nullptr) {
                    config::apesphere_tickables[i].disp_func();
                }
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

            if (mkb::main_mode == mkb::MD_ADV && config::apesphere_toggle_enabled && pad::button_chord_pressed(mkb::PAD_TRIGGER_L, mkb::PAD_TRIGGER_R)) {
                config::init_apesphere_tickables();
            }

            // Tick functions (REL patches)
            for (unsigned int i = 0; i < relpatches::PATCH_COUNT; i++) {
                if (relpatches::patches[i].enabled && relpatches::patches[i].tick_func != nullptr) {
                    relpatches::patches[i].tick_func();
                }
            }

            // Tick functions (ApeSphere)
            for (unsigned int i = 0; i < config::APESPHERE_TICKABLE_COUNT; i++) {
                if (config::apesphere_tickables[i].enabled && config::apesphere_tickables[i].tick_func != nullptr) {
                    config::apesphere_tickables[i].tick_func();
                }
            }

            pad::tick();
        });

    load_additional_rel_trampoline = patch::hook_function(
        mkb::load_additional_rel, [](char *rel_filepath, mkb::RelBufferInfo *rel_buffer_ptrs)
        {
            load_additional_rel_trampoline(rel_filepath, rel_buffer_ptrs);

            // Main game init functions
            if (STREQ(rel_filepath, "mkb2.main_game.rel")) {
                for (unsigned int i = 0; i < relpatches::PATCH_COUNT; i++) {
                    if (relpatches::patches[i].enabled && relpatches::patches[i].main_game_init_func != nullptr) {
                        relpatches::patches[i].main_game_init_func();
                    }
                }
            }

            // Sel_ngc init functions
            else if (STREQ(rel_filepath, "mkb2.sel_ngc.rel")) {
                for (unsigned int i = 0; i < relpatches::PATCH_COUNT; i++) {
                    if (relpatches::patches[i].enabled && relpatches::patches[i].sel_ngc_init_func != nullptr) {
                        relpatches::patches[i].sel_ngc_init_func();
                    }
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
