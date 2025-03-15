#include "stage_name_subtitles.h"

#include "internal/heap.h"
#include "internal/log.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"


// Allows for subtitles to be displayed under the stage name.
namespace stage_name_subtitles {

TICKABLE_DEFINITION((
        .name = "stage-name-subtitles",
        .description = "Stage name subtitles",
        .init_main_loop = init_main_loop,
        .init_exoption = init_exoption, ))

static char subtitle_file_path[] = "/stgname/subtitles.str";
static char fallback_subtitle = '\0';
static char* subtitle_file_buf = nullptr;
static constexpr u16 STAGE_COUNT = 421;
static char subtitle_list[STAGE_COUNT][64];

void sprite_init(float x, float y) {

    char* subtitle;

    if (mkb::current_stage_id >= 0 && mkb::current_stage_id < STAGE_COUNT) {
        if (mkb::main_mode != mkb::MD_EXOPT) {
            subtitle = subtitle_list[mkb::current_stage_id];// Handle the subtitle to load in most cases
        }
        else {
            subtitle = subtitle_list[mkb::g_replay_stage_id_to_load];// Handle the subtitle to load when loading a replay
        }
    }
    else {
        subtitle = &fallback_subtitle;
    }

    mkb::Sprite* sprite = mkb::create_sprite();
    float offset = (mkb::curr_difficulty == mkb::DIFF_BEGINNER) ? 38.0 : 34.0;
    if (sprite != nullptr) {
        sprite->pos.x = x + offset + 4.0;
        sprite->pos.y = y + 30.0;
        sprite->font = mkb::FONT_ASC_24x24;
        sprite->fpara1 = 1.0;
        sprite->fpara2 = 0.0;
        sprite->alpha = 0.0;
        (sprite->mult_color).red = 0xff;
        (sprite->mult_color).green = 0xff;
        (sprite->mult_color).blue = 0x00;
        sprite->width = 0.6;
        sprite->height = 0.6;
        sprite->field21_0x20 = 1.0;
        sprite->field22_0x24 = 1.0;
        sprite->g_flags1 = sprite->g_flags1 | 0xa1000000;
        sprite->widescreen_translation_x = 0x60;
        mkb::sprintf(sprite->text, subtitle);
        sprite->tick_func = mkb::sprite_hud_stage_name_tick;
    }

    mkb::Sprite* sprite_shadow = mkb::create_sprite();
    if (sprite_shadow != nullptr) {
        sprite_shadow->pos.x = x + offset + 4.0 + 3.0;
        sprite_shadow->pos.y = y + 30.0 + 3.0;
        sprite_shadow->font = mkb::FONT_ASC_24x24;
        sprite_shadow->depth = 0.11;
        sprite_shadow->fpara1 = 0.45;
        sprite_shadow->fpara2 = 0.0;
        sprite_shadow->alpha = 0.0;
        (sprite_shadow->mult_color).red = 0x00;
        (sprite_shadow->mult_color).green = 0x00;
        (sprite_shadow->mult_color).blue = 0x00;
        sprite_shadow->width = 0.6;
        sprite_shadow->height = 0.6;
        sprite_shadow->field21_0x20 = 0.45;
        sprite_shadow->field22_0x24 = 0;
        sprite_shadow->g_flags1 = sprite->g_flags1 | 0xa1000000;
        sprite_shadow->widescreen_translation_x = 0x60;
        mkb::sprintf(sprite_shadow->text, subtitle);
        sprite_shadow->tick_func = mkb::sprite_hud_stage_name_tick;
    }
}

void init_main_loop() {
    // Read the subtitle file. This should only be run once.
    if (subtitle_file_buf == nullptr) {
        mkb::DVDFileInfo subtitle_file_info;
        int subtitle_file_length = mkb::DVDOpen(subtitle_file_path, &subtitle_file_info);

        MOD_ASSERT_MSG(subtitle_file_length != 0,
                       "Subtitle file (stgname/subtitles.str) failed to load from disc");

        // Round the length of the subtitle file to a multiple of 32, necessary for DVDReadAsyncPrio
        subtitle_file_length = (subtitle_file_info.length + 0x1f) & 0xffffffe0;
        subtitle_file_buf = static_cast<char*>(heap::alloc(subtitle_file_length));
        subtitle_file_length = mkb::read_entire_file_using_dvdread_prio_async(&subtitle_file_info, subtitle_file_buf,
                                                                              subtitle_file_length, 0);
        char* eof = subtitle_file_buf + subtitle_file_info.length;

        MOD_ASSERT_MSG(subtitle_file_length != 0,
                       "Subtitle file (stgname/subtitles.str) failed to load from disc");

        LOG("Now parsing stage subtitle list file...");
        u16 current_stage_id = 0;

        do {
            char name[128] = {0};
            char* name_end = mkb::strchr(subtitle_file_buf, '\n');
            MOD_ASSERT_MSG(name_end != nullptr,
                           "Subtitle subtitle_file_buf ended unexpectedly, please ensure there are 421 lines in the subtitle_file_buf");
            MOD_ASSERT_MSG((name_end - subtitle_file_buf) < 128,
                           "Subtitle for a stage is greater than the limit of 127 characters");

            mkb::strncpy(name, subtitle_file_buf, (name_end - subtitle_file_buf));

            mkb::strcpy(subtitle_list[current_stage_id], name);
            current_stage_id++;
            subtitle_file_buf = name_end + 1;
        } while (current_stage_id < STAGE_COUNT && subtitle_file_buf <= eof);

        mkb::DVDClose(&subtitle_file_info);
    }

    static patch::Tramp<decltype(&mkb::create_hud_stage_name_sprites)> s_stage_name_tramp;
    patch::hook_function(
        s_stage_name_tramp,
        mkb::create_hud_stage_name_sprites, [](float x, float y) {
            s_stage_name_tramp.dest(x, y);
            sprite_init(x, y);
        });
}

void init_exoption() {
    static patch::Tramp<decltype(&mkb::create_replay_stage_name_sprites)> s_stage_name_tramp;
    patch::hook_function(
        s_stage_name_tramp,
        mkb::create_replay_stage_name_sprites, [](float x, float y) {
            s_stage_name_tramp.dest(x, y);
            sprite_init(x, y);
        });
}

}// namespace stage_name_subtitles
