#pragma once

#include <gc/gc.h>
#include <mkb/dip.h>
#include <mkb/stagedef.h>
#include <mkb/stage.h>
#include <mkb/ball.h>
#include <mkb/pool.h>
#include <mkb/item.h>
#include <mkb/effect.h>
#include <mkb/stobj.h>
#include <mkb/mode.h>
#include <mkb/sprite.h>
#include <mkb/minimap.h>
#include <mkb/story.h>
#include <mkb/unlock.h>
#include <mkb/event.h>
#include <mkb/mathutil.h>
#include <mkb/pad.h>

namespace mkb
{

struct GraphicsInfo
{
    void *active_framebuffer; /* A pointer to either framebuffer1 or framebuffer2 */
    void *framebuffer1;
    void *framebuffer2;
    u32 frame_count;
    u32 active_framebuffer_index; /* Either 0 or 1, representing framebuffer1 or framebuffer2 */
    gc::GXFifoObj *fifos[2];
};

struct GmaModelHeader { // Also known as a GCMF (GameCube Model Format?)
    char gcmf_magic[4]; // Just the string "GCMF"
    u32 attrs; // Also called "section flags"
    struct Vec3f origin; // Also the center of the bounding sphere
    f32 bounding_sphere_radius;
    u16 texture_count; // In F-Zero GX this is "texture count"
    u16 material_count; // In F-Zero GX this is "material count"
    u16 translucid_material_count; // In F-Zero GX this is "translucid material count"
    s8 transform_mtx_count;
    u8 field_0x1f;
    s32 model_header_size; // Memory size of this structure in bytes, including texture description array and transform matrices.
    struct GXTexObj * texobjs; // Array of texobjs, one for each texture in the model
    s8 default_mtx_indices[8]; // Default (root?) indices into Transform Matrix array
    u8 field_0x30;
    u8 field_0x31;
    u8 field_0x32;
    u8 field_0x33;
    u8 field_0x34;
    u8 field_0x35;
    u8 field_0x36;
    u8 field_0x37;
    u8 field_0x38;
    u8 field_0x39;
    u8 field_0x3a;
    u8 field_0x3b;
    u8 field_0x3c;
    u8 field_0x3d;
    u8 field_0x3e;
    u8 field_0x3f;
};

extern "C" {

extern u32 frame_counter;

extern gc::GXColor debug_text_bg_color;
extern GraphicsInfo *graphics_info;

extern gc::OSHeapHandle main_heap, stage_heap, bg_heap, char_heap, replay_heap;
extern u32 main_heap_size, stage_heap_size, bg_heap_size, char_heap_size, replay_heap_size;
extern u32 g_something_with_game_heaps;

extern u32 curr_player_idx;
extern u8 selected_characters[4];

extern u32 bananas_remaining, ball_mode;

extern u32 g_current_stage_id;

extern u8 mode_cnt, g_currently_visible_menu_screen;
extern bool g_character_selected;

extern u32 next_stobj_id;

extern u32 g_some_other_flags;

extern u32 active_monkey_id;

void draw_debugtext();
void draw_debugtext_bg(double x1, double y1, double x2, double y2);
void draw_debugtext_char_en(u32 x, u32 y, char ch, u32 color);

void GXSetZMode_cached(gc::GXBool compare_enable, gc::GXCompare func, gc::GXBool update_enable);
void GXLoadTexObj_cached(gc::GXTexObj *obj, gc::GXTexMapID id);

void g_SoftStreamStart_with_some_defaults_2(u32 track);
void load_additional_rel(const char *rel_filepath, void *rel_buffer_ptrs);
u32 g_something_with_life_counter();
void g_fancy_memcpy(void *dest,void *src,size_t count);
void *memset (void * ptr, int value, size_t num);
void event_stobj_init();
int spawn_stobj(Stobj *stobj);
int spawn_item(Item *item);
int sprintf(char *buffer,char *format,...);

}

}
