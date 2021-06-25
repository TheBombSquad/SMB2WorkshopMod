#pragma once
#include "mkb/mkb.h"
#include <vector>

namespace config {

u16* parse_stageid_list(char* buf, u16* array);
void parse_function_toggles(char* buf);
void parse_config();

static std::vector<void(*)()> main_loop_init_funcs;
static std::vector<void(*)()> main_game_init_funcs;
static std::vector<void(*)()> sel_ngc_init_funcs;
static std::vector<void(*)()> disp_funcs;
static std::vector<void(*)()> tick_funcs;
static bool tetris_enabled = false;

}
