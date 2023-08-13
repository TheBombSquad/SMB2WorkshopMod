#include "perfect_bonus.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"


namespace perfect_bonus {

TICKABLE_DEFINITION((
        .name = "perfect-bonus-completion",
        .description = "[wsmod]  Perfect bonus completion %s\n",
        .init_main_loop = init_main_loop, ))

// If the stage is a bonus stage (ball mode 0x40) and no bananas remain,
// end the stage with a perfect (|= 0x228)
void init_main_loop() {
    static patch::Tramp<decltype(&mkb::event_info_tick)> event_info_tick_tramp;

    patch::hook_function(
        event_info_tick_tramp,
        mkb::event_info_tick, []() {
            event_info_tick_tramp.dest();
            if (mkb::mode_info.ball_mode == mkb::BALLMODE_ON_BONUS_STAGE &&
                mkb::mode_info.bananas_remaining == 0) {
                mkb::mode_info.ball_mode |= 0x228;
            }
        });
}

}// namespace perfect_bonus
