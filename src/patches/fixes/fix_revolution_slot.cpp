#include "fix_revolution_slot.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace fix_revolution_slot {

TICKABLE_DEFINITION((
        .name = "fix-revolution-slot",
        .description = "Revolution stage slot fix",
        .init_main_loop = init_main_loop, ))

// Always return 'false' for a specific function that checks if the stage ID
// is 348 when determining whether or not to handle level loading specially
void init_main_loop() {
    patch::write_word(reinterpret_cast<void*>(0x802ca9fc), PPC_INSTR_LI(PPC_R3, 0x0));
}

}// namespace fix_revolution_slot
