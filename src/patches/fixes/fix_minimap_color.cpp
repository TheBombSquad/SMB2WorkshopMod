#include "fix_minimap_color.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace fix_minimap_color {

TICKABLE_DEFINITION((
        .name = "fix-minimap-color",
        .description = "Minimap color fix",
        .init_main_loop = init_main_loop, ))

void init_main_loop() {
    // Overwrite Baby's minimap color with pure white like the other monkeys
    patch::write_word(reinterpret_cast<void*>(0x80494494), (0x00ffffff));
}

}// namespace fix_minimap_color
