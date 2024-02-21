/* Classic bomb-style timer, ported from the SMB1 decompilation project
 * In order to use this, you must copy the bomb_fuse.gma and bomb_fuse.tpl
 * files to the root of the GameCube disc (the same directory that
 * mkb2.rel_sample.rel goes in), and replace test/bmp/bmp_nml.tpl file
 * with the modified one containing the old timer sprites.
 * It also must be enabled by having `smb1-timer: enabled` in the REL Patches
 * section of your config.txt. */
#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

using namespace mkb;

#define FATAL(...) OSPanic(__FILE__, __LINE__, __VA_ARGS__)
#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof(arr[0]))

namespace smb1_timer
{

/* from bitmap.h */

enum BitmapGroupID
{
    BMP_COM,
    BMP_ADV,
    BMP_END,
    BMP_RNK,
    BMP_SEL,
    BMP_NML,
    BMP_BWL,
    BMP_RAC,
    BMP_BIL,
    BMP_FGT,
    BMP_GLF,
    BMP_TGT,
    BMP_HOW,
    BMP_CMD,
    BMP_ALL,
};

// NML group bomb images
enum
{
    BMP_NML_icon_bombtimer = (BMP_NML << 8) | 0x18,
    BMP_NML_icon_bomb_hibi,
    BMP_NML_icon_bomb_part_a,
    BMP_NML_icon_bomb_part_b,
    BMP_NML_icon_bomb_part_c,
    BMP_NML_icon_bomb_part_d,
    BMP_NML_icon_bomb_part_e,
    BMP_NML_icon_bomb_part_f,
    BMP_NML_icon_bomb_part_g,
    BMP_NML_icon_bomb_part_h,
    BMP_NML_icon_bomb_part_i,
    BMP_NML_icon_bomb_part_j,
};

/* from hud.c */

static float calc_bomb_scale(u16 timer)
{
    float t;

    if (timer > 60)
    {
        float t = fabs(math_sin(((60 - ((u32)timer % 60)) & 0x3F) << 8));
        return 0.20000000298023224 * (1.0 - fabs(1.0f - t * 2.0f));
    }
    if (timer < 15)
    {
        t = (u32)(15 - timer) / 15.0f;
        if (t < 0.5f)
            return 0.2f - t;
        else
            return (-0.3f + t) - 0.5f;
    }
    else
    {
        t = 1.0f - (u32)(timer - 15) / 45.0f;
        t *= 0.2f;
        return t;
    }
}

static void scale_bomb_timer(struct Sprite *sprite)  // inline
{
    float scale = calc_bomb_scale(mode_info.stage_time_frames_remaining) + 1.0f;

    sprite->width = scale;
    sprite->height = scale;
}

static void bomb_crack_sprite_main(u8 *status, struct Sprite *sprite)
{
    if (mode_info.stage_time_frames_remaining <= 480)
    {
        if (mode_info.stage_time_frames_remaining > 420)
            sprite->alpha = 1.0f - ((mode_info.stage_time_frames_remaining - 420) / 60.0f);
        else
            sprite->alpha = 1.0f;
        scale_bomb_timer(sprite);
        if (mode_info.stage_time_frames_remaining < 240)
        {
            sprite->para1 += 40.0f - (mode_info.stage_time_frames_remaining * 40.0f) / 240.0f;
            sprite->mult_color.green = abs(255 - (sprite->para1 % 510));
            sprite->mult_color.blue = sprite->mult_color.green;
            if (sprite->mult_color.blue < 128)
                sprite->mult_color.blue = 128;
            sprite->mult_color.blue = (sprite->mult_color.blue - 128) * 2.0f;
        }
        if (mode_info.stage_time_frames_remaining <= 0)
            *status = 0;
    }
}

static void bomb_frag_sprite_main(u8 *status, struct Sprite *sprite)
{
    s16 x, y;
    float dx, dy;

    sprite->alpha *= 0.95f;
    sprite->width *= 1.01f;
    sprite->height *= 1.01f;

    x = sprite->para1;
    y = sprite->para2;

    dx = (x * 0.9f) * (sprite->alpha * sprite->alpha);
    dy = (y * 0.97f) * (sprite->alpha * sprite->alpha) + (1.0f - (sprite->alpha * sprite->alpha));

    sprite->pos.x += dx;
    sprite->pos.y += dy;

    if (sprite->alpha < 0.005f)
        *status = 0;
}

static s16 bombFragBitmapIds[] =
{
    BMP_NML_icon_bomb_part_a,
    BMP_NML_icon_bomb_part_b,
    BMP_NML_icon_bomb_part_c,
    BMP_NML_icon_bomb_part_d,
    BMP_NML_icon_bomb_part_e,
    BMP_NML_icon_bomb_part_f,
    BMP_NML_icon_bomb_part_g,
    BMP_NML_icon_bomb_part_h,
    BMP_NML_icon_bomb_part_i,
    BMP_NML_icon_bomb_part_j,
};

static float bombFragX[] = { 7.0f, 16.0f, 26.0f, 48.0f,  0.0f,  9.0f, 55.0f, 12.0f, 33.0f, 71.0f };
static float bombFragY[] = { 9.0f,  0.0f,  0.0f, 4.0f,  24.0f, 16.0f, 23.0f, 63.0f, 56.0f, 69.0f };

static void bomb_sprite_main(u8 *status, struct Sprite *sprite)
{
    float x;
    float y;
    float xscale;
    float yscale;
    struct Sprite *fragSprite;
    u32 i;

    if (mode_info.stage_time_frames_remaining <= 600)
    {
        if (mode_info.stage_time_frames_remaining > 0)
        {
            scale_bomb_timer(sprite);
            return;
        }

        // with no time left on clock, destroy this sprite and spawn fragments
        x = sprite->pos.x;
        y = sprite->pos.y;
        xscale = sprite->width;
        yscale = sprite->height;
        *status = 0;
        //u_debug_set_cursor_pos(5, 5);
        for (i = 0; i < 10; i++)
        {
            fragSprite = create_sprite();
            if (fragSprite == NULL)
                return;
            fragSprite->type = SPRT_BMP;
            fragSprite->unique_id = 2;
            fragSprite->pos.x = x - 44.0f + bombFragX[i];
            fragSprite->pos.y = y - 44.0f + bombFragY[i];
            fragSprite->font = FONT_ASCII;
            fragSprite->bmp = bombFragBitmapIds[i];
            fragSprite->alignment = ALIGN_LOWER_RIGHT;
            fragSprite->depth = 0.2f;
            fragSprite->tick_func = bomb_frag_sprite_main;
            fragSprite->width = xscale;
            fragSprite->height = yscale;
            fragSprite->para1 = 1.2f * (bombFragX[i] - 30.0f);
            fragSprite->para2 = 1.2f * (bombFragY[i] - 20.0f);
            sprintf(fragSprite->text, "bomb_scat%d.pic", i);
            sprite->g_flags1 |= 0x01000000;
            sprite->widescreen_translation_x = 320;
        }
    }
}

static void hud_show_bomb(float x, float y)
{
    struct Sprite *sprite;
    float crackX;
    float crackY;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = SPRT_BMP;
        sprite->unique_id = 2;
        sprite->pos.x = x;
        sprite->pos.y = y;
        sprite->font = FONT_ASCII;
        sprite->bmp = BMP_NML_icon_bombtimer;
        sprite->alignment = ALIGN_CENTER;
        sprite->depth = 0.2f;
        sprite->tick_func = bomb_sprite_main;
        sprintf(sprite->text, "timer.pic");
        sprite->g_flags1 |= 0x01000000;
        sprite->widescreen_translation_x = 320;
        crackY = sprite->pos.y;
        crackX = sprite->pos.x;

        // spawn a second sprite to show cracks
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->type = SPRT_BMP;
            sprite->unique_id = 2;
            sprite->pos.x = crackX;
            sprite->pos.y = crackY;
            sprite->font = FONT_ASCII;
            sprite->bmp = BMP_NML_icon_bomb_hibi;
            sprite->alignment = ALIGN_CENTER;
            sprite->depth = 0.197f;
            sprite->alpha = 0.0f;
            sprite->tick_func = bomb_crack_sprite_main;
            sprite->para1 = 0;
            sprintf(sprite->text, "hibi");
            sprite->g_flags1 |= 0x01000000;
            sprite->widescreen_translation_x = 320;
        }
    }
}

