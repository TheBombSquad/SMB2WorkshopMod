#include "remove_playpoints.h"

#include "internal/patch.h"
#include "mkb/mkb.h"
#include "patches/tickable.h"

// Removes the playpoint notification screens when exiting from story mode or challenge mode, or after a 'game over'.
namespace remove_playpoints {

TICKABLE_DEFINITION((
        .name = "remove-playpoints",
        .description = "[wsmod]  Playpoint removal patch %s\n",
        .init_main_game = init_main_game,
        .tick = tick, ))

void init_main_game() {
    // Removes playpoint screen when exiting challenge/story mode.
    patch::write_nop(reinterpret_cast<void*>(0x808f9ecc));
    patch::write_nop(reinterpret_cast<void*>(0x808f9eec));

    // Removes playpoint screen after the 'game over' sequence.
    patch::write_nop(reinterpret_cast<void*>(0x808f801c));
    patch::write_nop(reinterpret_cast<void*>(0x808f803c));
}

void tick() {
    mkb::unlock_info.party_games = 0x0001b600;
}
}// namespace remove_playpoints
