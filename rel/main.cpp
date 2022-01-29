#include "patch.h"
#include "assembly.h"
#include "heap.h"
#include "pad.h"
#include "config.h"
#include "version.h"
#include "modlink.h"
#include <mkb.h>

#define STREQ(x,y) (mkb::strcmp(const_cast<char*>(x),const_cast<char*>(y))==0)

namespace main
{

static void (*s_draw_debug_text_trampoline)();
static void (*s_process_inputs_trampoline)();
static void (*load_additional_rel_trampoline)(char *rel_filepath, mkb::RelBufferInfo *rel_buffer_ptrs);

bool debug_mode_enabled = false;

static void perform_assembly_patches()
{
    // Inject the run function at the start of the main game loop
    patch::write_branch_bl(reinterpret_cast<void*>(0x80270700),
                           reinterpret_cast<void*>(start_main_loop_assembly));

    /* Remove OSReport call ``PERF : event is still open for CPU!``
    since it reports every frame, and thus clutters the console */
    // Only needs to be applied to the US version
    patch::write_nop(reinterpret_cast<void *>(0x80033E9C));

    // Nop the conditional that guards `draw_debugtext`, enabling it even when debug mode is disabled
    patch::write_nop(reinterpret_cast<void *>(0x80299f54));
}

void init()
{
    mkb::OSReport("[wsmod] SMB2 Workshop Mod v%d.%d.%d loaded\n",
                  version::WSMOD_VERSION.major,
                  version::WSMOD_VERSION.minor,
                  version::WSMOD_VERSION.patch);

    heap::init();
    modlink::write();

    perform_assembly_patches();

    return;

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
                if (relpatches::patches[i].status && relpatches::patches[i].disp_func != nullptr) {
                    relpatches::patches[i].disp_func();
                }
            }

            s_draw_debug_text_trampoline();
        });

    s_process_inputs_trampoline = patch::hook_function(
        mkb::process_inputs, []()
        {
            s_process_inputs_trampoline();

            // These run after all controller inputs have been processed on the current frame,
            // to ensure lowest input delay

            // Tick functions (REL patches)
            for (unsigned int i = 0; i < relpatches::PATCH_COUNT; i++) {
                if (relpatches::patches[i].status && relpatches::patches[i].tick_func != nullptr) {
                    relpatches::patches[i].tick_func();
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
                    if (relpatches::patches[i].status && relpatches::patches[i].main_game_init_func != nullptr) {
                        relpatches::patches[i].main_game_init_func();
                    }
                }
            }

            // Sel_ngc init functions
            else if (STREQ(rel_filepath, "mkb2.sel_ngc.rel")) {
                for (unsigned int i = 0; i < relpatches::PATCH_COUNT; i++) {
                    if (relpatches::patches[i].status && relpatches::patches[i].sel_ngc_init_func != nullptr) {
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
