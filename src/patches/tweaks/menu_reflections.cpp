#include "menu_reflections.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

// Enables reflective surfaces on the menu. Experimental and I don't really know why this works.
// Assumes the menu stage slot is 3 or 201.
// TODO: If/when I make a patch/toggle for changing the menu BG stage slots, don't hardcode the value.
namespace menu_reflections {

TICKABLE_DEFINITION((
        .name = "enable-menu-reflections",
        .description = "Menu reflection patch",
        .init_main_loop = init_main_loop, ))

static patch::Tramp<decltype(&mkb::queue_stage_load)> s_load_stage_1_tramp;
static patch::Tramp<decltype(&mkb::load_stage)> s_load_stage_2_tramp;

void rendefc_handler(u32 stage_id) {
    if (mkb::main_mode == mkb::MD_SEL) {
        if (stage_id == 3 || stage_id == 201) {
            if (mkb::events[mkb::EVENT_REND_EFC].status == mkb::STAT_NULL) {
                mkb::OSReport("Created menu rendefc stage %d\n", stage_id);
                mkb::event_init(mkb::EVENT_REND_EFC);
            }
        }
        else {
            mkb::OSReport("Destroyed menu rendefc stage %d\n", stage_id);
            mkb::event_dest(mkb::EVENT_REND_EFC);
        }
    }
}

void init_main_loop() {
    patch::hook_function(
        s_load_stage_1_tramp, mkb::queue_stage_load, [](u32 stage_id) {
            rendefc_handler(stage_id);
            s_load_stage_1_tramp.dest(stage_id);
        });
    patch::hook_function(
        s_load_stage_2_tramp, mkb::load_stage, [](int stage_id) {
            rendefc_handler(stage_id);
            s_load_stage_2_tramp.dest(stage_id);
        });
}
}// namespace menu_reflections
