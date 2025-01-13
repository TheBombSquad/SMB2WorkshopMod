#include "remove_story_cutscenes.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"
#include "utils/ppcutil.h"


namespace remove_story_cutscenes {

static u16 DEFAULT_CUTSCENE_SKIP_SETTING = 0;

TICKABLE_DEFINITION((
        .name = "remove-story-cutscenes",
        .description = "Story Mode cutscene removal",
        .init_main_game = init_main_game, ))

constexpr auto WORLD_COUNT = 10;// TODO: attach to patch that changes this

// Skips cutscenes in story mode.
// TODO: Maybe fade the screen out so the transition screen color is not based off the world fog

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
    mkb::scen_info.next_world = 0;
    mkb::scen_info.mode = mkb::DMD_SCEN_SEL_WORLD_INIT;
    return;
}

// Skips the rest of the cutscenes, and handles the credits, name entry, and game over sequences.
// This is called when a cutscene is attempted to be loaded.
void dmd_scen_sceneplay_init_patch() {
    mkb::dest_all_sprites();
    if (active_state > WORLD_COUNT) {
        active_state++;
    }
    else {
        active_state = mkb::scen_info.next_world;
    }

    mute_all_music_tracks();

    // If we're in 'world 11', initialize the credits sequence.
    if (active_state == WORLD_COUNT) {
        mkb::mode_flags = mkb::mode_flags | 0x100000;
        patch::write_word(reinterpret_cast<void*>(0x8054dbdc), 0xffffffff);
        mkb::scen_info.mode = mkb::DMD_SCEN_GAME_CLEAR_INIT;
    }

    // If we're in 'world 12', initialize the name entry sequence.
    // This requires some odd heap-clearing stuff to be handled that would otherwise be handled
    // when returning from the cutscene.
    else if (active_state == WORLD_COUNT + 1) {
        mkb::dest_all_events();

        reinterpret_cast<void (*)()>(mkb::g_something_freeing_heap_4)();
        reinterpret_cast<void (*)()>(mkb::g_something_freeing_heap_parent)();
        reinterpret_cast<void (*)(int)>(mkb::g_something_with_sound7_and_game_heaps)(0);

        mkb::OSSetCurrentHeap(mkb::chara_heap);

        mkb::mode_flags = mkb::mode_flags | 0x100000;
        patch::write_word(reinterpret_cast<void*>(0x8054dbdc), 0xffffffff);
        mkb::scen_info.mode = mkb::DMD_SCEN_NAMEENTRY_INIT;
    }

    // If we're in 'world 13', initialize the game over sequence.
    else if (active_state == WORLD_COUNT + 2) {
        mkb::mode_flags = mkb::mode_flags | 0x100000;
        patch::write_word(reinterpret_cast<void*>(0x8054dbdc), 0xffffffff);
        mkb::scen_info.mode = mkb::DMD_SCEN_GAME_OVER_INIT;
    }

    // Otherwise, we're on a valid world, so, instead of loading a cutscene, just go to the
    // next world's stage select screen.
    else {
        mkb::g_SoftStreamStart_with_some_defaults_2(0);
        mkb::scen_info.next_world++;
        mkb::scen_info.mode = mkb::DMD_SCEN_SEL_WORLD_NEXT;
    }

    return;
}

// Keeps active_state synced between file loads.
void dmd_scen_sel_floor_init_patch() {
    active_state = mkb::scen_info.world;
    mkb::scen_info.mode = mkb::DMD_SCEN_SEL_FLOOR_MAIN;

    // I have no idea what this does, it's something the game does in the original function
    u32 data = *reinterpret_cast<u32*>(0x8054dbc0);
    patch::write_word(reinterpret_cast<void*>(0x8054dbc0), data | 2);
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
    patch::write_branch(reinterpret_cast<void*>(mkb::dmd_scen_newgame_main),
                        reinterpret_cast<void*>(dmd_scen_newgame_main_patch));
    patch::write_branch(reinterpret_cast<void*>(mkb::dmd_scen_sceneplay_init),
                        reinterpret_cast<void*>(dmd_scen_sceneplay_init_patch));
    patch::write_branch(reinterpret_cast<void*>(mkb::dmd_scen_sel_floor_init),
                        reinterpret_cast<void*>(dmd_scen_sel_floor_init_patch));
    patch::write_branch(reinterpret_cast<void*>(mkb::g_preload_next_stage_files),
                        reinterpret_cast<void*>(handle_preloading));
}

}// namespace remove_story_cutscenes
