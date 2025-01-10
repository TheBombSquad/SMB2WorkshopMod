#include "pause_cooldown.h"

#include "internal/patch.h"
#include "internal/tickable.h"

namespace pause_cooldown {

TICKABLE_DEFINITION((
        .name = "pause-cooldown",
        .description = "Pause cooldown",
        .tick = tick, ))

// When the game is paused, set the pause cooldown to 60 frames
// every frame, ensuring even once unpaused the player must wait
// a full second before pausing again
void tick() {
    if (mkb::g_pause_status == 2) {
        mkb::g_repause_cooldown_counter = 0x3c;
    }
}

}// namespace pause_cooldown