/* Time numbers */

// TODO: these functions must already be in SMB2, right? Do we need func_80049E7C?
static void normal_timer_seconds_sprite_main(u8 *arg0, struct Sprite *sprite)
{
    //int time = (int)func_80049E7C(replayInfo.unk0[replayInfo.unk14], replayInfo.unk10) + 1;
    int time = mode_info.stage_time_frames_remaining;
    sprintf(sprite->text, "%03d", time / 60);
}

static void normal_timer_100th_seconds_sprite_main(u8 *arg0, struct Sprite *sprite)
{
    //int time = (int)func_80049E7C(replayInfo.unk0[replayInfo.unk14], replayInfo.unk10) + 1;
    int time = mode_info.stage_time_frames_remaining;
    int val = 100.0 * ((float)(time % 60) / 60.0);
    sprintf(sprite->text, ":%02d", val);
}

// TODO: don't hardcode position. I don't know where else the timer can be drawn in SMB2.
static void show_smb1_timer(float x, float y)
{
    struct Sprite *sprite;

    hud_show_bomb(320.0f, 68.0f);

    // numbers
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->pos.x = 320.0f;
        sprite->pos.y = 85.0f;
        sprite->font = FONT_NUM_NML_TIME;  // TODO: is the classic SMB1 FONT_NUM_24x37 font available?
        sprite->alignment = ALIGN_UPPER_CENTER;
        sprite->depth = 0.19f;
        sprite->tick_func = normal_timer_seconds_sprite_main;
        sprintf(sprite->text, "000");
        sprite->g_flags1 |= 0x01000000;
        sprite->widescreen_translation_x = 320;
        sprite = create_linked_sprite(sprite);
        if (sprite != NULL)
        {
            sprite->pos.x = -4.0f;
            sprite->font = FONT_NUM_NML_TIME_S;
            sprite->alignment = ALIGN_UPPER_CENTER;
            sprite->depth = 0.19f;
            sprite->tick_func = normal_timer_100th_seconds_sprite_main;
            sprintf(sprite->text, ":00");
            sprite->g_flags1 |= 0x01000000;
            sprite->widescreen_translation_x = 320;
        }
    }
}

