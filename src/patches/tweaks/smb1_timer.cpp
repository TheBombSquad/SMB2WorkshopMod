/* Classic bomb-style timer, ported from the SMB1 decompilation project
 * In order to use this, you must copy the bomb_fuse.gma and bomb_fuse.tpl
 * files to the root of the GameCube disc (the same directory that
 * mkb2.rel_sample.rel goes in), and also copy the test/bmp/bmp_nml.tpl file
 * from SMB1 into the test/bmp directory of SMB2, and name it bmp_nml1.tpl.
 * Do NOT overwrite the one that's already there.
 * It also must be enabled by having `smb1-timer: disabled` in the REL Patches
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

#define BMP_NML1 BMP_ADV  // we replace the BMP_ADV group with SMB1's BMP_NML

// NML group from SMB1
enum
{
    BMP_NML1_icon_bombtimer = (BMP_NML1 << 8),
    BMP_NML1_icon_lv1234_j,
    BMP_NML1_game_rank,
    BMP_NML1_game_result_e3,
    BMP_NML1_game_goal,
    BMP_NML1_asc_ball22x22,
    BMP_NML1_asc_komo16x16,
    BMP_NML1_asc_tama32x32,
    BMP_NML1_asc_ball18x16,
    BMP_NML1_asc_ball20x20,
    BMP_NML1_fukidashi,
    BMP_NML1_game_player,
    BMP_NML1_asc_ball26x38,
    BMP_NML1_asc_ball16x22,
    BMP_NML1_DUMMY14,
    BMP_NML1_DUMMY15,
    BMP_NML1_icon_bomb_hibi,  // 0x510
    BMP_NML1_icon_bomb_part_a,
    BMP_NML1_icon_bomb_part_b,
    BMP_NML1_icon_bomb_part_c,
    BMP_NML1_icon_bomb_part_d,
    BMP_NML1_icon_bomb_part_e,
    BMP_NML1_icon_bomb_part_f,
    BMP_NML1_icon_bomb_part_g,
    BMP_NML1_icon_bomb_part_h,
    BMP_NML1_icon_bomb_part_i,
    BMP_NML1_icon_bomb_part_j,
};

static char *filepathOld = NULL;
static char *categoryOld = NULL;

// replaces some bitmaps in the NML bitmap group with ones from bmp_nml1.tpl
// The TPL is freed in the sprite dest callback.
static void load_smb1_nml_bmp(void)
{
	/*
	// debug stuff
	OSReport("Bitmap Groups:\n");
	for (int i = 0; i < 25; i++)
		OSReport("%i: file: %s, category: %s, loaded: %i\n", i, bmp_infos[i].filepath, bmp_infos[i].category, bmp_infos[i].is_loaded);
	*/

	// Replace the ADV bitmap group with our own. TODO: reset this when finished
	if (bmp_infos[BMP_NML1].is_loaded)
	{
		if (mkb::strcmp(bmp_infos[BMP_NML1].category, "BMP_ADV") == 0)
			FATAL("[smb1-timer] original ADV bitmap group is loaded. Not replacing it.\n");
		if (mkb::strcmp(bmp_infos[BMP_NML1].category, "BMP_NML1") == 0)
			return;  // already replaced, nothing to do
	}
	filepathOld = bmp_infos[BMP_NML1].filepath;
	categoryOld = bmp_infos[BMP_NML1].category;
	bmp_infos[BMP_NML1].filepath = "bmp/bmp_nml1.tpl";
	bmp_infos[BMP_NML1].category = "BMP_NML1";
	load_bmp_by_id_child(BMP_NML1);  // load it
}

