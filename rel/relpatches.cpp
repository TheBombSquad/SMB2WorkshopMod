#include "relpatches.h"
#include "patch.h"
#include <mkb.h>
#include <mkb.h>
#include <mkb.h>
#include "assembly.h"
#include "ppcutil.h"
#include "pad.h"
#include <cstring>
#include <cstdio>

namespace relpatches
{

    Tickable patches[] = {
        {
            .name = "challenge-mode-death-count",
            .message = "[mod]  Challenge mode death count %s\n",
            .main_game_init_func = challenge_death_count::init_main_game,
        },

        {
            .name = "disable-how-to-play-screen",
            .message = "[mod]  Tutorial sequence removal %s\n",
            .main_loop_init_func = disable_tutorial::init_main_loop,
        },

        {
            .name = "enhance-reflective-surfaces",
            .message = "[mod]  Reflective surface enhancements %s\n",
            .main_loop_init_func = extend_reflections::init_main_loop,
        },

        {
            .name = "fix-labyrinth-camera",
            .message = "[mod]  Labyrinth stage slot fix %s\n",
            .main_loop_init_func = fix_labyrinth_camera::init_main_loop,
        },
        {
            .name = "fix-missing-w",
            .message = "[mod]  Missing 'w'  patch %s\n",
            .main_game_init_func = fix_missing_w::init_main_game,
        },
        {
            .name = "fix-revolution-slot",
            .message = "[mod]  Revolution stage slot fix %s\n",
            .main_loop_init_func = fix_revolution_slot::init_main_loop,
        },
        {
            .name = "fix-stage-object-reflection",
            .message = "[mod]  Stobj reflection flag support %s\n",
            .main_loop_init_func = fix_stobj_reflection::init_main_loop,
            .main_game_init_func = fix_stobj_reflection::init_main_game,
        },
        {
            .name = "fix-wormhole-surfaces",
            .message = "[mod]  Party game stage slot fix %s\n",
            .main_loop_init_func = fix_wormhole_surfaces::init_main_loop,
        },
        {
            .name = "custom-music-id",
            .message = "[mod]  Custom music ID patch %s\n",
            .main_loop_init_func = music_id_per_stage::init_main_loop,
        },
        {
            .name = "no-hurry-up-music",
            .message = "[mod]  Hurry up music removal %s\n",
            .main_game_init_func = no_hurry_up_music::init_main_game,
            .tick_func = no_hurry_up_music::tick,
        },
        {
            .name = "no-music-vol-decrease-on-pause",
            .message = "[mod]  No music volume decrease on pause %s\n",
            .main_loop_init_func = no_music_vol_decrease_on_pause::init_main_loop,
        },
        {
            .name = "perfect-bonus-completion",
            .message = "[mod]  Perfect bonus completion %s\n",
            .tick_func = perfect_bonus::tick,
        },
        {
            .name = "remove-desert-haze",
            .message = "[mod]  Desert haze removal %s\n",
            .main_loop_init_func = remove_desert_haze::init_main_loop,
        },
        {
            .name = "skip-intro-movie",
            .message = "[mod]  Skip intro movie patch %s\n",
            .main_loop_init_func = skip_intro_movie::init_main_loop,
            .tick_func = skip_intro_movie::tick,
        },
        {
            .name = "smb1-camera-toggle",
            .message = "[mod]  SMB1 camera toggle %s\n",
            .main_loop_init_func = smb1_camera_toggle::init_main_loop,
            .tick_func = smb1_camera_toggle::tick,
        },
        {
            .name = "story-mode-music-fix",
            .message = "[mod]  Continuous story mode music %s\n",
            .main_loop_init_func = story_continuous_music::init_main_loop,
        },
        {
            .name = "story-mode-char-select",
            .message = "[mod]  Story mode character select %s\n",
            .main_loop_init_func = story_mode_char_select::init_main_loop,
            .main_game_init_func = story_mode_char_select::init_main_game,
            .tick_func = story_mode_char_select::tick,
        },
        {
            .name = "custom-theme-id",
            .message = "[mod]  Custom theme ID patch %s\n",
            .main_loop_init_func = theme_id_per_stage::init_main_loop,
        },
        {
            .name = "skip-cutscenes",
            .message = "[mod]  Cutscene skip patch %s\n",
            .main_game_init_func = skip_cutscenes::init_main_game,
        },
        {
            .name = "remove-playpoints",
            .message = "[mod]  Playpoint removal patch %s\n",
            .main_game_init_func = remove_playpoints::init_main_game,
            .tick_func = remove_playpoints::tick,
        },
        {
            .name = "fix-storm-continue-platform",
            .message = "[mod]  Storm continue platform patch %s\n",
            .main_loop_init_func = fix_storm_continue_platform::init_main_loop,
        },
        {
            .name = "fix-any-percent-crash",
            .message = "[mod]  Story mode any-percent crash fix %s\n",
            .main_loop_init_func = fix_any_percent_crash::init_main_loop,
            .tick_func = fix_any_percent_crash::tick,
        },
        {
            .name = "party-game-toggle",
            .message = "[mod]  Party game toggle patch %s\n",
            .sel_ngc_init_func = party_game_toggle::sel_ngc_init,
        }
    };