/* from polydisp.c */

static struct
{
    int state;
    int unused;
    u32 angle;
    s32 angleDelta;
    float scale;
    float scaleDelta;
} bombSpark;

static void reset_spark_vars(void)
{
    bombSpark.state = 0;
    bombSpark.angle = 0;
    bombSpark.angleDelta = 0;
    bombSpark.scale = 1.0f;
    bombSpark.scaleDelta = 0.0f;
}

struct StagedefAnimKeyframe bombSparkXKeyframes[] =
{
    { 1,   0,  8.7540102,          0,          0 },
    { 1,  72, 1.83571005,  -0.090412,  -0.090412 },
    { 1,  74,    1.66866,   -0.07464,   -0.07464 },
    { 1,  76,      1.536, -0.0599843, -0.0599843 },
    { 1,  84,    1.11173, -0.0567173, -0.0567173 },
    { 1,  89,   0.801074, -0.0672826, -0.0672826 },
    { 1,  92,   0.582271, -0.0860023, -0.0860023 },
    { 1,  93,    0.48935, -0.0852437, -0.0852437 },
    { 1,  95,    0.33974, -0.0706416, -0.0706416 },
    { 1, 100,  0.0010883,          0,          0 },
};

struct StagedefAnimKeyframe bombSparkYKeyframes[] =
{
    { 1,   0,   -1.00663,            0,            0 },
    { 1,  49,   -1.00662, -0.000118196, -0.000118196 },
    { 1,  70,  -0.994419,     0.010028,     0.010028 },
    { 1,  72,  -0.957594,    0.0308768,    0.0308768 },
    { 1,  73,  -0.919437,    0.0458789,    0.0458789 },
    { 1,  75,  -0.800302,    0.0691816,    0.0691816 },
    { 1,  83,   -0.16478,     0.079378,     0.079378 },
    { 1,  88,   0.210374,    0.0705764,    0.0705764 },
    { 1,  91,   0.411975,    0.0589553,    0.0589553 },
    { 1,  92,   0.465595,    0.0239604,    0.0239604 },
    { 1,  93,   0.459896,   -0.0309341,   -0.0309341 },
    { 1,  95,   0.340102,    -0.065165,    -0.065165 },
    { 1, 100, -0.0010541,            0,            0 },
};

