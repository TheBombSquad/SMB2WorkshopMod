#include "pause_volume_fix.h"

#include "internal/patch.h"
#include "internal/tickable.h"

namespace pause_volume_fix {

TICKABLE_DEFINITION((
        .name = "no-music-vol-decrease-on-pause",
        .description = "No music volume decrease on pause",
        .init_main_loop = init_main_loop, ))

// Nop a call to a function that decreases in-game volume on pause
void init_main_loop() {
    patch::write_nop(reinterpret_cast<void*>(0x802a32a8));
}

}// namespace pause_volume_fix