    const unsigned int PATCH_COUNT = sizeof(patches) / sizeof(patches[0]);

    // If the stage is a bonus stage (ball mode 0x40) and no bananas remain,
    // end the stage with a perfect (|= 0x228)
    void relpatches::perfect_bonus::tick() {
        if (mkb::mode_info.ball_mode & 0x40 && mkb::mode_info.bananas_remaining == 0) {
            mkb::mode_info.ball_mode |= 0x228;
        }
    }

    // TODO: Probably not the best way to implement this, will need to look into a
    // proper fix soon. In a function that sets a parameter that enables heat
    // haze for the specific desert theme ID, the theme ID is compared to 0xffff
    // instead of 0x7.
    // 0x2c00ffff = cmpwi r0, 0xffff
    void remove_desert_haze::init_main_loop()
    {
        patch::write_word(reinterpret_cast<void*>(0x802e4ed8), 0x2c00ffff);
    }

    // Hooks right before the call to SoftStreamStart, then nops the
    // "Stage Select" music fadeout
    // Modifies the 1st parameter to SoftStreamStart following the goal sequence
    // affecting whether or not the music restarts/changes. Only modifies this when
    // the submode indicates we're currently on a stage, or if we're on the 'Retry' screen.
    void story_continuous_music::init_main_loop()
    {
        patch::write_branch_bl(reinterpret_cast<void*>(0x802a5c34), reinterpret_cast<void*>(main::story_mode_music_hook));
        patch::write_nop(reinterpret_cast<void*>(0x80273aa0));
    }

    // Nop a call to a function that decreases in-game volume on pause
    void no_music_vol_decrease_on_pause::init_main_loop()
    {
        patch::write_nop(reinterpret_cast<void*>(0x802a32a8));
    }

    // Nop out calls to start the hurry-up music. Call after main_game load
    void no_hurry_up_music::init_main_game()
    {
        patch::write_nop(reinterpret_cast<void*>(0x808f509c));
        patch::write_nop(reinterpret_cast<void*>(0x808f50a4));
    }

    // The init function breaks the "Time Over" sound, as it checks to see if the
    // hurry up music is playing. This re-imlements the playing of the sound.
    void no_hurry_up_music::tick()
    {
        if (mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT) {
            mkb::g_SoftStreamStart_with_some_defaults_2(0x2c);
        }
    }

    // Always return 'false' for a specific function that checks if the stage ID
    // is 348 when determining whether or not to handle level loading specially
    void fix_revolution_slot::init_main_loop()
    {
        patch::write_word(reinterpret_cast<void*>(0x802ca9fc), PPC_INSTR_LI(PPC_R3, 0x0));
    }

    // Always return 'true' for a specific function that checks if the stage ID
    // belongs to a slot normally used for party games.
    void fix_wormhole_surfaces::init_main_loop()
    {
        patch::write_word(reinterpret_cast<void*>(0x802c8ce4), PPC_INSTR_LI(PPC_R0, 0x1));
    }

