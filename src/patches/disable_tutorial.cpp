#include "disable_tutorial.h"

#include "internal/patch.h"
#include "patches/tickable.h"
#include "util/ppcutil.h"

namespace disable_tutorial {

TICKABLE_DEFINITION((
        .name = "disable-how-to-play-screen",
        .description = "[wsmod]  Tutorial sequence removal %s\n",
        .init_main_loop = init_main_loop, ))

// Nops the sub_mode_frame_counter decrement in smd_adv_title_tick.
// This ensures the tutorial sequence will never start.
void init_main_loop() {
    patch::write_nop(reinterpret_cast<void*>(0x8027bbb0));
}

}// namespace disable_tutorial
