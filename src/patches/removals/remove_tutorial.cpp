#include "remove_tutorial.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace remove_tutorial {

TICKABLE_DEFINITION((
        .name = "remove-tutorial",
        .description = "Tutorial sequence removal",
        .init_main_loop = init_main_loop, ))

// Nops the sub_mode_frame_counter decrement in smd_adv_title_tick.
// This ensures the tutorial sequence will never start.
void init_main_loop() {
    patch::write_nop(reinterpret_cast<void*>(0x8027bbb0));
}

}// namespace remove_tutorial
