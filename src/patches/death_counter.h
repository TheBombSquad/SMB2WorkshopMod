#pragma once

#include "mkb/mkb.h"
#include "tickable.h"

namespace death_counter {
    void init_main_game();
    u32 update_death_count();
    void death_counter_sprite_tick(u8* status, mkb::Sprite* sprite);

    /*
     */
}