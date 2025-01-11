#include "remove_hardcodes.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace remove_hardcodes {

TICKABLE_DEFINITION((
        .name = "remove-stage-slot-hardcodes",
        .description = "Remove stage slot hardcodes",
        .init_main_loop = init_main_loop,
        .init_main_game = init_main_game, ))

void init_main_loop() {
    // Nop a call to handle hardcoded stage object drawing for
    // SMB2 stages
    patch::write_nop(reinterpret_cast<void*>(0x802c96d8));
    // Nop a call to handle hardcoded stage lights leftover from
    // SMB1
    patch::write_nop(reinterpret_cast<void*>(0x802945d8));
    // In the function which handles Bonus Wave's collision, nop
    // the part which decides what space has hardcoded collision
    patch::write_nop(reinterpret_cast<void*>(0x802c79b4));
    // In an array for hardcoded camera settings, check for stage ID
    // 0xffffffff instead of any used stage slots
    patch::write_word(reinterpret_cast<void*>(0x8044b208), 0xffffffff);
    patch::write_word(reinterpret_cast<void*>(0x8044b1e0), 0xffffffff);
    patch::write_word(reinterpret_cast<void*>(0x8044b1f4), 0xffffffff);
}

void init_main_game() {
    // Nop a call to handle hardcoded stage object drawing for
    // SMB2 stages
    patch::write_nop(reinterpret_cast<void*>(0x809140f4));
}

}// namespace remove_hardcodes
