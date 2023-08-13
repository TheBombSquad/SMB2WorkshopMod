#include "custom_world_count.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace custom_world_count {

constexpr auto WORLD_COUNT = 10;// TODO: attach to patch that changes this

TICKABLE_DEFINITION((
        .name = "custom-world-count",
        .description = "Custom world count patch",
        .default_value = 10,
        .lower_bound = 1,
        .upper_bound = 10,
        .init_main_game = init_main_game,
        .init_sel_ngc = init_sel_ngc, ))

// Allows for the number of worlds in story mode to be customized.
static patch::Tramp<decltype(&mkb::dmd_scen_sceneplay_init)> s_sceneplay_init_tramp;

void dmd_scen_sceneplay_init_patch() {
    if (mkb::scen_info.next_world == WORLD_COUNT) {
        mkb::scen_info.next_world = 10;
    }
    s_sceneplay_init_tramp.dest();
}

void init_main_game() {
    mkb::OSReport("set world count to %d\n", WORLD_COUNT);

    /*
    if (!patches[::SKIP_CUTSCENES].status) {
        mkb::OSReport("hooking sceneplay\n");

        patch::hook_function(s_sceneplay_init_tramp,
                             mkb::dmd_scen_sceneplay_init, dmd_scen_sceneplay_init_patch);
    }
*/
}

void init_sel_ngc() {
    // Update the practice mode story mode display counter to show the proper number of worlds

    /*
    // Visually update the indicator
    patch::write_word(reinterpret_cast<void*>(0x8090dbd0),
                      (0x2c1a0000 | patches[Patches::CUSTOM_WORLD_COUNT].status));
    // Update the indicator logic
    patch::write_word(reinterpret_cast<void*>(0x80900f08),
                      PPC_INSTR_LI(PPC_R29, patches[Patches::CUSTOM_WORLD_COUNT].status));
                      */
}

}// namespace custom_world_count
