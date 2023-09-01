#include "fix_view_stage.h"

#include "internal/patch.h"
#include "utils/ppcutil.h"
#include "internal/tickable.h"

namespace fix_view_stage {

TICKABLE_DEFINITION((
.name = "fix-view-stage",
.description = "Various 'View Stage' fixes",
.enabled = true, //TODO: this is for debug purposes, add to default config later
.init_main_game = init_main_game, ))

void init_main_game() {
    // Prevent background animations from animating twice as fast in 'View Stage'
    patch::write_word(reinterpret_cast<void*>(0x80912d90), PPC_INSTR_NOP());
}

}
