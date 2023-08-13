#include "config/config.h"
#include "internal/assembly.h"
#include "internal/heap.h"
#include "internal/modlink.h"
#include "internal/pad.h"
#include "internal/patch.h"
#include "internal/version.h"
#include "mkb/mkb.h"
#include "patches/tickable.h"

namespace main {
static patch::Tramp<decltype(&mkb::process_inputs)> s_process_inputs_tramp;

bool debug_mode_enabled = false;

static void perform_assembly_patches() {
    // Inject the run function at the start of the main game loop
    patch::write_branch_bl(reinterpret_cast<void*>(0x80270700),
                           reinterpret_cast<void*>(start_main_loop_assembly));

    /* Remove OSReport call ``PERF : event is still open for CPU!``
since it reports every frame, and thus clutters the console */
    // Only needs to be applied to the US version
    patch::write_nop(reinterpret_cast<void*>(0x80033E9C));

    // Nop the conditional that guards `draw_debugtext`, enabling it even when debug mode is disabled
    patch::write_nop(reinterpret_cast<void*>(0x80299f54));
}

void init() {
    mkb::OSReport("[wsmod] SMB2 Workshop Mod v%d.%d.%d loaded\n",
                  version::WSMOD_VERSION.major,
                  version::WSMOD_VERSION.minor,
                  version::WSMOD_VERSION.patch);

    heap::init();
    modlink::write();

    perform_assembly_patches();
    tickable::get_tickable_manager().init();

    // Load our config file
    config::parse_config();

    /*
    patch::hook_function(
        s_process_inputs_tramp, mkb::process_inputs, []() {
            s_process_inputs_tramp.dest();

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
        */
}

/*
 * This runs at the very start of the main game loop. Most per-frame code runs after
 * controller inputs have been read and processed however, to ensure the lowest input delay.
 */
void tick() {
    pad::on_frame_start();
}

}// namespace main