static void free_smb1_nml_bmp(void)
{
	if (filepathOld == NULL || categoryOld == NULL)
		FATAL("[smb1-timer] Tried to destroy bomb sprite, but SMB1 bitmaps aren't loaded somehow!\n");

	// unload SMB1 bitmap group
	g_something_with_freeing_memory(BMP_NML1);
	// change it back to what it was before
	bmp_infos[BMP_NML1].filepath = filepathOld;
	bmp_infos[BMP_NML1].category = categoryOld;
	filepathOld = NULL;
	categoryOld = NULL;
}

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

    x = ((s16*)&sprite->para1)[0];
    y = ((s16*)&sprite->para1)[1];

    dx = (x * 0.9f) * (sprite->alpha * sprite->alpha);
    dy = (y * 0.97f) * (sprite->alpha * sprite->alpha) + (1.0f - (sprite->alpha * sprite->alpha));

    sprite->pos.x += dx;
    sprite->pos.y += dy;

/*
    // pointless, since the variables aren't modified
    ((s16*)&sprite->para1)[0] = x;
    ((s16*)&sprite->para1)[1] = y;
*/
    if (sprite->alpha < 0.005f)
        *status = 0;
}

static s16 bombFragBitmapIds[] =
{
    BMP_NML1_icon_bomb_part_a,
    BMP_NML1_icon_bomb_part_b,
    BMP_NML1_icon_bomb_part_c,
    BMP_NML1_icon_bomb_part_d,
    BMP_NML1_icon_bomb_part_e,
    BMP_NML1_icon_bomb_part_f,
    BMP_NML1_icon_bomb_part_g,
    BMP_NML1_icon_bomb_part_h,
    BMP_NML1_icon_bomb_part_i,
    BMP_NML1_icon_bomb_part_j
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
            ((s16 *)&fragSprite->para1)[0] = 1.2f * (bombFragX[i] - 30.0f);
            ((s16 *)&fragSprite->para1)[1] = 1.2f * (bombFragY[i] - 20.0f);
            sprintf(fragSprite->text, "bomb_scat%d.pic", i);
        }
    }
}

// When the bomb sprite is destroyed, clean up the hacking we did earlier
static void bomb_sprite_dest(struct Sprite *sprite)
{
	//OSReport("[smb1-timer] bomb_sprite_dest\n");

	free_smb1_nml_bmp();
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
        sprite->bmp = BMP_NML1_icon_bombtimer;
        sprite->alignment = ALIGN_CENTER;
        sprite->depth = 0.2f;
        sprite->tick_func = bomb_sprite_main;
        sprite->dest_func = bomb_sprite_dest;
        sprintf(sprite->text, "timer.pic");
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
            sprite->bmp = BMP_NML1_icon_bomb_hibi;
            sprite->alignment = ALIGN_CENTER;
            sprite->depth = 0.197f;
            sprite->alpha = 0.0f;
            sprite->tick_func = bomb_crack_sprite_main;
            sprite->para1 = 0;
            sprintf(sprite->text, "hibi");
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

	//OSReport("[smb1-timer] show_smb1_timer(%.2f, %.2f)\n", x, y);

	load_smb1_nml_bmp();

	hud_show_bomb(320.0f, 68.0f);

	// numbers
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->pos.x = 320.0f;
        sprite->pos.y = 85.0f;
        //sprite->font = FONT_NUM_24x37;
        sprite->font = FONT_NUM_NML_TIME;  // TODO: is the classic SMB1 FONT_NUM_24x37 font available?
        sprite->alignment = ALIGN_UPPER_CENTER;
        sprite->depth = 0.19f;
        sprite->tick_func = normal_timer_seconds_sprite_main;
        sprintf(sprite->text, "000");
        sprite = create_linked_sprite(sprite);
        if (sprite != NULL)
        {
            sprite->pos.x = -4.0f;
            //sprite->font = FONT_NUM_12x19;
            sprite->font = FONT_NUM_NML_TIME_S;
            sprite->alignment = ALIGN_UPPER_CENTER;
            sprite->depth = 0.19f;
            sprite->tick_func = normal_timer_100th_seconds_sprite_main;
            sprintf(sprite->text, ":00");
        }
    }
}

/* from polydisp.c */

