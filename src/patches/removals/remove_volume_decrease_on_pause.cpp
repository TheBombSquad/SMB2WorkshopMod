#include "remove_volume_decrease_on_pause.h"

#include "internal/patch.h"
#include "internal/tickable.h"

namespace remove_volume_decrease_on_pause {

TICKABLE_DEFINITION((
        .name = "remove-volume-decrease-on-pause",
        .description = "No music volume decrease on pause",
        .init_main_loop = init_main_loop, ))

// Nop a call to a function that decreases in-game volume on pause
void init_main_loop() {
    patch::write_nop(reinterpret_cast<void*>(0x802a32a8));
}

}// namespace remove_volume_decrease_on_pause
