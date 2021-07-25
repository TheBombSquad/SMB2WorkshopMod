#pragma once
#include <mkb.h>
#include "relpatches.h"

namespace config {

u16* parse_stageid_list(char* buf, u16* array);
void parse_function_toggles(char* buf);
void parse_config();
void init_apesphere_tickables();

extern const unsigned int APESPHERE_TICKABLE_COUNT;
extern relpatches::Tickable apesphere_tickables[];

extern bool tetris_enabled;
extern bool apesphere_toggle_enabled;

}
