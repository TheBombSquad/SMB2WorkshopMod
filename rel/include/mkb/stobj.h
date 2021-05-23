#pragma once
#include "mkb/mkb.h"

namespace mkb
{

constexpr s32 MAX_STAGE_OBJECTS = 144;
constexpr s32 MAX_GOALS = 16;

enum StobjType
{
    STOBJ_BUMPER = 0,
    STOBJ_JAMABAR = 1,
    STOBJ_GOALTAPE = 2,
    STOBJ_GOALBAG = 3,
    STOBJ_GOALBAG_EXMASTER = 4,
    STOBJ_MF_PNL_BUMPER = 5,
    STOBJ_MF_PNL_ELECTRAP = 6,
    STOBJ_MF_BULLET_TEST = 7,
    STOBJ_MF_BOX = 8,
    STOBJ_BUMPER_BGSPECIAL = 9,
    STOBJ_NAMEENT_BTN = 10,
    STOBJ_BUTTON = 11,
    STOBJ_RETURNGATE = 12
};

struct Stobj
{
    short idx;
    short id;
    short type;
    u8 field_0x6;
    u8 field_0x7;
    u32 g_some_bitflag;
    short g_mode;
    short g_counter;
    struct Vec3f g_model_origin;
    struct Vec3f position;
    struct Vec3f position_2; // Copy of position?
    float bounding_sphere_radius; // Has something to do w/ collision
    void (* coli_func)(struct Stobj *, struct PhysicsBall *);
    struct Vec3f scale;
    float field_0x48;
    float field_0x4c;
    float field_0x50;
    void* g_visual_model_ptr;
    struct Vec3f g_some_position; // Has something to do w/ position
    struct Vec3f velocity;
    struct Vec3s rot;
    short field_0x76;
    short field_0x78;
    u8 field_0x7a;
    u8 field_0x7b;
    struct Vec3f g_prev_pos;
    struct Vec3s g_prev_rot;
    u8 field_0x8e;
    u8 field_0x8f;
    float field_0x90;
    float field_0x94;
    float field_0x98;
    float field_0x9c;
    u8 itemgroup_id;
    u8 field_0xa1;
    u8 field_0xa2;
    u8 field_0xa3;
    void * extra_data; // Extra stobj-type-specific data, such as switch stagedef header for switches or goaltape struct for goaltapes. Maybe worth making a union
    struct Vec3f g_some_pos2;
    struct Vec3f g_local_position;
    struct Vec3f g_local_velocity;
};

static_assert(sizeof(Stobj) == 204);

// Extra data associated with goaltape stage objects
struct GoalTape
{
    u8 unk_0x0[408];
};

static_assert(sizeof(GoalTape) == 408);

// Extra data associated with party ball stage objects
struct GoalBag
{
    u8 unk_0x0[40];
};

static_assert(sizeof(GoalBag) == 40);

extern "C"
{
extern Stobj stobjs[MAX_STAGE_OBJECTS];
extern PoolInfo stobj_pool_info;
extern GoalTape goaltapes[MAX_GOALS];
extern GoalBag goalbags[MAX_GOALS];
void stobj_bumper_init(Stobj *stobj);
void stobj_bumper_coli(Stobj *obj, PhysicsBall *physicsball);
}

}
