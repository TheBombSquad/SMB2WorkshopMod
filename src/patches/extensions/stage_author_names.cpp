#include "stage_author_names.h"

#include "internal/heap.h"
#include "internal/log.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"


// Allows for stage author names to be displayed under the stage name.
namespace stage_author_names {

TICKABLE_DEFINITION((
        .name = "stage-author-names",
        .description = "[mod] Stage author name display %s\n",
        .init_main_loop = init_main_loop, ))

static char author_file_path[] = "/stgname/authors.str";
static char author_fallback_name = '\0';
static char* author_file_buf = nullptr;
static constexpr u16 STAGE_COUNT = 421;
static char author_list[STAGE_COUNT][128];

void sprite_init(float x, float y) {

    char* author_name;

    if (mkb::current_stage_id >= 0 && mkb::current_stage_id < STAGE_COUNT) {
        author_name = author_list[mkb::current_stage_id];
    }
    else {
        author_name = &author_fallback_name;
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
        mkb::sprintf(sprite->text, author_name);
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
        mkb::sprintf(sprite_shadow->text, author_name);
        sprite_shadow->tick_func = mkb::sprite_hud_stage_name_tick;
    }
}

void init_main_loop() {
    // Read the author file. This should only be run once.
    if (author_file_buf == nullptr) {
        mkb::DVDFileInfo author_file_info;
        int author_file_length = mkb::DVDOpen(author_file_path, &author_file_info);

        MOD_ASSERT_MSG(author_file_length != 0,
                       "Author name file (stgname/authors.str) failed to load from disc");

        // Round the length of the author file to a multiple of 32, necessary for DVDReadAsyncPrio
        author_file_length = (author_file_info.length + 0x1f) & 0xffffffe0;
        author_file_buf = static_cast<char*>(heap::alloc(author_file_length));
        author_file_length = mkb::read_entire_file_using_dvdread_prio_async(&author_file_info, author_file_buf,
                                                                            author_file_length, 0);
        char* eof = author_file_buf + author_file_info.length;

        MOD_ASSERT_MSG(author_file_length != 0,
                       "Author name file (stgname/authors.str) failed to load from disc");

        mkb::OSReport("[mod] Now parsing stage author list file...\n");
        u16 current_stage_id = 0;

        do {
            char name[128] = {0};
            char* name_end = mkb::strchr(author_file_buf, '\n');
            MOD_ASSERT_MSG(name_end != nullptr,
                           "Author name author_file_buf ended unexpectedly, please ensure there are 421 lines in the author_file_buf");
            MOD_ASSERT_MSG((name_end - author_file_buf) < 128,
                           "Author name for a stage is greater than the limit of 127 characters");

            mkb::strncpy(name, author_file_buf, (name_end - author_file_buf));

            mkb::strcpy(author_list[current_stage_id], name);
            current_stage_id++;
            author_file_buf = name_end + 1;
        } while (current_stage_id < STAGE_COUNT && author_file_buf <= eof);

        mkb::DVDClose(&author_file_info);
    }

    static patch::Tramp<decltype(&mkb::create_hud_stage_name_sprites)> s_stage_name_tramp;
    patch::hook_function(
        s_stage_name_tramp,
        mkb::create_hud_stage_name_sprites, [](float x, float y) {
            s_stage_name_tramp.dest(x, y);
            sprite_init(x, y);
        });
}

}// namespace stage_author_names
