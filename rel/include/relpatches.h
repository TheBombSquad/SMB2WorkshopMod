#pragma once
#include "gc/gc.h"
#include "mkb/mkb.h"

namespace relpatches
{

static constexpr u16 STAGE_COUNT = 421;

namespace perfect_bonus {
void init();
void tick();
}

namespace remove_desert_haze {
void init();
void tick();
}

namespace story_continuous_music {
void init();
void tick();
}

namespace no_music_vol_decrease_on_pause {
void init();
void tick();
}

namespace story_mode_char_select {
void init_main_loop();
void init_main_game();
void tick();
void set_nameentry_filename();
}

namespace no_hurry_up_music {
void init_main_game();
void tick();
}

namespace fix_revolution_slot {
void init();
void tick();
}

namespace fix_labyrinth_camera {
void init();
void tick();
}

namespace fix_wormhole_surfaces {
void init();
void tick();
}

namespace challenge_death_count {
void init_main_game();
void tick();
u32 update_death_count();
void death_counter_sprite_tick(u8 *status, mkb::Sprite *sprite);
}

namespace disable_tutorial {
void init();
void tick();
}


namespace fix_stobj_reflection {
void init_main_loop();
void init_main_game();
}

namespace extend_reflections {
void init();
void tick();
void mirror_tick();
float get_distance(Vec3f& vec1, Vec3f& vec2);
}

namespace music_id_per_stage {
void init();
}

namespace theme_id_per_stage {
void init();
}

namespace skip_intro_movie {
void init();
void tick();
}

namespace smb1_camera_toggle {
void init();
void tick();
}

namespace fix_missing_w {
void init_main_game();
void tick();
}

namespace party_game_toggle {
void init();
void sel_ngc_init();
void tick();
}

namespace skip_cutscenes {
void init_main_game();
void tick();
void tick_next();
void update_state();
}

}
