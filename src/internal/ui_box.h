#pragma once

#include "etl/delegate.h"
#include "etl/list.h"
#include "etl/memory.h"
#include "internal/log.h"
#include "mkb/mkb.h"

namespace ui {

void init();
void disp_all();

void draw_ui_box_ext(u32 texture_id);

float lerp(float f1, float f2, float t);
}// namespace ui
