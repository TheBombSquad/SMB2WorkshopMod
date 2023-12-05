#pragma once

#include "../mkb/mkb.h"
#define LEN(array) (sizeof(array) / sizeof((array)[0]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, min, max) (MIN((max), MAX((min), (x))))