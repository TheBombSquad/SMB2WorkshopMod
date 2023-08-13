#include "custom_world_count.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

// Allows for the number of worlds in story mode to be customized.
namespace custom_world_count {

static u16 DEFAULT_WORLD_COUNT = 10;

TICKABLE_DEFINITION((
        .name = "custom-world-count",
        .description = "Custom world count patch",
        .active_value = DEFAULT_WORLD_COUNT,
        .lower_bound = 1,
        .upper_bound = 10,
        .init_main_game = init_main_game,
        .init_sel_ngc = init_sel_ngc, ))

static patch::Tramp<decltype(&mkb::dmd_scen_sceneplay_init)> s_sceneplay_init_tramp;

u16 get_world_count() {
    if (!active_tickable_ptr->enabled) {
        return DEFAULT_WORLD_COUNT;
    }
    else {
        return *active_tickable_ptr->active_value;
    }
}
void dmd_scen_sceneplay_init_patch() {
    if (mkb::scen_info.next_world == get_world_count()) {
        mkb::scen_info.next_world = 10;
    }
    s_sceneplay_init_tramp.dest();
}

void init_main_game() {
    mkb::OSReport("set world count to %d\n", get_world_count());

    if (tickable::get_tickable_manager().get_tickable_status("skip-cutscenes")) {
        mkb::OSReport("hooking sceneplay\n");

        patch::hook_function(s_sceneplay_init_tramp,
                             mkb::dmd_scen_sceneplay_init, dmd_scen_sceneplay_init_patch);
    }
}

void init_sel_ngc() {
    // Update the practice mode story mode display counter to show the proper number of worlds

    // Visually update the indicator
    patch::write_word(reinterpret_cast<void*>(0x8090dbd0),
                      (0x2c1a0000 | *active_tickable_ptr->active_value));
    // Update the indicator logic
    patch::write_word(reinterpret_cast<void*>(0x80900f08),
                      PPC_INSTR_LI(PPC_R29, *active_tickable_ptr->active_value));
}

}// namespace custom_world_count
