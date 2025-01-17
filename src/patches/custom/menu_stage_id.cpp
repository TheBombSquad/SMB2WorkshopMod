#include "menu_stage_id.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace menu_stage_id {

u16 stage_id;

// Always enabled

TICKABLE_DEFINITION((
        .name = "menu-stage-id",
        .description = "Menu stage ID",
        .active_value = stage_id,
        .lower_bound = 1,
        .upper_bound = 420,
        .init_main_loop = init_main_loop,
        .init_main_game = init_main_game))

// In functions which handle the menu stage to load, use our stage ID
void init_main_loop() {
    stage_id = *active_tickable_ptr->active_value;// Get our stage ID
    patch::write_word(reinterpret_cast<void*>(0x80282c10), PPC_INSTR_LI(PPC_R31, stage_id));
    patch::write_word(reinterpret_cast<void*>(0x80282c20), PPC_INSTR_LI(PPC_R31, stage_id));
}

void init_main_game() {
    patch::write_word(reinterpret_cast<void*>(0x808fd958), PPC_INSTR_LI(PPC_R3, stage_id));
    patch::write_word(reinterpret_cast<void*>(0x808fe7d8), PPC_INSTR_LI(PPC_R3, stage_id));
}

}// namespace menu_stage_id
