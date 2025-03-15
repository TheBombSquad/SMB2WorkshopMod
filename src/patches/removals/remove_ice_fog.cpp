#include "remove_ice_fog.h"

#include "internal/patch.h"
#include "internal/tickable.h"

namespace remove_ice_fog {

TICKABLE_DEFINITION((
        .name = "remove-ice-fog",
        .description = "Ice fog removal",
        .init_main_loop = init_main_loop, ))

void init_main_loop() {
    mkb::ICE_FOG_THEME_ID = 0xff;
}

}// namespace remove_ice_fog
