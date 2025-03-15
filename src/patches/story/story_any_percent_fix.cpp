#include "story_any_percent_fix.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

// Fixes an issue where skipping more than a certain number of stages for story mode any% causes a crash.
namespace story_any_percent_fix {

TICKABLE_DEFINITION((
        .name = "story-any-percent-fix",
        .description = "Story mode any-percent crash fix",
        .init_main_loop = init_main_loop,
        .tick = tick, ))

static bool chara_heap_cleared;
static mkb::SpriteTex* texes[10] = {};
static u8 active_sprite_idx = 0.;
static patch::Tramp<decltype(&mkb::g_load_preview_texture)> tex_load_tramp;

// Keeps track all preview image sprite pointers as they are loaded. Only 10 are loaded for story mode.
void init_main_loop() {
    patch::hook_function(tex_load_tramp, mkb::g_load_preview_texture,
                         [](mkb::SpriteTex* sprite_tex, char* file_path, u32 param_3, u16 width, u16 height,
                            u32 format) {
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
                             tex_load_tramp.dest(sprite_tex, file_path, param_3, width, height, format);
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

}// namespace story_any_percent_fix
