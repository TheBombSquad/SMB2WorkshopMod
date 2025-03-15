#include "goal_texture_scroll.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace goal_texture_scroll {

TICKABLE_DEFINITION((.name = "goal-texture-scroll",
                     .description = "Goal texture scroll",
                     .init_main_loop = init_main_loop))

void new_draw_goalpost_models() {
    int cond;
    bool bVar1;
    int iVar3;
    mkb::StagedefGoal* stagedef_goal;
    mkb::GmaModel* goal_model;
    int coli_header_idx;
    mkb::StagedefColiHeader* coli_header;
    mkb::Itemgroup* itemgroup;
    mkb::StagedefGoal* local_68;
    mkb::undefined2 local_64;
    mkb::undefined2 local_62;
    mkb::GmaModel* local_60;
    mkb::Mtx mtx;

    local_64 = 0x40;
    local_62 = 4;
    cond = mkb::g_check_some_condition(0x40);
    coli_header = mkb::stagedef->coli_header_list;
    itemgroup = mkb::itemgroups;
    for (coli_header_idx = 0; coli_header_idx < (int) mkb::stagedef->coli_header_count;
         coli_header_idx = coli_header_idx + 1) {
        if (coli_header->goal_count != 0) {
            mkb::mtxa_from_mtxb();
            if (0 < coli_header_idx) {
                mkb::mtxa_mult_right(&itemgroup->transform);
            }
            mkb::mtxa_to_mtx(&mtx);
            stagedef_goal = coli_header->goal_list;
            for (iVar3 = 0; iVar3 < (int) coli_header->goal_count; iVar3 = iVar3 + 1) {
                mkb::mtxa_from_mtx(&mtx);
                mkb::mtxa_translate(&stagedef_goal->position);
                mkb::mtxa_rotate_z((stagedef_goal->rotation).z);
                mkb::mtxa_rotate_y((stagedef_goal->rotation).y);
                mkb::mtxa_rotate_x((stagedef_goal->rotation).x);
                goal_model = mkb::goal_gma_models[(char) stagedef_goal->type];
                // Scroll the texture if the IG has texture scroll parameters set
                if (coli_header->texture_scroll != (mkb::StagedefTextureScroll*) 0x0) {
                    mkb::g_something_with_texture_scroll(coli_header->texture_scroll);
                }
                if ((goal_model != (mkb::GmaModel*) 0x0) &&
                    (bVar1 = mkb::g_is_sphere_visible((double) goal_model->bound_sphere_radius,
                                                      &goal_model->bound_sphere_center),
                     bVar1)) {
                    mkb::load_gx_pos_nrm_mtx(mkb::mtxa, 0);
                    mkb::avdisp_draw_model_unculled_sort_auto(goal_model);
                    local_60 = goal_model;
                    /* This is meant to allow goals to receive shadows but is crashing at the moment
                    if (cond != 0) {
                        local_68 = stagedef_goal;
                        mkb::g_some_shadow_draw_func(&local_68);
                    }
                    */
                }
                stagedef_goal = stagedef_goal + 1;
            }
        }
        itemgroup = itemgroup + 1;
        coli_header = coli_header + 1;
    }
    return;
}

void init_main_loop() {
    patch::write_branch(
        reinterpret_cast<void*>(mkb::g_draw_goalpost_models),
        reinterpret_cast<void*>(new_draw_goalpost_models));
}

}// namespace goal_texture_scroll