static struct TplBuffer *customTpl = NULL;
static struct GmaBuffer *customGma = NULL;

static char tplName[] = "bomb_fuse.tpl";
static char gmaName[] = "bomb_fuse.gma";

static void load_smb1_bombfuse_assets(void)
{
    if (customGma == NULL)
    {
        if (customTpl == NULL)
        {
            customTpl = g_load_tpl(tplName);
            if (customTpl == NULL)
                FATAL("[smb1-timer] Could not load %s\n", tplName);
            OSReport("[smb1-timer] Loaded %s\n", tplName);
        }
        customGma = g_load_gma(gmaName, customTpl);
        if (customGma == NULL)
            FATAL("[smb1-timer] Could not load %s\n", gmaName);
        OSReport("[smb1-timer] Loaded %s\n", gmaName);
    }
}

#define INFO_FLAG_TIMER_PAUSED (1 << 3)

enum
{
    EV_STATE_INACTIVE = 0,
    EV_STATE_RUNNING = 2,
    EV_STATE_SUSPENDED = 4
};

void draw_timer_bomb_fuse(void)
{
    load_smb1_bombfuse_assets();

    struct Sprite *sprite;
    float t;  // portion of clock time remaining (from 0.0 to 1.0)
    float x;
    float y;
    float scale;
    Mtx44 mtx;
    Vec sparkPos;

    if (events[EVENT_VIEW].status == EV_STATE_RUNNING || mode_info.stage_time_frames_remaining <= 0)
        return;

    sprite = get_sprite_with_unique_id(2);
    if (sprite == NULL)
    {
        x = 0.0f;
        y = 0.0f;
    }
    else
    {
        x = (sprite->pos.x - 320.0f) / 320.0f;
        y = (56.0f - sprite->pos.y) / 240.0f;
    }

    C_MTXPerspective(mtx, 60.0f, g_current_camera->aspect, 0.00989999994635582f, 20000.0f);
    mtx[0][2] -= mtx[0][0] * x * g_current_camera->aspect * 0.5773502588272095f;
    mtx[1][2] -= mtx[1][1] * y * 0.5773502588272095f;
    GXSetProjection(mtx, 0);

    t = (float)mode_info.stage_time_frames_remaining / (float)mode_info.stage_time_limit;

    switch (bombSpark.state)
    {
    case 0:
        if (!(mode_info.ball_mode & INFO_FLAG_TIMER_PAUSED))
        {
            bombSpark.state = 1;
            bombSpark.scaleDelta = 0.125f;
            bombSpark.angleDelta = -((rand() & 0x3FF) + 0x400);
        }
        break;
    case 1:
        bombSpark.scaleDelta -= 0.0083333333333333332;
        bombSpark.scale += bombSpark.scaleDelta;
        if (bombSpark.scale < 1.0)
        {
            bombSpark.scale = 1.0f;
            bombSpark.scaleDelta = 0.0f;
            bombSpark.state = 2;
        }
        break;
    case 2:
        if (mode_info.ball_mode & INFO_FLAG_TIMER_PAUSED)
            bombSpark.state = 3;
        break;
    case 3:
        bombSpark.state = 4;
        break;
    case 4:
        bombSpark.state = 0;
        break;
    }
    if (mode_info.ball_mode & INFO_FLAG_TIMER_PAUSED)
        bombSpark.angleDelta -= (bombSpark.angleDelta >> 3);
    else if (t > 0.5)
        bombSpark.angleDelta += (-768 - bombSpark.angleDelta) >> 4;
    else
        bombSpark.angleDelta += (-1536 - bombSpark.angleDelta) >> 4;
    if (!(g_some_other_flags & 0xA))
        bombSpark.angle += bombSpark.angleDelta;

    avdisp_set_post_mult_color(1.0f, t, 0.0f, 1.0f);
    mtxa_from_translate_xyz(0.0f, (1.0 - t) - 0.5, 0.0f);
    g_something_with_texture_scroll_3(0, mtxa);

    // Draw new bomb fuse
    mtxa_from_identity();
    mtxa_translate_xyz(0.00094f, 0.00519f, -0.01f);
    scale = 0.0007f;
    mtxa_scale_s(scale);
    load_gx_pos_nrm_mtx(mtxa, 0);
    //avdisp_set_bound_sphere_scale(scale);
    g_avdisp_bound_sphere_scale = scale;
    g_avdisp_draw_model_now1(customGma->model_entries[0].model);

    // Draw spark
    sparkPos.x = evaluate_stagedef_keyframe((1.0 - t) * 100.0, ARRAY_COUNT(bombSparkXKeyframes), bombSparkXKeyframes);
    sparkPos.y = evaluate_stagedef_keyframe((1.0 - t) * 100.0, ARRAY_COUNT(bombSparkYKeyframes), bombSparkYKeyframes);
    sparkPos.z = 0.141f;
    mtxa_translate(&sparkPos);
    mtxa_sq_from_identity();
    mtxa_rotate_z(bombSpark.angle);
    mtxa_scale_s(0.0149f);
    mtxa_scale_xyz(bombSpark.scale, bombSpark.scale, bombSpark.scale);
    // SMB1 draws the spark as a Naomi model. However, SMB2 seems to also have the spark model in common.gma
    load_gx_pos_nrm_mtx(mtxa, 0);
    avdisp_set_post_mult_color(1, 1, 1, 1);
    avdisp_draw_model_unculled_sort_never(init_common_gma->model_entries[88].model);
}

