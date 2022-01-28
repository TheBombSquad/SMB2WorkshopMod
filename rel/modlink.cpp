#include "modlink.h"

#include "version.h"
#include "heap.h"

namespace modlink {

static constexpr u32 MODLINK_ADDR = 0x800a9cb4;
static constexpr u32 MAGIC = 0xFEEDC0DE;

void write() {
    ModLink* link = reinterpret_cast<ModLink*>(MODLINK_ADDR);
    link->magic = MAGIC;
    link->modlink_version = {1, 0, 0};
    link->wsmod_version = version::WSMOD_VERSION;
    link->malloc_func = heap::alloc;
    link->heap_info = &heap::get_heap_info();
}

ModLink* get() {
    ModLink* link = reinterpret_cast<ModLink*>(MODLINK_ADDR);
    if (link->magic != MAGIC) {
        return nullptr;
    }
    return link;
}

}