#include "remove_challenge_cutscenes.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"
#include "utils/ppcutil.h"


namespace remove_challenge_cutscenes {

TICKABLE_DEFINITION((
        .name = "remove-challenge-cutscenes",
        .description = "Challenge Mode cutscene removal",
        .init_main_game = init_main_game, ))

void init_main_game() {
        patch::write_word(reinterpret_cast<void*>(0x808f6274), PPC_INSTR_LI(PPC_R0, mkb::MD_GAME));
        patch::write_word(reinterpret_cast<void*>(0x808f6284), PPC_INSTR_LI(PPC_R0, mkb::SMD_GAME_ROLL_INIT));
}

}// namespace remove_challenge_cutscenes
