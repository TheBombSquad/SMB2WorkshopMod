#include "story_continuous_music.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "internal/tickable.h"

namespace story_continuous_music {

TICKABLE_DEFINITION((
        .name = "story-mode-music-fix",
        .description = "Continuous story mode music",
        .init_main_loop = init_main_loop, ))

// Hooks right before the call to SoftStreamStart, then nops the
// "Stage Select" music fadeout
// Modifies the 1st parameter to SoftStreamStart following the goal sequence
// affecting whether or not the music restarts/changes. Only modifies this when
// the submode indicates we're currently on a stage, or if we're on the 'Retry' screen.
void init_main_loop() {
    patch::write_branch_bl(reinterpret_cast<void*>(0x802a5c34),
                           reinterpret_cast<void*>(main::story_mode_music_hook));
    patch::write_nop(reinterpret_cast<void*>(0x80273aa0));
}

}// namespace story_continuous_music