static patch::Tramp<decltype(&mkb::create_timer_sprites)> s_timerSpiteTramp;
static patch::Tramp<decltype(&mkb::polydisp_main)> s_bombFuseTramp;
static patch::Tramp<decltype(&mkb::create_monkey_counter_sprites)> s_lifeIconTramp;

// insert code to draw the SMB1 bomb fuse model
static void polydisp_main_override(void)
{
    s_bombFuseTramp.dest();
    if (get_sprite_with_unique_id(2))
        draw_timer_bomb_fuse();
}

// reposition the life icon as to not cover up the timer fuse
void create_monkey_counter_sprites_override(double x, double y)
{
    x = 52.0;
    y = 92.0;
    if (widescreen_mode != NORMAL)
        x -= 150;
    s_lifeIconTramp.dest(x, y);
}

void init_main_loop()
{
    // TODO: I would like to load stuff here, at the beginning, but for some reason, the custom GMA and
    // TPL get overwritten with other stuff and cause the game to crash.
    //load_smb1_bombfuse_assets();
    reset_spark_vars();
    patch::hook_function(s_timerSpiteTramp, mkb::create_timer_sprites, show_smb1_timer);
    patch::hook_function(s_bombFuseTramp, mkb::polydisp_main, polydisp_main_override);
    patch::hook_function(s_lifeIconTramp, mkb::create_monkey_counter_sprites, create_monkey_counter_sprites_override);
}

TICKABLE_DEFINITION((
        .name = "smb1-timer",
        .description = "Classic bomb timer from SMB1",
        .init_main_loop = init_main_loop, ))

}  // namespace smb1_timer
