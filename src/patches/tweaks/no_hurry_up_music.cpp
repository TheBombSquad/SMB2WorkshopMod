#include "no_hurry_up_music.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace no_hurry_up_music {

TICKABLE_DEFINITION((
        .name = "no-hurry-up-music",
        .description = "[wsmod]  Hurry up music removal %s\n",
        .init_main_game = init_main_game,
        .tick = tick, ))

// Nop out calls to start the hurry-up music. Call after main_game load
void init_main_game() {
    patch::write_nop(reinterpret_cast<void*>(0x808f509c));
    patch::write_nop(reinterpret_cast<void*>(0x808f50a4));
}

// The init function breaks the "Time Over" sound, as it checks to see if the
// hurry up music is playing. This re-imlements the playing of the sound.
void tick() {
    if (mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT) {
        mkb::g_SoftStreamStart_with_some_defaults_2(0x2c);
    }
}
}// namespace no_hurry_up_music
