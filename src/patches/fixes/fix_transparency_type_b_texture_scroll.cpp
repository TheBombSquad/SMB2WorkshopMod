#include "fix_transparency_type_b_texture_scroll.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace fix_transparency_type_b_texture_scroll {

TICKABLE_DEFINITION((
        .name = "fix-transparency-type-b-texture-scroll",
        .description = "Transparency Type B texture scroll fix",
        .init_main_loop = init_main_loop, ))

// Always return 'true' for a specific instruction that checks if the texture should
// scroll or not on transparency type B stage models
void init_main_loop() {
    patch::write_word(reinterpret_cast<void*>(0x802c94f8), PPC_INSTR_LI(PPC_R3, 0x1));
}

}// namespace fix_transparency_type_b_texture_scroll
