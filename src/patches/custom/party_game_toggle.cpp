#include "party_game_toggle.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"
#include "utils/ppcutil.h"

// Allows for party games to be toggled with a config option.
namespace party_game_toggle {

TICKABLE_DEFINITION((
        .name = "party-game-toggle",
        .description = "Party game toggle patch",
        .init_sel_ngc = init_sel_ngc, ))

u16 party_game_bitflag = 0;

// Variable-precision SWAR algorithm
// Source: https://stackoverflow.com/a/109025
u32 number_of_unlocked_party_games(u32 i) {
    i = i - ((i >> 1) & 0x55555555);               // add pairs of bits
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);// quads
    i = (i + (i >> 4)) & 0x0F0F0F0F;               // groups of 8
    return (i * 0x01010101) >> 24;                 // horizontal sum of bytes
}

// Party game IDs do not match their menu positions, hence this ugly mess
u32 determine_party_game_unlock_status(int id) {
    switch (id) {
        case 0xa:
            return (party_game_bitflag & 0x40);
        case 0x9:
            return (party_game_bitflag & 0x80);
        case 0xd:
            return (party_game_bitflag & 0x100);
        case 0xc:
            return (party_game_bitflag & 0x200);
        case 0xf:
            return (party_game_bitflag & 0x400);
        case 0x10:
            return (party_game_bitflag & 0x800);
        default:
            return 0;
    }
}

void init_sel_ngc() {
    patch::hook_function(mkb::g_check_if_partygame_unlocked, determine_party_game_unlock_status);
    patch::write_word(reinterpret_cast<void*>(0x808f9154), PPC_INSTR_LI(PPC_R0, (~party_game_bitflag & 0x3f)));

    mkb::strcpy(mkb::CANNOT_SELECT_PARTY_GAME_STRING, "You cannot play this game\n in this custom pack.");
    mkb::strcpy(mkb::CAN_PURCHASE_PARTY_GAME_STRING, "You cannot unlock this game\n in this custom pack.");
    mkb::sprintf(mkb::CAN_PLAY_NUM_PARTY_GAMES_STRING, "You can play /bcff8000/%d/bcffff00/ party games!",
                 number_of_unlocked_party_games(party_game_bitflag));
}

}// namespace party_game_toggle
