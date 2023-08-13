#include "custom_theme_id.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace custom_theme_id {

TICKABLE_DEFINITION((
        .name = "custom-theme-id",
        .description = "Custom theme ID patch",
        .init_main_loop = init_main_loop, ))

// Hooks into two functions that set the global world_theme variable
// Not entirely sure what the second one is for, but it may be used for SMB1 themes
void init_main_loop() {
    patch::write_branch(reinterpret_cast<void*>(0x802c7c3c), reinterpret_cast<void*>(main::get_theme_id_hook_1));
    patch::write_branch(reinterpret_cast<void*>(0x802c7cc8), reinterpret_cast<void*>(main::get_theme_id_hook_2));
}

}// namespace custom_theme_id
