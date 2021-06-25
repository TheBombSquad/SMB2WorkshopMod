#pragma once
#include <vector>

namespace config {
extern std::vector<void(*)()> main_loop_init_funcs;
extern std::vector<void(*)()> main_game_init_funcs;
extern std::vector<void(*)()> sel_ngc_init_funcs;
extern std::vector<void(*)()> disp_funcs;
extern std::vector<void(*)()> tick_funcs;

extern bool tetris_enabled;
}
