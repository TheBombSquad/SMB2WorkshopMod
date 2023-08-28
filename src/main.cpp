#include "config/config.h"
#include "internal/assembly.h"
#include "internal/heap.h"
#include "internal/log.h"
#include "internal/modlink.h"
#include "internal/pad.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "internal/ui/ui_manager.h"
#include "internal/version.h"
#include "mkb/mkb.h"

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
    LOG("SMB2 Workshop Mod v%d.%d.%d loaded",
        version::WSMOD_VERSION.major,
        version::WSMOD_VERSION.minor,
        version::WSMOD_VERSION.patch);

    heap::init();
    modlink::write();

    perform_assembly_patches();

    // Load our config file
    config::parse_config();

    // Init all tickables/patches
    tickable::get_tickable_manager().init();

    patch::hook_function(
        s_process_inputs_tramp, mkb::process_inputs, []() {
            s_process_inputs_tramp.dest();

            // These run after all controller inputs have been processed on the current frame,
            // to ensure lowest input delay

            pad::tick();

            // Tick functions (REL patches)
            for (const auto& tickable: tickable::get_tickable_manager().get_tickables()) {
                if (tickable->enabled && tickable->tick) {
                    tickable->tick();
                }
            }

            ui::get_widget_manager().tick();
        });
}

/*
 * This runs at the very start of the main game loop. Most per-frame code runs after
 * controller inputs have been read and processed however, to ensure the lowest input delay.
 */
void tick() {
    pad::on_frame_start();
}

}// namespace main
