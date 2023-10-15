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
        patch::write_word(reinterpret_cast<void*>(0x80287cf8), 0x418200a8);
    }
    else {
        patch::write_nop(reinterpret_cast<void*>(0x80287cf8));
    }
    if (mkb::main_mode == mkb::MD_GAME) {
        if (mkb::widescreen_mode == 0) {
            patch::write_word(reinterpret_cast<void*>(0x809155fc), (0x3faaaaab));
        }
        else {
            patch::write_word(reinterpret_cast<void*>(0x809155fc), (0x3fe38e3b));
        }
    }
}

}// namespace fix_widescreen