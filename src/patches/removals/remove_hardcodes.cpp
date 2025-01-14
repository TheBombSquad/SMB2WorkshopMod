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
    // Labyrinth camera removal
    // Always compare the stage ID to 0xFFFF when these camera functions check
    // if the current stage ID is 0x15a when determining specific constants.
    // 0x2c00ffff = cmpwi r0. 0xFFFF
    patch::write_word(reinterpret_cast<void*>(0x802858D4), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x802874BC), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x8028751C), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x802880EC), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x802881D4), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x802883B4), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x802886B8), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x8028BF44), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x8028C1CC), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x8028C650), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x8028CA84), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291338), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291420), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291664), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291904), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291930), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291960), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x8029198C), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291AEC), 0x2c00ffff);
    // Wormhole surface fix
    // Always return 'true' for a specific function that checks if the stage ID
    // belongs to a slot normally used for party games.
    patch::write_word(reinterpret_cast<void*>(0x802c8ce4), PPC_INSTR_LI(PPC_R0, 0x1));
}

void init_main_game() {
    // Nop a call to handle hardcoded stage object drawing for
    // SMB2 stages
    patch::write_nop(reinterpret_cast<void*>(0x809140f4));
}

}// namespace remove_hardcodes
