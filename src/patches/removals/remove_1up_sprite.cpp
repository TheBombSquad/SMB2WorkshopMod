#include "remove_1up_sprite.h"

#include "internal/patch.h"
#include "internal/tickable.h"

namespace remove_1up_sprite {

TICKABLE_DEFINITION((
        .name = "remove-1up-sprite",
        .description = "1UP sprite removal",
        .init_main_loop = init_main_loop, ))

// Nops a call to the function which draws the 1UP sprite
void init_main_loop() {
    patch::write_nop(reinterpret_cast<void*>(0x802b8264));
}

}// namespace remove_1up_sprite
