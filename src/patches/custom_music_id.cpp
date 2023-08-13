#include "custom_music_id.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "patches/tickable.h"
#include "util/ppcutil.h"

namespace custom_music_id {

TICKABLE_DEFINITION((
        .name = "custom-music-id",
        .description = "[wsmod]  Custom music ID patch %s\n",
        .init_main_loop = init_main_loop, ))

// Hooks into g_handle_world_bgm, modifies the variable for BGM ID to point to
// the one in our stage ID ->
void init_main_loop() {
    patch::write_branch_bl(reinterpret_cast<void*>(0x802a5f08), reinterpret_cast<void*>(main::get_bgm_id_hook));
}

}// namespace custom_music_id
