#pragma once

#include <mkb/pool.h>
#include "mkb.h"

namespace mkb
{

constexpr s32 MAX_ITEMS = 256;

enum ItemType
{
    ITEM_COIN = 0,
    ITEM_FGT_BANANA = 1,
    ITEM_MINI_RACE = 2,
    ITEM_FGT_POWER = 3,
    ITEM_FGT_PUNCH = 4,
    ITEM_PILOT = 5,
    ITEM_DOGFIGHT = 6,
    ITEM_TMP001 = 7,
    ITEM_TMP002 = 8,
    ITEM_TMP003 = 9,
    ITEM_TMP004 = 10,
    ITEM_TMP005 = 11,
    ITEM_TMP006 = 12,
    ITEM_TMP007 = 13,
    ITEM_TMP008 = 14,
    ITEM_TMP009 = 15,
    ITEM_TMP010 = 16,
    ITEM_TMP011 = 17,
    ITEM_TMP012 = 18,
    ITEM_TMP013 = 19,
    ITEM_TMP014 = 20,
    ITEM_TMP015 = 21,
    ITEM_TMP016 = 22
};

struct Item { // Represents an item that can be picked up by the player. These are bananas in main game, but can be other pickups in the party games. Are "tickable" like events, effects, etc.
    short index;
    short id;
    short type;
    s16 coin_type;
    u32 g_some_bitfield;
    u16 field_0xc;
    u16 g_some_flag;
    short field_0x10;
    u16 g_some_flag2;
    float scale;
    float field_0x18;
    u8* g_something_with_gma_model;
    struct Vec3f position;
    struct Vec3f velocity;
    struct Vec3s rotation;
    struct Vec3s angular_velocity;
    struct Vec3f g_position_copy;
    struct Vec3s g_rotation_copy;
    u8 field_0x56;
    u8 field_0x57;
    void (* item_coli_func)(struct Item *, struct PhysicsBall *); // Created by retype action
    u8 itemgroup_idx;
    u8 field_0x5d;
    s16 g_some_frame_counter;
    struct Vec3f* g_some_vec3f_ptr;
    u32 field_0x64;
    struct GmaModelHeader * model_ptr;
    float field_0x6c;
    s16 negative_y_rotation;
    u8 field_0x72;
    u8 field_0x73;
    float field_0x74;
    float g_something_with_shadow_disp;
    struct Vec3f shadow_scale;
    float shadow_intensity;
    struct Vec3f g_position_copy_2;
    u32 g_some_flag_2;
    struct Vec3f shadow_position;
    struct Vec3f g_something_with_shadows;
};

static_assert(sizeof(Item) == 180);

extern "C"
{
extern Item items[MAX_ITEMS];
extern PoolInfo item_pool_info;
extern u32 g_next_item_id;
extern u32 some_item_thing;

void item_coin_init(Item* item);
void item_coin_tick(Item* item);
void item_coin_disp(Item* item);
void item_coin_coli(Item* item, PhysicsBall* phys_ball);
}

}
