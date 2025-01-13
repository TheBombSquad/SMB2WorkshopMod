#include "fix_goal_types.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace fix_goal_types {

TICKABLE_DEFINITION((
        .name = "fix-goal-types",
        .description = "Goal type fix",
        .init_main_loop = init_main_loop, ))

// In functions responsible for calculating goal score and
// type, check for goal IDs up to 255 instead of up to 3,
// preventing score and warp issues on stages with loads of
// goals
void init_main_loop() {
    patch::write_word(reinterpret_cast<void*>(0x802d81a8), 0x2c0800ff); // cmpwi r8, 255
    patch::write_word(reinterpret_cast<void*>(0x8031373c), 0x2c0500ff); // cmpwi r5, 255
}

}// namespace fix_goal_types
