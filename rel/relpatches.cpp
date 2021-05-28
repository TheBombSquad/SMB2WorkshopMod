#include "relpatches.h"
#include "patch.h"
#include "mkb/mkb.h"
#include "mkb/camera.h"
#include "mkb/mode.h"
#include "assembly.h"
#include "pad.h"
#include <cstring>
#include <cstdio>

#define activeidx mkb::balls[0]
namespace relpatches
{

    // If the stage is a bonus stage (ball mode 0x40) and no bananas remain,
    // end the stage with a perfect (|= 0x228)
	void relpatches::perfect_bonus::tick() {
        if (mkb::ball_mode & 0x40 && mkb::bananas_remaining == 0) {
            mkb::ball_mode |= 0x228;
		}
	}

	// TODO: Probably not the best way to implement this, will need to look into a
	// proper fix soon. In a function that sets a parameter that enables heat
	// haze for the specific desert theme ID, the theme ID is compared to 0xffff
	// instead of 0x7.
	// 0x2c00ffff = cmpwi r0, 0xffff
	void remove_desert_haze::init()
	{
		patch::write_word(reinterpret_cast<void*>(0x802e4ed8), 0x2c00ffff);
	}

    // Hooks right before the call to SoftStreamStart, then nops the
    // "Stage Select" music fadeout
    // Modifies the 1st parameter to SoftStreamStart following the goal sequence
    // affecting whether or not the music restarts/changes. Only modifies this when
    // the submode indicates we're currently on a stage, or if we're on the 'Retry' screen.
	void story_continuous_music::init()
	{
	    patch::write_branch_bl(reinterpret_cast<void*>(0x802a5c34), reinterpret_cast<void*>(main::story_mode_music_hook));
        patch::write_nop(reinterpret_cast<void*>(0x80273aa0));
    }

