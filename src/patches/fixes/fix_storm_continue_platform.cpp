#include "fix_storm_continue_platform.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

// Fixes an issue with rain droplets not appearing correctly on the continue platform in the storm theme.
namespace fix_storm_continue_platform {

TICKABLE_DEFINITION((
        .name = "fix-storm-continue-platform",
        .description = "[wsmod]  Storm continue platform patch %s\n",
        .init_main_loop = init_main_loop, ))

void init_main_loop() {
    patch::write_branch(reinterpret_cast<void*>(mkb::effect_bgstm_rainripple_disp),
                        reinterpret_cast<void*>(main::fix_rain_ripple));
}

}// namespace fix_storm_continue_platform
