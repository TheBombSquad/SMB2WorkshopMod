#pragma once

#include "mkb/mkb.h"

namespace menu_option_toggle {

void init_sel_ngc();
void tick();
extern u16 mode_bitflag;
extern u16 main_game_bitflag;
extern u16 level_bitflag;
extern u16 party_game_bitflag;

}// namespace menu_option_toggle