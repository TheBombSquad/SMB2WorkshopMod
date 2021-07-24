#include <mkb.h>
#include "assembly.h"

namespace main
{

mkb::GXColor debug_text_color = {};
u32 currently_playing_iw = 0;
u16 bgm_id_lookup[relpatches::STAGE_COUNT] = {0};
u16 theme_id_lookup[relpatches::STAGE_COUNT] = {0};

}
