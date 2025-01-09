#include "fix_view_stage.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace fix_view_stage {

TICKABLE_DEFINITION((
        .name = "fix-view-stage",
        .description = "Various 'View Stage' fixes",
        .init_main_game = init_main_game, ))

void new_render_func(void) {
    float fVar1;
    bool bVar4;
    mkb::uint uVar2;
    mkb::GmaModel* pGVar3;
    int* piVar5;
    mkb::StagedefStageModelPtrB* pSVar6;
    int iVar7;
    mkb::StagedefStageModelInstance* pSVar8;
    int iVar9;
    mkb::StagedefColiHeader* pSVar10;
    mkb::Itemgroup* pIVar11;
    double dVar12;
    mkb::g_set_some_draw_values(1.0, 1.0, 1.0);
    mkb::mtxa_from_mtxb();
    mkb::mtxa_translate(&mkb::stagedef->start->position);
    mkb::mtxa_rotate_y((s16) ((int) mkb::view_stage_timer << 9));
    mkb::load_gx_pos_nrm_mtx(mkb::mtxa, 0);
    mkb::avdisp_draw_model_culled_sort_auto(mkb::init_common_gma->model_entries[0x4d].model);
    mkb::g_some_draw_func();
    if (mkb::stage_gma != (mkb::GmaBuffer*) 0x0) {
        pSVar10 = mkb::stagedef->coli_header_list;
        pIVar11 = mkb::itemgroups;
        for (iVar9 = 0; iVar9 < (int) mkb::stagedef->coli_header_count; iVar9 = iVar9 + 1) {
            mkb::mtxa_from_mtxb();
            if (0 < iVar9) {
                mkb::mtxa_mult_right((mkb::Mtx*) pIVar11->transform);
            }
            mkb::load_gx_pos_nrm_mtx(mkb::mtxa, 0);
            pSVar6 = pSVar10->stage_model_b_list;
            for (iVar7 = 0; iVar7 < (int) pSVar10->stage_model_b_count; iVar7 = iVar7 + 1) {
                if (((pSVar6->stage_model_a->some_effect_bitflag & mkb::EFFECT_TRANSPARENCY_A) == mkb::EFFECT_NULL) && (pGVar3 = (mkb::GmaModel*) mkb::g_some_draw_func2((int) pSVar6->stage_model_a),
                                                                                                                        pGVar3 != (mkb::GmaModel*) 0x0)) {
                    if (pSVar10->texture_scroll != (mkb::StagedefTextureScroll*) 0x0) {
                        mkb::g_something_with_texture_scroll(pSVar10->texture_scroll);
                    }
                    if ((pSVar6->stage_model_a->some_effect_bitflag & mkb::EFFECT_TRANSPARENCY_B) == mkb::EFFECT_NULL) {
                        mkb::avdisp_draw_model_culled_sort_never(pGVar3);
                    }
                    else {
                        mkb::g_smth_with_rendefc_reflective_height(400.0);
                        mkb::avdisp_draw_model_unculled_sort_always(pGVar3);
                        mkb::g_smth_with_rendefc_reflective_height(0.0);
                    }
                }
                pSVar6 = pSVar6 + 1;
            }
            uVar2 = mkb::g_smth_with_challenge_mode_var_and_3p();
            pSVar8 = mkb::stagedef->coli_header_list[iVar9].stage_model_instance_list;
            mkb::mtxa_push();
            for (iVar7 = 0; iVar7 < (int) mkb::stagedef->coli_header_list[iVar9].stage_model_instance_count;
                 iVar7 = iVar7 + 1) {
                if (((mkb::ushort) pSVar8->g_not_padding & uVar2) == 0) {
                    mkb::mtxa_peek();
                    mkb::mtxa_translate(&pSVar8->position);
                    mkb::mtxa_rotate_z((pSVar8->rotation).z);
                    mkb::mtxa_rotate_y((pSVar8->rotation).y);
                    mkb::mtxa_rotate_x((pSVar8->rotation).x);
                    mkb::mtxa_scale(&pSVar8->scale);
                    if ((pSVar8->scale).x <= (pSVar8->scale).y) {
                        fVar1 = (pSVar8->scale).y;
                    }
                    else {
                        fVar1 = (pSVar8->scale).x;
                    }
                    dVar12 = (double) fVar1;
                    if (dVar12 < (double) (pSVar8->scale).z) {
                        dVar12 = (double) (pSVar8->scale).z;
                    }
                    mkb::g_some_draw_func3(dVar12);
                    pGVar3 = (mkb::GmaModel*) mkb::g_some_draw_func4(dVar12, (int) pSVar8->stage_model_a);
                    if ((pGVar3 != (mkb::GmaModel*) 0x0) &&
                        (bVar4 = mkb::g_is_sphere_visible((double) (float) ((double) pGVar3->bound_sphere_radius *
                                                                            dVar12),
                                                          &pGVar3->bound_sphere_center),
                         bVar4)) {
                        mkb::load_gx_pos_nrm_mtx(mkb::mtxa, 0);
                        if ((pSVar6->stage_model_a->some_effect_bitflag & mkb::EFFECT_TRANSPARENCY_B) == mkb::EFFECT_NULL)
                            mkb::avdisp_draw_model_unculled_sort_never(pGVar3);
                    }
                }
                pSVar8 = pSVar8 + 1;
            }
            mkb::mtxa_pop();
            pIVar11 = pIVar11 + 1;
            pSVar10 = pSVar10 + 1;
        }
    }
    if (mkb::g_some_draw_var != (int*) 0x0) {
        mkb::mtxa_from_mtxb();
        for (piVar5 = mkb::g_some_draw_var; *piVar5 != 0; piVar5 = piVar5 + 6) {
            mkb::g_some_draw_func5((int*) piVar5[5]);
        }
    }
    mkb::g_draw_collision_triangles();
    return;
}

void init_main_game() {
    // Prevent background animations from animating twice as fast in 'View Stage'
    patch::write_nop(reinterpret_cast<void*>(0x80912d90));
    patch::write_branch(
        reinterpret_cast<void*>(mkb::g_smth_with_rendering_models_for_reflective_surfaces),
        reinterpret_cast<void*>(new_render_func));
}

}// namespace fix_view_stage