struct
{
    //u32 unk0;
    //Vec unk4;
    //Vec unk10;
    //Vec unk1C;
    //Vec unk28;
    //Vec unk34;
    //Vec unk40;
    s32 unk4C;  // 5C
    //u8 filler50[4];
    u32 unk54;
    s32 unk58;
    float unk5C;
    float unk60;
    //u8 filler64[4];
} polyDisp;  // TODO: see if this struct exists in SMB2

static void reset_spark_vars(void)
{
    polyDisp.unk4C = 0;
    polyDisp.unk54 = 0;
    polyDisp.unk58 = 0;
    polyDisp.unk5C = 1.0f;
    polyDisp.unk60 = 0.0f;
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

static /*const*/ char tplName[] = "bomb_fuse.tpl";
static /*const*/ char gmaName[] = "bomb_fuse.gma";

static void debug_gma(struct GmaBuffer *gma)
{
	OSReport("%i models:\n", gma->model_count);
	for (int i = 0; i < gma->model_count; i++)
	{
		struct GmaModelEntry *entry = &gma->model_entries[i];
		struct GmaModel *model = entry->model;
		OSReport("%i: '%s' %p\n", i, entry->name, entry->model);
		if (model != NULL)
			OSReport("  attrs: 0x%X, header_size: 0x%X, tex_objs: %p\n", model->attrs, model->header_size, model->texobjs);
	}
}

static void load_smb1_common_assets(void)
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
		//debug_gma(customGma);
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
	load_smb1_common_assets();

    //struct NlModel *tempModel;
    struct Sprite *sprite;
    float t;  // portion of clock time remaining (from 0.0 to 1.0)
    float x;
    float y;
    float scale;
    //Vec sp94;
    Mtx44 mtx;
    //Vec sp48;
    //Vec sp3C;
    //Vec sp30;
    //Vec sp24;
    Vec sparkPos;
    //float f4;
    //float f3;
    //struct NlVtxTypeB *vtx;
    //int i;
    //int r7;
    //int faceCount;
    //float f1;
    //struct NlMesh *mesh;

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

    C_MTXPerspective(mtx, 60.0f, 1.3333332538604736f, 0.00989999994635582f, 20000.0f);
    mtx[0][2] -= mtx[0][0] * x * 1.3333332538604736f * 0.5773502588272095f;
    mtx[1][2] -= mtx[1][1] * y * 0.5773502588272095f;
    GXSetProjection(mtx, 0);

    /* NOTE: Most of the code here deals with manipulating vertices for the
     * arcade fuse model, which is never drawn in-game.
     */
    //tempModel = lbl_802F1B4C;
    t = (float)mode_info.stage_time_frames_remaining / (float)mode_info.stage_time_limit;
/*
    // Make a temporary copy of the timer fuse, which we will modify
    memcpy(
        tempModel,
        NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_OBJ_COLOR_BAR_03),
        NLMODEL_HEADER(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_OBJ_COLOR_BAR_03))->unk4->modelSize);

    mesh = (struct NlMesh *)tempModel->meshStart;
    faceCount = ((struct NlDispList *)(((struct NlMesh *)tempModel->meshStart)->dispListStart))->faceCount;

    f4 = 2.0 * (t - 0.5);
    f4 = CLAMP(f4, 0.0, 1.0);

    f3 = f4 * (faceCount - 2.0);
    r7 = mathutil_floor(f3 * 0.5) * 2.0f;
    f1 = (f3 - r7) * 0.5;

    vtx = (struct NlVtxTypeB *)((struct NlDispList *)mesh->dispListStart)->vtxData;
    for (i = faceCount - 1; i >= 0; i--, vtx++)
    {
        if (t < 0.5)
            vtx->s = 0.25;
        else if (i < r7)
            vtx->s = 0.75f;
        else if (i < r7 + 2)
            vtx->s = f1 * 0.25 + 0.5;
        else if (i < r7 + 4)
            vtx->s = 0.25 + f1 * 0.25;
        else
            vtx->s = 0.25;
    }

    // Calculate something based on vertex positions?
    // The result is never used, so this is pointless.
    if (t >= 0.5)
    {
        int index = faceCount - 4 - r7;
        float f2 = 1.0 - f1;

        vtx = &((struct NlVtxTypeB *)((struct NlDispList *)mesh->dispListStart)->vtxData)[index];

        sp48.x = vtx[0].x * f1 + vtx[2].x * f2;
        sp48.y = vtx[0].y * f1 + vtx[2].y * f2;
        sp48.z = vtx[0].z * f1 + vtx[2].z * f2;

        sp3C.x = vtx[1].x * f1 + vtx[3].x * f2;
        sp3C.y = vtx[1].y * f1 + vtx[3].y * f2;
        sp3C.z = vtx[1].z * f1 + vtx[3].z * f2;

        sp94.x = 0.5 * (sp48.x + sp3C.x);
        sp94.y = 0.5 * (sp48.y + sp3C.y);
        sp94.z = 0.001 + 0.5 * (sp48.z + sp3C.z);
    }

    // WTF??
    mesh = (void *)((u32 *)mesh + (((s32)mesh->dispListSize >> 2) + 0x14));

    faceCount = ((struct NlDispList *)mesh->dispListStart)->faceCount;

    f4 = t * 2.0;
    f4 = CLAMP(f4, 0.0, 1.0);

    f3 = f4 * (faceCount - 2.0);
    r7 = mathutil_floor(f3 * 0.5) * 2.0f;
    f1 = (f3 - r7) * 0.5;

    vtx = (void *)((struct NlDispList *)mesh->dispListStart)->vtxData;
    for (i = faceCount - 1; i >= 0; i--, vtx++)
    {
        if (t > 0.5)
            vtx->s = 0.75;
        else if (i < r7)
            vtx->s = 0.75;
        else if (i < r7 + 2)
            vtx->s = 0.5 + f1 * 0.25;
        else if (i < r7 + 4)
            vtx->s = 0.25 + f1 * 0.25;
        else
            vtx->s = 0.25;
    }

    // Calculate something based on vertex positions?
    // The result is never used, so this is pointless.
    if (t < 0.5)
    {
        int index = faceCount - 4 - r7;
        float f2 = 1.0 - f1;

        vtx = &((struct NlVtxTypeB *)((struct NlDispList *)mesh->dispListStart)->vtxData)[index];

        sp30.x = vtx[0].x * f1 + vtx[2].x * f2;
        sp30.y = vtx[0].y * f1 + vtx[2].y * f2;
        sp30.z = vtx[0].z * f1 + vtx[2].z * f2;

        sp24.x = vtx[1].x * f1 + vtx[3].x * f2;
        sp24.y = vtx[1].y * f1 + vtx[3].y * f2;
        sp24.z = vtx[1].z * f1 + vtx[3].z * f2;

        sp94.x = 0.5 * (sp30.x + sp24.x);
        sp94.y = 0.5 * (sp30.y + sp24.y);
        sp94.z = 0.001 + 0.5 * (sp30.z + sp24.z);
    }
*/
    switch (polyDisp.unk4C)
    {
    case 0:
        if (!(mode_info.ball_mode & INFO_FLAG_TIMER_PAUSED))
        {
            polyDisp.unk4C = 1;
            polyDisp.unk60 = 0.125f;
            polyDisp.unk58 = -((rand() & 0x3FF) + 0x400);
        }
        break;
    case 1:
        polyDisp.unk60 -= 0.0083333333333333332;
        polyDisp.unk5C += polyDisp.unk60;
        if (polyDisp.unk5C < 1.0)
        {
            polyDisp.unk5C = 1.0f;
            polyDisp.unk60 = 0.0f;
            polyDisp.unk4C = 2;
        }
        break;
    case 2:
        if (mode_info.ball_mode & INFO_FLAG_TIMER_PAUSED)
            polyDisp.unk4C = 3;
        break;
    case 3:
        polyDisp.unk4C = 4;
        break;
    case 4:
        polyDisp.unk4C = 0;
        break;
    }
    if (mode_info.ball_mode & INFO_FLAG_TIMER_PAUSED)
        polyDisp.unk58 -= (polyDisp.unk58 >> 3);
    else if (t > 0.5)
        polyDisp.unk58 += (-768 - polyDisp.unk58) >> 4;
    else
        polyDisp.unk58 += (-1536 - polyDisp.unk58) >> 4;
    if (!(g_some_other_flags & 0xA))
        polyDisp.unk54 += polyDisp.unk58;

    //nlObjPutSetFadeColorBase(1.0f, 1.0f, 1.0f);

    avdisp_set_post_mult_color(1.0f, t, 0.0f, 1.0f);
    mtxa_from_translate_xyz(0.0f, (1.0 - t) - 0.5, 0.0f);
    //avdisp_set_custom_tex_mtx(0, mtxa);
		mkb::memcpy(avdisp_tex_mtx, mtxa, sizeof(Mtx));

    // Draw new bomb fuse
    mtxa_from_identity();
    mtxa_translate_xyz(0.00094f, 0.00519f, -0.01f);
    scale = 0.0007f;
    mtxa_scale_s(scale);
    //u_gxutil_upload_some_mtx(mtxa, 0);
		GXLoadPosMtxImm(*mtxa, 0);
		GXLoadNrmMtxImm(*mtxa, 0);
    //avdisp_set_bound_sphere_scale(scale);
    //avdisp_enable_custom_tex_mtx(1);
		//g_avdisp_func8(1);
    //avdisp_draw_model_unculled_sort_translucent(init_common_gma->modelEntries[2].model);
		avdisp_draw_model_unculled_sort_auto(customGma->model_entries[0].model);
    //avdisp_enable_custom_tex_mtx(0);
		//g_avdisp_func8(0);

    // Draw spark
    sparkPos.x = evaluate_stagedef_keyframe((1.0 - t) * 100.0, ARRAY_COUNT(bombSparkXKeyframes), bombSparkXKeyframes);
    sparkPos.y = evaluate_stagedef_keyframe((1.0 - t) * 100.0, ARRAY_COUNT(bombSparkYKeyframes), bombSparkYKeyframes);
    sparkPos.z = 0.141f;
    mtxa_translate(&sparkPos);
    mtxa_sq_from_identity();
    mtxa_rotate_z(polyDisp.unk54);
    mtxa_scale_s(0.0149f);
    mtxa_scale_xyz(polyDisp.unk5C, polyDisp.unk5C, polyDisp.unk5C);
    //nlObjPutImm(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_TIMER_FIRE));
		GXLoadPosMtxImm(*mtxa, 0);
		GXLoadNrmMtxImm(*mtxa, 0);
		avdisp_set_post_mult_color(1, 1, 1, 1);
		avdisp_draw_model_unculled_sort_never(init_common_gma->model_entries[88].model);
	//fade_color_base_default();
}

static patch::Tramp<decltype(&mkb::create_timer_sprites)> s_timerSpiteTramp;
static patch::Tramp<decltype(&mkb::polydisp_main)> s_bombFuseTramp;

static void polydisp_main_override(void)
{
	s_bombFuseTramp.dest();
	//if (spriteClassMask & 4)
	if (get_sprite_with_unique_id(2))
		draw_timer_bomb_fuse();
}

void init_main_loop()
{
	// TODO: I would like to load stuff here, at the beginning, but for some reason, the custom GMA and
	// TPL get overwritten with other stuff and cause the game to crash.
	//load_smb1_common_assets();
	reset_spark_vars();
	patch::hook_function(s_timerSpiteTramp, mkb::create_timer_sprites, show_smb1_timer);
	patch::hook_function(s_bombFuseTramp, mkb::polydisp_main, polydisp_main_override);
}

TICKABLE_DEFINITION((
        .name = "smb1-timer",
        .description = "Classic bomb timer from SMB1",
        .init_main_loop = init_main_loop, ))

}  // namespace smb1_timer
