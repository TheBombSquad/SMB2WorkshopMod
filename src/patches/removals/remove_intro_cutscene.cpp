#include "remove_intro_cutscene.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace remove_intro_cutscene {

TICKABLE_DEFINITION((
        .name = "remove-intro-cutscene",
        .description = "Intro cutscene removal",
        .init_main_loop = init_main_loop, ))

// Immediately goes to the title screen
void smd_adv_first_logo_tick_patch() {
    mkb::sub_mode_request = mkb::SMD_ADV_MOVIE_RETURN;
}

// Hooks into the smd_adv_first_logo_tick function, calling our own tick function
void init_main_loop() {
    patch::hook_function(mkb::smd_adv_first_logo_tick, smd_adv_first_logo_tick_patch);
}


}// namespace remove_intro_cutscene
