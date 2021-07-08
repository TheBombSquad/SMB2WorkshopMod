#pragma once
#include "gc/gc.h"
#include "mkb/mkb.h"

namespace relpatches
{

static constexpr u16 STAGE_COUNT = 421;

struct Tickable {
    const char* name = nullptr;
    const char* message = nullptr;
    bool enabled = false;
    void(*main_loop_init_func)() = nullptr;
    void(*main_game_init_func)() = nullptr;
    void(*sel_ngc_init_func)() = nullptr;
    void(*disp_func)() = nullptr;
    void(*tick_func)() = nullptr;
};

extern const unsigned int PATCH_COUNT;
extern Tickable patches[];

namespace perfect_bonus {
void tick();

}

namespace remove_desert_haze {
void init_main_loop();

}

namespace story_continuous_music {
void init_main_loop();

}

namespace no_music_vol_decrease_on_pause {
void init_main_loop();

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
void init_main_loop();

}

namespace fix_labyrinth_camera {
void init_main_loop();

}

namespace fix_wormhole_surfaces {
void init_main_loop();

}

namespace challenge_death_count {
void init_main_game();
u32 update_death_count();
void death_counter_sprite_tick(u8 *status, mkb::Sprite *sprite);

}

namespace disable_tutorial {
void init_main_loop();

}

namespace fix_stobj_reflection {
void init_main_loop();
void init_main_game();

}

namespace extend_reflections {
void init_main_loop();
void mirror_tick();
float get_distance(Vec3f& vec1, Vec3f& vec2);

}

namespace music_id_per_stage {
void init_main_loop();

}

namespace theme_id_per_stage {
void init_main_loop();

}

namespace skip_intro_movie {
void init_main_loop();
void tick();

}

namespace smb1_camera_toggle {
void init_main_loop();
void tick();

}

namespace fix_missing_w {
void init_main_game();
}


}