    // Nop a call to a function that decreases in-game volume on pause
    void no_music_vol_decrease_on_pause::init()
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
        if (mkb::sub_mode == mkb::SubMode::SMD_GAME_TIMEOVER_INIT) {
            mkb::g_SoftStreamStart_with_some_defaults_2(0x2c);
        }
    }

    // Always return 'false' for a specific function that checks if the stage ID
    // is 348 when determining whether or not to handle level loading specially
    // 0x38600000 = li r3, 0x0
    void fix_revolution_slot::init()
    {
        patch::write_word(reinterpret_cast<void*>(0x802ca9fc), 0x38600000);
    }

    // Always return 'true' for a specific function that checks if the stage ID
    // belongs to a slot normally used for party games.
    // 0x38000001 = li r0, 0x1
    void fix_wormhole_surfaces::init()
    {
        patch::write_word(reinterpret_cast<void*>(0x802c8ce4), 0x38000001);
    }

    // Always compare the stage ID to 0xFFFF when these camera functions check
    // if the current stage ID is 0x15a when determining specific constants.
    // 0x2c00ffff = cmpwi r0. 0xFFFF
    void fix_labyrinth_camera::init()
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
            memset(death_count, 0, sizeof(death_count));

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
                sprite->width = 0.3;
            }
            else if (display > 999) {
                sprite->width = 0.4;
            }
            else if (display > 99) {
                sprite->width = 0.5;
            }
            else if (display > 9) {
                sprite->width = 0.6;
            }
            else {
                sprite->width = 1;
            }

            sprintf(sprite->text, "%u", display);
        }
    }

    // Nops the sub_mode_frame_counter decrement in smd_adv_title_tick.
    // This ensures the tutorial sequence will never start.
    void disable_tutorial::init()
    {
        patch::write_nop(reinterpret_cast<void*>(0x8027bbb0));
    }

    // Disables the check that prevents stage objects from being reflected,
    // then runs an additional check against the stage object's model flags to
    // determine if the proper flag was set. TODO: Maybe make this more elegant?
    // 0x38000000 = li r0, 0
    void fix_stobj_reflection::init_main_loop()
    {
        patch::write_word(reinterpret_cast<void*>(0x802ca480), 0x38000000);
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
    void music_id_per_stage::init()
    {
        patch::write_branch_bl(reinterpret_cast<void*>(0x802a5f08), reinterpret_cast<void*>(main::get_bgm_id_hook));
    }

    // Hooks into two functions that set the global world_theme variable
    // Not entirely sure what the second one is for, but it may be used for SMB1 themes
    void theme_id_per_stage::init()
    {
       patch::write_branch(reinterpret_cast<void*>(0x802c7c3c), reinterpret_cast<void*>(main::get_theme_id_hook_1));
       patch::write_branch(reinterpret_cast<void*>(0x802c7cc8), reinterpret_cast<void*>(main::get_theme_id_hook_2));
    }

    namespace extend_reflections {
        float nearest_dist_to_mir, distance_to_mirror;
        Vec3f current_ball_position, mirror_origin, ig_init_pos, translation_factor;
        mkb::Itemgroup* active_ig;

        // Hooks into the reflection-handling function, calling our function instead
        void init()
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
                    for (u32 col_hdr_idx = 0; col_hdr_idx < (mkb::stagedef->collision_header_count); col_hdr_idx++) {
                        mkb::StagedefCollisionHeader* hdr = &mkb::stagedef->collision_header_list[col_hdr_idx];
                        for (u32 refl_idx = 0; refl_idx < hdr->reflective_stage_model_count; refl_idx++) {
                            mkb::StagedefReflectiveStageModel* refl = &hdr->reflective_stage_model_list[refl_idx];
                            current_ball_position = ball->pos;
                            distance_to_mirror = get_distance(current_ball_position, refl->gma_ptr->origin);

                            if (nearest_dist_to_mir == -1.0 || distance_to_mirror < nearest_dist_to_mir) {
                                nearest_dist_to_mir = distance_to_mirror;
                                active_ig = &mkb::itemgroups[col_hdr_idx];
                                mirror_origin = refl->gma_ptr->origin;
                                ig_init_pos = hdr->origin;
                            }
                        }
                    }
                    // Translates the mirror plane according to the active animation and rotates it as well
                    if (active_ig != nullptr) {
                        mkb::mtxa_from_identity();
                        translation_factor.x = active_ig->pos.x - ig_init_pos.x;
                        translation_factor.y = active_ig->pos.y - ig_init_pos.y;
                        translation_factor.z = active_ig->pos.z - ig_init_pos.z;

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

    static char* AIAI[] = {&mkb::CHAR_A, &mkb::CHAR_I, &mkb::CHAR_A, &mkb::CHAR_I, &mkb::CHAR_SPACE, &mkb::CHAR_SPACE};
    static char* MEEMEE[] = {&mkb::CHAR_M, &mkb::CHAR_E, &mkb::CHAR_E, &mkb::CHAR_M, &mkb::CHAR_E, &mkb::CHAR_E};
    static char* BABY[] = {&mkb::CHAR_B, &mkb::CHAR_A, &mkb::CHAR_B, &mkb::CHAR_Y, &mkb::CHAR_SPACE, &mkb::CHAR_SPACE};
    static char* GONGON[] = {&mkb::CHAR_G, &mkb::CHAR_O, &mkb::CHAR_N, &mkb::CHAR_G, &mkb::CHAR_O, &mkb::CHAR_N};
    static char* HIHI[] = {&mkb::CHAR_H, &mkb::CHAR_I, &mkb::CHAR_H, &mkb::CHAR_I, &mkb::CHAR_SPACE, &mkb::CHAR_SPACE};
    static char** monkey_name_lookup[] = {AIAI, MEEMEE, BABY, GONGON, HIHI};

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
            mkb::story_file_name[i] = monkey_name_lookup[mkb::active_monkey_id][i];
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
                if (pad::button_pressed(gc::PAD_BUTTON_A)) {
                    mkb::mode_cnt = 1;
                    *(&mkb::mode_cnt+2) = 7;
                }
                else if (pad::button_pressed(gc::PAD_BUTTON_B)){
                    if (mkb::g_character_selected) return;
                    mkb::mode_cnt = 2;
                }
            }
        }
    }

    }
    // Hooks into the smd_adv_first_logo_tick function, calling our own tick function
    void skip_intro_movie::init() {
        patch::write_branch_bl(reinterpret_cast<void*>(0x8027ec6c), reinterpret_cast<void*>(tick));
    }

    // Immediately goes to the title screen
    void skip_intro_movie::tick() {
        mkb::sub_mode_request = mkb::SMD_ADV_MOVIE_RETURN;
    }

    namespace smb1_camera_toggle {
        static bool smb1_cam_toggled;

        void init() {
            smb1_cam_toggled = false;
        }

        // The write_word statement moves the camera's angle down by 2.8 degrees to match SMB1's angle
        // Everything else brings the camera position/pivot values in-line with SMB1's values
        // Camera mode 1 enables SMB1-like vertical camera tracking, camera mode 0x4c is SMB2's default
        void tick() {
            if (mkb::main_mode == mkb::MD_GAME && (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN)) {
                if (smb1_cam_toggled) {
                    if (mkb::cameras[0].mode == 0x4c) mkb::cameras[0].mode = 1;
                    patch::write_word(reinterpret_cast<void*>(0x802886c8), 0x38000400);
                    mkb::g_camera_turn_rate_scale = 0.6875;
                    mkb::camera_pivot_height = -0.5;
                    mkb::camera_height = 1;
                }
                else {
                    if (mkb::cameras[0].mode == 0x1) mkb::cameras[0].mode = 0x4c;
                    patch::write_word(reinterpret_cast<void*>(0x802886c8), 0x38000200);
                    mkb::g_camera_turn_rate_scale = 0.75;
                    mkb::camera_pivot_height = 0.18;
                    mkb::camera_height = 0.8;
                }
            }

            if (pad::button_pressed(gc::PAD_TRIGGER_Z)) {
                smb1_cam_toggled = !smb1_cam_toggled;
            }

        }
    }

    // Defines the missing lowercase 'w' character and opints the game's duplicate 't' to the new 'w'.
    namespace fix_missing_w {
        static char CHAR_w[4] = {'w', '\0', '\0', '\0'};

        void init_main_game() {
        mkb::nameentry_character_ptr_list[62] = &(CHAR_w[0]);
        gc::OSReport("patched %x to %x with %x\n", &mkb::nameentry_character_ptr_list, &(mkb::nameentry_character_ptr_list[46]), &(CHAR_w[0]));
        }
    }
}
