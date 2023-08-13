#include "stobj_draw_fix.h"

#include "internal/patch.h"
#include "mkb/mkb.h"
#include "patches/tickable.h"

namespace stobj_draw_fix {
/*
 * Stobjs (goaltapes, party balls, bumpers, etc) placed on itemgroups with index greater than 127 may get an
 * incorrect itemgroup transform, making them appear weirdly or not at all. This patch forces stobj itemgroup
 * indices to be treated as unsigned bytes instead, increasing the greatest itemgroup idx a stobj may reside on
 * to 255.
 */

TICKABLE_DEFINITION((
        .name = "stobj-draw-fix",
        .description = "[wsmod] Stobj draw fix patch %s\n",
        .init_main_loop = init_main_loop, ))

// These are Ghidra addresses...
static const u16 lbz_addrs_lo[] = {
    0x0fb8,
    0x0fb8,
    0x0fcc,
    0x1aa0,
    0x1aa0,
    0x1abc,
    0x1abc,
    0x1ce4,
    0x1ce4,
    0x1d08,
    0x1d08,
    0x1d34,
    0x1d34,
    0x3cf0,
    0x4ea4,
    0x4ecc,
    0x4f64,
    0x6500,
    0x6570,
    0x6a64,
    0x7208,
    0x7c5c,
    0x7c98,
    0x7d34,
};

void init_main_loop() {
    for (u32 addr: lbz_addrs_lo) {
        u32 ram_addr = addr + 0x80240000 - 0x80199fa0 + 0x802701d8;
        // Nop `extsb` instr following lbz to prevent sign extension
        patch::write_nop(reinterpret_cast<void*>(ram_addr + 4));
    }
}

}// namespace stobj_draw_fix
