#include "remove_extras.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace remove_extras {

TICKABLE_DEFINITION((
        .name = "remove-extras",
        .description = "Remove extras",
        .init_main_game = init_main_game, ))

// Always return 'false' in the function which handles what to do
// once you enter a goal
void init_main_game() {
    patch::write_word(reinterpret_cast<void*>(0x808f61a8), PPC_INSTR_LI(PPC_R4, 0x0));
}

}// namespace remove_extras
