#include "fix_wormhole_surfaces.h"

#include "internal/patch.h"
#include "patches/tickable.h"
#include "util/ppcutil.h"

namespace fix_wormhole_surfaces {

TICKABLE_DEFINITION((
        .name = "fix-wormhole-surfaces",
        .description = "[wsmod]  Party game stage slot fix %s\n",
        .init_main_loop = init_main_loop, ))

// Always return 'true' for a specific function that checks if the stage ID
// belongs to a slot normally used for party games.
void init_main_loop() {
    patch::write_word(reinterpret_cast<void*>(0x802c8ce4), PPC_INSTR_LI(PPC_R0, 0x1));
}

}// namespace fix_wormhole_surfaces
