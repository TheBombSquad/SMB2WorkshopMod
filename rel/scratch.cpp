#include "scratch.h"

#include <gc/gc.h>
#include <mkb/mkb.h>

#include "patch.h"

#include <cstdint>
#include <cstring>

namespace mkb
{
extern "C"
{
void load_additional_rel(const char *rel_filepath, void *rel_buffer_ptrs);
void race2_tick();
bool did_ball_enter_wormhole(Ball *ball, s32 *out_wormhole_idx);
void teleport_through_wormhole(s32 ball_idx, s32 wormhole_idx);
}
}

namespace scratch
{
//static bool (*DVDOpen_trampoline)(char *fileName, gc::DVDFileInfo *fileInfo) = nullptr;
//static int32_t (*DVDConvertPathToEntrynum_trampoline)(char *filename) = nullptr;
//static bool (*DVDFastOpen_trampoline)(int32_t entrynum, gc::DVDFileInfo *fileInfo) = nullptr;
//static void (*DVDChangeDir_trampoline)(char *dirName) = nullptr;
//static bool (*DVDOpenDir_trampoline)(char *dirName, gc::DVDDir *dir) = nullptr;
//
//static void *(*OSAllocFromHeap_trampoline)(gc::OSHeapHandle heap, uint32_t size) = nullptr;
//static gc::OSHeapHandle (*OSCreateHeap_trampoline)(void *start, void *end) = nullptr;
//static void (*OSDestroyHeap_trampoline)(gc::OSHeapHandle heap) = nullptr;
//static void (*OSFreeToHeap_trampoline)(gc::OSHeapHandle heap, void *ptr) = nullptr;
//static void *(*OSInitAlloc_trampoline)(void *arenaStart, void *arenaEnd, int maxHeaps) = nullptr;
//static gc::OSHeapHandle (*OSSetCurrentHeap_trampoline)(gc::OSHeapHandle) = nullptr;
//
//static static void (*OSSetArenaLo_trampoline)(void *newLo) = nullptr;
//static void (*OSSetArenaHi_trampoline)(void *newHi) = nullptr;
//static void *(*OSAllocFromArenaLo_trampoline)(uint32_t size, uint32_t align) = nullptr;
//static void *(*OSAllocFromArenaHi_trampoline)(uint32_t size, uint32_t align) = nullptr;
//
//static void (*gxFinishFrame_trampoline)() = nullptr;
//static void (*createGameHeaps_trampoline)(int param1) = nullptr;

static void check_wormholes()
{
    for (u32 i = 0; i < mkb::ball_pool_info.upper_bound; i++)
    {
        s32 status = mkb::ball_pool_info.status_list[i];
        if (status == 0 || status == 4) continue;

        s32 wormhole_idx;
        if (mkb::did_ball_enter_wormhole(&mkb::balls[i], &wormhole_idx))
        {
            mkb::teleport_through_wormhole(mkb::balls[i].idx, wormhole_idx);
        }
    }
}

void init()
{
    static decltype(&mkb::load_additional_rel) load_additional_rel_trampoline;
    static decltype(&mkb::race2_tick) race2_tick_trampoline;

    load_additional_rel_trampoline = patch::hook_function(
        mkb::load_additional_rel,
        [](const char *rel_filepath, void *rel_buffer_ptrs)
        {
            load_additional_rel_trampoline(rel_filepath, rel_buffer_ptrs);

            if (strcmp(rel_filepath, "mkb2.race2.rel") != 0) return;
            gc::OSReport("[mod] Hooking race2\n");

            race2_tick_trampoline = patch::hook_function(mkb::race2_tick, []
            {
                check_wormholes();
                race2_tick_trampoline();
            });

            gc::OSReport("[mod] Finished hooking race2\n");
        });
}

void tick()
{

}

}