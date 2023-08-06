#pragma once
#include <mkb.h>
#include <mkb.h>

namespace relpatches
{

static constexpr u16 STAGE_COUNT = 421;
extern u16 WORLD_COUNT;

struct Tickable {
    char* name = nullptr;                       // Name of the patch, what the config parser checks for
    char* message = nullptr;                    // Message to be output to stdout by the config parser on load. Include %s for 'enabled/disabled'
    int status = false;                         // The status/value of the patch, generally whether or not it is enabled
    int default_value = false;                  // The default status/value of the patch, to determine whether or not the patch should be run if the passed status value differs
    int minimum_value = 0;                      // The minimum status/value of the patch
    int maximum_value = 1;                      // The maximum status/value of the patch
    void(*main_loop_init_func)() = nullptr;     // Initialization function on load of mkb2.main_loop.rel
    void(*main_game_init_func)() = nullptr;     // Initialization function on load of mkb2.main_game.rel
    void(*sel_ngc_init_func)() = nullptr;       // Initialization function on load of mkb2.sel_ngc.rel
    void(*disp_func)() = nullptr;               // Display function (debug window stuff)
    void(*tick_func)() = nullptr;               // Tick function
};

extern const unsigned int PATCH_COUNT;
extern Tickable patches[];

namespace perfect_bonus {
void init();

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
float get_distance(Vec& vec1, Vec& vec2);

}

namespace music_id_per_stage {
void init_main_loop();

}

namespace theme_id_per_stage {
void init_main_loop();

}

namespace skip_intro_movie {
void init_main_loop();
void smd_adv_first_logo_tick_patch();

}

namespace smb1_camera_toggle {
void init_main_loop();
void tick();

}

namespace fix_missing_w {
void init_main_game();
}

namespace skip_cutscenes {
void init_main_game();
void dmd_scen_newgame_main_patch();
void dmd_scen_sceneplay_init_patch();
void dmd_scen_sel_floor_init_patch();
}

namespace remove_playpoints {
void init_main_game();
void tick();
}

namespace fix_storm_continue_platform {
void init_main_loop();
}

namespace fix_any_percent_crash {
void init_main_loop();
void tick();
}

namespace party_game_toggle {
u32 number_of_unlocked_party_games(u32 i);
u32 determine_party_game_unlock_status(int id);
void sel_ngc_init();
extern u16 party_game_bitflag;
}


namespace enable_menu_reflections {
void rendefc_handler(u32 stage_id);
void init_main_loop();
}

namespace custom_world_count {
void init_main_game();
void init_sel_ngc();
void dmd_scen_sceneplay_init_patch();
}

namespace stobj_draw_fix {
void init_main_loop();
}

namespace stage_author_names {
void init_main_loop();
void sprite_init(float x, float y);
void sprite_tick();
void sprite_disp();
}

}
