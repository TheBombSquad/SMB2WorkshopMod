#include "remove_ball_rolling_sound.h"

#include "internal/patch.h"
#include "internal/tickable.h"

namespace remove_ball_rolling_sound {

TICKABLE_DEFINITION((
        .name = "remove-ball-rolling-sound",
        .description = "Ball rolling sound removal",
        .init_main_loop = init_main_loop, ))

// Nops a call to the function which plays ball rolling sounds
void init_main_loop() {
    patch::write_nop(reinterpret_cast<void*>(0x802bca7c));
}

}// namespace remove_ball_rolling_sound
