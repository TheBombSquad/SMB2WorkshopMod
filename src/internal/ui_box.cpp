#include "ui_box.h"

#include "etl/list.h"
#include "internal/heap.h"
#include "internal/log.h"
#include "mkb/mkb.h"
#include "utils/vecutil.h"

#define SIN(x) mkb::math_sin(x)
#define COS(x) SIN(16384 - x)

namespace ui {
void init() {
}


float lerp(float f1, float f2, float t) {
    return (1 - t) * f1 + t * f2;
}
}// namespace ui