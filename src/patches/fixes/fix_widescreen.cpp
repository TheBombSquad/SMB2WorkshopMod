#include "fix_widescreen.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace fix_widescreen {

TICKABLE_DEFINITION((
        .name = "fix-widescreen",
        .description = "Widescreen fixes",
        .tick = tick, ))

// The SEL_NGC check fixes less being visible vertically when playing in widescreen.
// It only activates when we're not in menus as the calibration menu's visuals break otherwise.
// The MD_GAME check fixes the View Stage aspect ratio when in widescreen.

void tick() {
    if (mkb::sub_mode == mkb::SMD_SEL_NGC_MAIN) {
        patch::write_word(reinterpret_cast<void*>(0x80287cf8), 0x418200a8); // original instruction
    }
    else {
        patch::write_nop(reinterpret_cast<void*>(0x80287cf8)); // nops a branch to the FOV-modifying code
    }
    if (mkb::main_mode == mkb::MD_GAME) {
        if (mkb::widescreen_mode == 0) {
            mkb::view_stage_aspect_ratio = 1.333333333f;
        }
        else {
            mkb::view_stage_aspect_ratio = 1.777777777f;
        }
    }
}

}// namespace fix_widescreen