#include "story_missing_w_fix.h"

#include "internal/assembly.h"
#include "internal/patch.h"
#include "internal/tickable.h"

// Defines the missing lowercase 'w' character and opints the game's duplicate 't' to the new 'w'.
namespace story_missing_w_fix {

TICKABLE_DEFINITION((
        .name = "story-fix-missing-w",
        .description = "Missing 'w' patch",
        .init_main_game = init_main_game, ))

static char CHAR_w[4] = {'w', '\0', '\0', '\0'};

void init_main_game() {
    mkb::nameentry_character_ptr_list[114] = reinterpret_cast<mkb::undefined4**>(&(CHAR_w[0]));
}

}// namespace story_missing_w_fix
