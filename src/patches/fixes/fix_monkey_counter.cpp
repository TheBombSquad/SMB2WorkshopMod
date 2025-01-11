#include "fix_monkey_counter.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace fix_monkey_counter {

TICKABLE_DEFINITION((
        .name = "fix-monkey-counter",
        .description = "Monkey counter sprite fix",
        .init_main_loop = init_main_loop, ))

// In the function responsible for displaying the monkey
// counter sprite, check for if monkey count is less than
// 255 instead of 4, effectively never executing leftover 
// sprite code from SMB1
void init_main_loop() {
    patch::write_word(reinterpret_cast<void*>(0x80332874), 0x2c0000ff); // cmpwi r0, 255
    patch::write_word(reinterpret_cast<void*>(0x80332f94), 0x2c0000ff); // cmpwi r0, 255
}

}// namespace fix_monkey_counter
