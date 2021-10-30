#pragma once
#include "mkb.h"

namespace scratch
{

void init();
void tick();
void disp();
void sprite_tick(u8* status, mkb::Sprite* sprite);
}
