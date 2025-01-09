#include "fix_storm_continue_platform.h"
#include "internal/patch.h"
#include "internal/tickable.h"

// Fixes an issue with rain droplets not appearing correctly on the continue platform in the storm theme.
namespace fix_storm_continue_platform {

TICKABLE_DEFINITION((
        .name = "fix-storm-continue-platform",
        .description = "Storm continue platform patch",
        .tick = tick, ))

void tick() {
     // Check if we're entering or on the continue screen
   if ((mkb::sub_mode ==(mkb::SMD_GAME_CONTINUE_MAIN) || mkb::sub_mode == (mkb::SMD_GAME_CONTINUE_INIT)) && (mkb::sub_mode_frame_counter >= 2)) { // frame counter check helps prevent race conditions
     // Nop some branches to code which handle rotating the storm raindrops
        patch::write_nop(reinterpret_cast<void*>(0x802de2e4));
        patch::write_nop(reinterpret_cast<void*>(0x802de2ec));
        patch::write_nop(reinterpret_cast<void*>(0x802de2f4));
        patch::write_nop(reinterpret_cast<void*>(0x802de2fc));
   }
   else {
     // Original instructions
        patch::write_word(reinterpret_cast<void*>(0x802de2e4), 0x4bd84c4d);
        patch::write_word(reinterpret_cast<void*>(0x802de2ec), 0x4bd84bd1);
        patch::write_word(reinterpret_cast<void*>(0x802de2f4), 0x4bd84cc9);
        patch::write_word(reinterpret_cast<void*>(0x802de2fc), 0x4bd84bc1);
   }
}

}// namespace fix_storm_continue_platform