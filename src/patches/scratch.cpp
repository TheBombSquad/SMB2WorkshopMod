#include "scratch.h"
#include "internal/tickable.h"

namespace scratch {

TICKABLE_DEFINITION((
        .name = "scratch",
        .description = "Scratch",
        .enabled = false,
        .init_main_loop = init,
        .tick = tick))

void init() {}
void tick() {}
void disp() {}

}// namespace scratch