    // Always compare the stage ID to 0xFFFF when these camera functions check
    // if the current stage ID is 0x15a when determining specific constants.
    // 0x2c00ffff = cmpwi r0. 0xFFFF
    void fix_labyrinth_camera::init_main_loop()
    {
        patch::write_word(reinterpret_cast<void*>(0x802858D4), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x802874BC), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x8028751C), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x802880EC), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x802881D4), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x802883B4), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x802886B8), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x8028BF44), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x8028C1CC), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x8028C650), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x8028CA84), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x80291338), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x80291420), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x80291664), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x80291904), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x80291930), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x80291960), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x8029198C), 0x2c00ffff);
        patch::write_word(reinterpret_cast<void*>(0x80291AEC), 0x2c00ffff);
    }

    namespace challenge_death_count {
        static u32 death_count[4];
        // Clears the per-player death counter, then nops the instruction that
        // decrements the life counter on player death. Then, hooks into the
        // decrement life counter function, and runs the update_death_count func.
        // Then, hooks into the monkey counter sprite tick function, and calls
        // the death counter sprite tick function instead.'
        void init_main_game()
        {
            mkb::memset(death_count, 0, sizeof(death_count));

            patch::write_nop(reinterpret_cast<void*>(0x808fa4f4));

            patch::write_branch_bl(reinterpret_cast<void*>(0x808fa560), reinterpret_cast<void*>(update_death_count));
            patch::write_branch(reinterpret_cast<void*>(mkb::sprite_monkey_counter_tick), reinterpret_cast<void*>(death_counter_sprite_tick));

        }

        // Increments the death counter of the current player on death.
        // This hooks into the life counter decrement function, so it is
        // only called in situations when the player would actually lose a life.
        // Returns 99, since the result of this function, normally, is the number
        // of lives of the player, which determines whether or not
        // the game should proceed to the continue/game over screen or not.
        u32 update_death_count()
        {
            mkb::Ball *ball = mkb::balls;
            for (int idx = 0; idx < 4; idx++) {
                if (ball->status == mkb::STAT_NORMAL) {
                    death_count[idx]++;
                }
                ball++;
            }
            return 99;
        }

        // Assigns our own parameters to the life counter sprite for the purposes
        // of counting deaths. Green when no deaths have occured, scales the sprite
        // so numbers don't go off-screen or get obstructed.
        void death_counter_sprite_tick(u8 *status, mkb::Sprite *sprite)
        {
            u32 display = death_count[mkb::curr_player_idx];

            if (display == 0) {
                sprite->blue = 0;
            }
            else {
                sprite->blue = 0xff;
            }

            if (display > 9999) {
                sprite->g_width = 0.3;
            }
            else if (display > 999) {
                sprite->g_width = 0.4;
            }
            else if (display > 99) {
                sprite->g_width = 0.5;
            }
            else if (display > 9) {
                sprite->g_width = 0.6;
            }
            else {
                sprite->g_width = 1;
            }

            mkb::sprintf(sprite->text, "%u", display);
        }
    }

    // Nops the sub_mode_frame_counter decrement in smd_adv_title_tick.
    // This ensures the tutorial sequence will never start.
    void disable_tutorial::init_main_loop()
    {
        patch::write_nop(reinterpret_cast<void*>(0x8027bbb0));
    }

    // Disables the check that prevents stage objects from being reflected,
    // then runs an additional check against the stage object's model flags to
    // determine if the proper flag was set. TODO: Maybe make this more elegant?
    // 0x38000000 = li r0, 0
    void fix_stobj_reflection::init_main_loop()
    {
        patch::write_word(reinterpret_cast<void*>(0x802ca480), PPC_INSTR_LI(PPC_R0, 0x0));
        patch::write_branch_bl(reinterpret_cast<void*>(0x802c9434), reinterpret_cast<void*>(main::reflection_draw_stage_hook));
    }

    // Checks the stage object's model flag to determine if the proper flag is set
    // during the 'view stage' sequence.
    void fix_stobj_reflection::init_main_game()
    {
        patch::write_branch_bl(reinterpret_cast<void*>(0x80913F34), reinterpret_cast<void*>(main::reflection_view_stage_hook));
    }

    // Hooks into g_handle_world_bgm, modifies the variable for BGM ID to point to
    // the one in our stage ID ->
    void music_id_per_stage::init_main_loop()
    {
        patch::write_branch_bl(reinterpret_cast<void*>(0x802a5f08), reinterpret_cast<void*>(main::get_bgm_id_hook));
    }

    // Hooks into two functions that set the global world_theme variable
    // Not entirely sure what the second one is for, but it may be used for SMB1 themes
    void theme_id_per_stage::init_main_loop()
    {
        patch::write_branch(reinterpret_cast<void*>(0x802c7c3c), reinterpret_cast<void*>(main::get_theme_id_hook_1));
        patch::write_branch(reinterpret_cast<void*>(0x802c7cc8), reinterpret_cast<void*>(main::get_theme_id_hook_2));
    }

    namespace extend_reflections {
        float nearest_dist_to_mir, distance_to_mirror;
        Vec3f current_ball_position, mirror_origin, ig_init_pos, translation_factor;
        mkb::Itemgroup* active_ig;

        // Hooks into the reflection-handling function, calling our function instead
        void init_main_loop()
        {
            patch::write_branch_bl(reinterpret_cast<void*>(0x8034b270), reinterpret_cast<void*>(mirror_tick));
            patch::write_nop(reinterpret_cast<void*>(0x8034b11c));
            nearest_dist_to_mir = -1.0;
            distance_to_mirror = 0.0;
            active_ig = nullptr;
        }

        // Translates the nearest mirror from its origin to the current translation/rotation of its collision header
        void mirror_tick()
        {
            mkb::Ball *ball = mkb::balls;

            // Determines the nearest reflective surface to the active ball
            for (int idx = 0; idx < 4; idx++) {
                if (ball != nullptr && ball->status == mkb::STAT_NORMAL) {
                    for (u32 col_hdr_idx = 0; col_hdr_idx < (mkb::stagedef->coli_header_count); col_hdr_idx++) {
                        mkb::StagedefColiHeader* hdr = &mkb::stagedef->coli_header_list[col_hdr_idx];
                        for (u32 refl_idx = 0; refl_idx < hdr->reflective_stage_model_count; refl_idx++) {
                            mkb::StagedefReflectiveStageModel* refl = &hdr->reflective_stage_model_list[refl_idx];
                            current_ball_position = ball->pos;
                            distance_to_mirror = get_distance(current_ball_position, refl->g_model_header_ptr->origin);

                            if (nearest_dist_to_mir == -1.0 || distance_to_mirror < nearest_dist_to_mir) {
                                nearest_dist_to_mir = distance_to_mirror;
                                active_ig = &mkb::itemgroups[col_hdr_idx];
                                mirror_origin = refl->g_model_header_ptr->origin;
                                ig_init_pos = hdr->origin;
                            }
                        }
                    }
                    // Translates the mirror plane according to the active animation and rotates it as well
                    if (active_ig != nullptr) {
                        mkb::mtxa_from_identity();
                        translation_factor.x = active_ig->position.x - ig_init_pos.x;
                        translation_factor.y = active_ig->position.y - ig_init_pos.y;
                        translation_factor.z = active_ig->position.z - ig_init_pos.z;

                        mkb::mtxa_translate(&mirror_origin);
                        mkb::mtxa_translate(&translation_factor);
                        mkb::mtxa_rotate_x(active_ig->rotation.x);
                        mkb::mtxa_rotate_y(active_ig->rotation.y);
                        mkb::mtxa_rotate_z(active_ig->rotation.z);
                    }
                }
                ball++;
            }

            active_ig = nullptr;
            nearest_dist_to_mir = -1.0;
            return;
        }

        float get_distance(Vec3f& vec1, Vec3f& vec2)
        {
            float xcmp = (vec1.x-vec2.x)*(vec1.x-vec2.x);
            float ycmp = (vec1.y-vec2.y)*(vec1.y-vec2.y);
            float zcmp = (vec1.z-vec2.z)*(vec1.z-vec2.z);

            return mkb::math_sqrt(xcmp+ycmp+zcmp);
        }
    }


    namespace story_mode_char_select {

        static mkb::undefined4* AIAI[] = {&mkb::CHAR_A, &mkb::CHAR_I, &mkb::CHAR_A, &mkb::CHAR_I, &mkb::CHAR_SPACE, &mkb::CHAR_SPACE};
        static mkb::undefined4* MEEMEE[] = {&mkb::CHAR_M, &mkb::CHAR_E, &mkb::CHAR_E, &mkb::CHAR_M, &mkb::CHAR_E, &mkb::CHAR_E};
        static mkb::undefined4* BABY[] = {&mkb::CHAR_B, &mkb::CHAR_A, &mkb::CHAR_B, &mkb::CHAR_Y, &mkb::CHAR_SPACE, &mkb::CHAR_SPACE};
        static mkb::undefined4* GONGON[] = {&mkb::CHAR_G, &mkb::CHAR_O, &mkb::CHAR_N, &mkb::CHAR_G, &mkb::CHAR_O, &mkb::CHAR_N};
        static mkb::undefined4* HIHI[] = {&mkb::CHAR_H, &mkb::CHAR_I, &mkb::CHAR_H, &mkb::CHAR_I, &mkb::CHAR_SPACE, &mkb::CHAR_SPACE};
        static mkb::undefined4** monkey_name_lookup[] = {AIAI, MEEMEE, BABY, GONGON, HIHI};

        // Overrides the return value of certain functions to force the chosen monkey to be
        // preloaded in place of AiAi
        void init_main_loop()
        {
            patch::write_branch_bl(reinterpret_cast<void*>(0x803daffc), reinterpret_cast<void*>(main::get_monkey_id_hook));
        }

        // Sets the storymode filename to the name of the selected monkey, when no name is provided.
        void set_nameentry_filename()
        {
            for (int i = 0; i < 6; i++) {
                mkb::story_file_name[i] = reinterpret_cast<char*>(monkey_name_lookup[mkb::active_monkey_id][i]);
            }
            mkb::g_some_nameentry_length = 0x6;
        }

        // Overrides the return value of certain functions to force the chosen monkey to be
        // preloaded in place of AiAi.
        // Also calls the function to set the default filename to the name of the selected
        // monkey, rather than deafulting to 'AIAI'.
        void init_main_game()
        {
            patch::write_branch_bl(reinterpret_cast<void*>(0x808fcac4), reinterpret_cast<void*>(main::get_monkey_id_hook));
            patch::write_branch_bl(reinterpret_cast<void*>(0x808ff120), reinterpret_cast<void*>(main::get_monkey_id_hook));
            patch::write_branch_bl(reinterpret_cast<void*>(0x80908894), reinterpret_cast<void*>(main::get_monkey_id_hook));

            patch::write_branch_bl(reinterpret_cast<void*>(0x80906368), reinterpret_cast<void*>(set_nameentry_filename));
            patch::write_nop(reinterpret_cast<void*>(0x8090636c));
            patch::write_nop(reinterpret_cast<void*>(0x80906370));
            patch::write_nop(reinterpret_cast<void*>(0x80906374));
            patch::write_nop(reinterpret_cast<void*>(0x80906378));

        }

        // Assign the correct 'next screen' variables to redirect Story Mode to the
        // character select screen. Also handle input to prevent Story Mode from not
        // initializing if mode_cnt isn't set to 1.
        void tick()
        {
            if (mkb::sub_mode == mkb::SMD_SEL_NGC_MAIN) {
                patch::write_word(reinterpret_cast<void*>(0x80921a20), 0x6000000);
                patch::write_word(reinterpret_cast<void*>(0x80920ba0), 0xC000000);
                if (mkb::g_currently_visible_menu_screen == 0x6) {
                    if (pad::button_pressed(mkb::PAD_BUTTON_A)) {
                        mkb::menu_stack_ptr = 1;
                        *(&mkb::menu_stack_ptr+2) = 7;
                    }
                    else if (pad::button_pressed(mkb::PAD_BUTTON_B)){
                        if (mkb::g_character_selected) return;
                        mkb::menu_stack_ptr = 2;
                    }
                }
            }
        }

    }
    // Hooks into the smd_adv_first_logo_tick function, calling our own tick function
    void skip_intro_movie::init_main_loop() {
        patch::write_branch_bl(reinterpret_cast<void*>(0x8027ec6c), reinterpret_cast<void*>(tick));
    }

    // Immediately goes to the title screen
    void skip_intro_movie::tick() {
        mkb::sub_mode_request = mkb::SMD_ADV_MOVIE_RETURN;
    }

    namespace smb1_camera_toggle {
        static bool smb1_cam_toggled;

        void init_main_loop() {
            smb1_cam_toggled = false;
        }

        // The write_word statement moves the camera's angle down by 2.8 degrees to match SMB1's angle
        // Everything else brings the camera position/pivot values in-line with SMB1's values
        // Camera mode 1 enables SMB1-like vertical camera tracking, camera mode 0x4c is SMB2's default
        void tick() {
            if (mkb::main_mode == mkb::MD_GAME && (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN)) {
                if (smb1_cam_toggled) {
                    if (mkb::cameras[0].mode == 0x4c) mkb::cameras[0].mode = 1;
                    patch::write_word(reinterpret_cast<void*>(0x802886c8), PPC_INSTR_LI(PPC_R0, 0x400));
                    mkb::g_camera_turn_rate_scale = 0.6875;
                    mkb::camera_pivot_height = -0.5;
                    mkb::camera_height = 1;
                }
                else {
                    if (mkb::cameras[0].mode == 0x1) mkb::cameras[0].mode = 0x4c;
                    patch::write_word(reinterpret_cast<void*>(0x802886c8), PPC_INSTR_LI(PPC_R0, 0x200));
                    mkb::g_camera_turn_rate_scale = 0.75;
                    mkb::camera_pivot_height = 0.18;
                    mkb::camera_height = 0.8;
                }
            }

            if (pad::button_pressed(mkb::PAD_TRIGGER_Z)) {
                smb1_cam_toggled = !smb1_cam_toggled;
            }

        }
    }

    // Defines the missing lowercase 'w' character and opints the game's duplicate 't' to the new 'w'.
    namespace fix_missing_w {
        static char CHAR_w[4] = {'w', '\0', '\0', '\0'};

        void init_main_game() {
            mkb::nameentry_character_ptr_list[114] = reinterpret_cast<mkb::undefined4**>(&(CHAR_w[0]));
        }
    }

    // Skips cutscenes in story mode.
    // TODO: Maybe fade the screen out so the transition screen color is not based off the  world fog
    namespace skip_cutscenes {
        // Variable for keeping track of the 'state' of the current story mode game.
        // This basically represents the active world, except world 11 is the credits sequence,
        // world 12 is the name entry sequence, and world 13 is the game over sequence.
        static int active_state = 0;

        // Mutes all actively playing music track. This function is inlined a lot in the actual game code.
        void mute_all_music_tracks() {
            for (int i = 0; i < 10; i++) {
                if (mkb::g_active_music_tracks[i] != 0xffff) {
                    mkb::g_change_music_volume(mkb::g_active_music_tracks[i], 15, 0);
                }
            }
        }

        // Skips the first cutscene. Replaces the original dmd_scen_newgame_main function, which sets up cutscene stuff.
        void dmd_scen_newgame_main_patch() {
            mute_all_music_tracks();
            mkb::g_SoftStreamStart_with_some_defaults_2(0);
            mkb::g_storymode_next_world = 0;
            mkb::g_storymode_mode = mkb::DMD_SCEN_SEL_WORLD_INIT;
            return;
        }

        // Skips the rest of the cutscenes, and handles the credits, name entry, and game over sequences.
        // This is called when a cutscene is attempted to be loaded.
        void dmd_scen_sceneplay_init_patch() {
            mkb::dest_all_sprites();
            if (active_state > 10) {
                active_state++;
            }

            else {
                active_state = mkb::g_storymode_next_world;
            }

            mute_all_music_tracks();

            // If we're in 'world 11', initialize the credits sequence.
            if (active_state == 10) {
                mkb::mode_flags = mkb::mode_flags | 0x100000;
                patch::write_word(reinterpret_cast<void*>(0x8054dbdc), 0xffffffff);
                mkb::g_storymode_mode = mkb::DMD_SCEN_GAME_CLEAR_INIT;
            }

            // If we're in 'world 12', initialize the name entry sequence.
            // This requires some odd heap-clearing stuff to be handled that would otherwise be handled
            // when returning from the cutscene.
            else if (active_state == 11) {
                mkb::dest_all_events();

                reinterpret_cast<void(*)()>(mkb::g_something_freeing_heap_4)();
                reinterpret_cast<void(*)()>(mkb::g_something_freeing_heap_parent)();
                reinterpret_cast<void(*)(int)>(mkb::g_something_with_sound7_and_game_heaps)(0);

                mkb::OSSetCurrentHeap(mkb::chara_heap);

                mkb::mode_flags = mkb::mode_flags | 0x100000;
                patch::write_word(reinterpret_cast<void*>(0x8054dbdc), 0xffffffff);
                mkb::g_storymode_mode = mkb::DMD_SCEN_NAMEENTRY_INIT;
            }

            // If we're in 'world 13', initialize the game over sequence.
            else if (active_state == 12) {
                mkb::mode_flags = mkb::mode_flags | 0x100000;
                patch::write_word(reinterpret_cast<void*>(0x8054dbdc), 0xffffffff);
                mkb::g_storymode_mode = mkb::DMD_SCEN_GAME_OVER_INIT;
            }

            // Otherwise, we're on a valid world, so, instead of loading a cutscene, just go to the
            // next world's stage select screen.
            else {
                mkb::g_SoftStreamStart_with_some_defaults_2(0);
                mkb::g_storymode_next_world++;
                mkb::g_storymode_mode = mkb::DMD_SCEN_SEL_WORLD_NEXT;
            }

            return;
        }

        // Keeps active_state synced between file loads.
        void dmd_scen_sel_floor_init_patch() {
            active_state = mkb::curr_world;
            mkb::g_storymode_mode = mkb::DMD_SCEN_SEL_FLOOR_MAIN;

            // I have no idea what this does, it's something the game does in the original function
            u32 data = *reinterpret_cast<u32*>(0x8054dbc0);
            patch::write_word(reinterpret_cast<void*>(0x8054dbc0), data|2);
            mkb::dmd_scen_sel_floor_init_child();
        }

        // Handles preloading, to prevent cutscene files from attempting to be loaded.
        void handle_preloading() {
            if (mkb::main_mode != mkb::MD_GAME || mkb::main_game_mode != mkb::STORY_MODE) {
                // Preload files normally
                patch::write_word(reinterpret_cast<void*>(0x803db048), 0x9421ffd0);
            }
            else {
                // Do not preload files
                patch::write_blr(reinterpret_cast<void*>(0x803db048));
            }
        }

        void init_main_game() {
            patch::write_branch(reinterpret_cast<void*>(mkb::dmd_scen_newgame_main), reinterpret_cast<void*>(dmd_scen_newgame_main_patch));
            patch::write_branch(reinterpret_cast<void*>(mkb::dmd_scen_sceneplay_init), reinterpret_cast<void*>(dmd_scen_sceneplay_init_patch));
            patch::write_branch(reinterpret_cast<void*>(mkb::dmd_scen_sel_floor_init), reinterpret_cast<void*>(dmd_scen_sel_floor_init_patch));
            patch::write_branch(reinterpret_cast<void*>(mkb::g_preload_next_stage_files), reinterpret_cast<void*>(handle_preloading));
        }

    }

    // Removes the playpoint notification screens when exiting from story mode or challenge mode, or after a 'game over'.
    namespace remove_playpoints {
        void init_main_game() {
            // Removes playpoint screen when exiting challenge/story mode.
            patch::write_nop(reinterpret_cast<void*>(0x808f9ecc));
            patch::write_nop(reinterpret_cast<void*>(0x808f9eec));

            // Removes playpoint screen after the 'game over' sequence.
            patch::write_nop(reinterpret_cast<void*>(0x808f801c));
            patch::write_nop(reinterpret_cast<void*>(0x808f803c));
        }

        void tick() {
            mkb::unlock_info.party_games = 0x0001b600;
        }
    }

    // Fixes an issue with rain droplets not appearing correctly on the continue platform in the storm theme.
    namespace fix_storm_continue_platform {
        void init_main_loop() {
            patch::write_branch(reinterpret_cast<void*>(mkb::effect_bgstm_rainripple_disp), reinterpret_cast<void*>(main::fix_rain_ripple));
        }
    }

    // Fixes an issue where skipping more than a certain number of stages for story mode any% causes a crash.
    namespace fix_any_percent_crash {
        static bool chara_heap_cleared;
        static mkb::SpriteTex* texes[10] = {};
        static u8 active_sprite_idx = 0.;
        static void (*tex_load_trampoline)(mkb::SpriteTex *sprite_tex, char *file_path, u32 param_3, u16 width, u16 height, u32 format);

        // Keeps track all preview image sprite pointers as they are loaded. Only 10 are loaded for story mode.
        void init_main_loop() {
            tex_load_trampoline = patch::hook_function(mkb::g_load_preview_texture, [](mkb::SpriteTex *sprite_tex,char *file_path,u32 param_3,u16 width,u16 height,u32 format) {
                if (mkb::main_mode == mkb::MD_GAME || mkb::main_game_mode == mkb::STORY_MODE) {
                    if (active_sprite_idx > 9) {
                        for (int i = 0; i < 10; i++) {
                            texes[i] = nullptr;
                        }
                    active_sprite_idx = 0;
                    }

                texes[active_sprite_idx] = sprite_tex;
                active_sprite_idx++;
                }
            tex_load_trampoline(sprite_tex, file_path, param_3, width, height, format);
            });
        }

        // Frees the preview images to heap on the 'save data' screen.
        void tick() {
            if (mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_INIT) {
                chara_heap_cleared = false;
            }

            if (mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_MAIN && !chara_heap_cleared) {
                for (int i = 0; i < 10; i++) {
                    if (texes[i] != nullptr && texes[i]->tex_data != nullptr) {
                        mkb::OSFreeToHeap(mkb::chara_heap, texes[i]->tex_data);
                    }
                }

                chara_heap_cleared = true;
            }
        }

    }

    // Allows for party games to be toggled with a config option.
    namespace party_game_toggle {
        u16 party_game_bitflag = 0;

        // Variable-precision SWAR algorithm
        // Source: https://stackoverflow.com/a/109025
        u32 number_of_unlocked_party_games(u32 i)
        {
             i = i - ((i >> 1) & 0x55555555);        // add pairs of bits
             i = (i & 0x33333333) + ((i >> 2) & 0x33333333);  // quads
             i = (i + (i >> 4)) & 0x0F0F0F0F;        // groups of 8
             return (i * 0x01010101) >> 24;          // horizontal sum of bytes
        }

        // Party game IDs do not match their menu positions, hence this ugly mess
        u32 determine_party_game_unlock_status(int id) {
            switch (id) {
                case 0xa:
                    return (party_game_bitflag & 0x40);
                case 0x9:
                    return (party_game_bitflag & 0x80);
                case 0xd:
                    return (party_game_bitflag & 0x100);
                case 0xc:
                    return (party_game_bitflag & 0x200);
                case 0xf:
                    return (party_game_bitflag & 0x400);
                case 0x10:
                    return (party_game_bitflag & 0x800);
                default:
                    return 0;
            }

        }

        void sel_ngc_init() {
            patch::hook_function(mkb::g_check_if_partygame_unlocked, determine_party_game_unlock_status);
            patch::write_word(reinterpret_cast<void*>(0x808f9154), PPC_INSTR_LI(PPC_R0, (~party_game_bitflag & 0x3f)));

            mkb::strcpy(mkb::CANNOT_SELECT_PARTY_GAME_STRING, "You cannot play this game\n in this custom pack.");
            mkb::strcpy(mkb::CAN_PURCHASE_PARTY_GAME_STRING, "You cannot unlock this game\n in this custom pack.");
            mkb::sprintf(mkb::CAN_PLAY_NUM_PARTY_GAMES_STRING, "You can play /bcff8000/%d/bcffff00/ party games!", number_of_unlocked_party_games(party_game_bitflag));
        }
    }

}
