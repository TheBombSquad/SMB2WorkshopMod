#include "extended_reflections.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"

namespace extended_reflections {

TICKABLE_DEFINITION((
        .name = "enhance-reflective-surfaces",
        .description = "[wsmod]  Reflective surface enhancements %s\n",
        .init_main_loop = init_main_loop, ))

float nearest_dist_to_mir, distance_to_mirror;
Vec current_ball_position, mirror_origin, ig_init_pos, translation_factor;
mkb::Itemgroup* active_ig;

float get_distance(Vec& vec1, Vec& vec2) {
    float xcmp = (vec1.x - vec2.x) * (vec1.x - vec2.x);
    float ycmp = (vec1.y - vec2.y) * (vec1.y - vec2.y);
    float zcmp = (vec1.z - vec2.z) * (vec1.z - vec2.z);

    return mkb::math_sqrt(xcmp + ycmp + zcmp);
}

// Translates the nearest mirror from its origin to the current translation/rotation of its collision header
void mirror_tick() {
    mkb::Ball* ball = mkb::balls;

    // Determines the nearest reflective surface to the active ball
    for (int idx = 0; idx < 4; idx++) {
        if (ball != nullptr && ball->status == mkb::STAT_NORMAL) {
            for (u32 col_hdr_idx = 0; col_hdr_idx < (mkb::stagedef->coli_header_count); col_hdr_idx++) {
                mkb::StagedefColiHeader* hdr = &mkb::stagedef->coli_header_list[col_hdr_idx];
                for (u32 refl_idx = 0; refl_idx < hdr->reflective_stage_model_count; refl_idx++) {
                    mkb::StagedefReflectiveStageModel* refl = &hdr->reflective_stage_model_list[refl_idx];
                    current_ball_position = ball->pos;
                    distance_to_mirror = get_distance(current_ball_position, refl->g_model_header_ptr->bound_sphere_center);

                    if (nearest_dist_to_mir == -1.0 || distance_to_mirror < nearest_dist_to_mir) {
                        nearest_dist_to_mir = distance_to_mirror;
                        active_ig = &mkb::itemgroups[col_hdr_idx];
                        mirror_origin = refl->g_model_header_ptr->bound_sphere_center;
                        ig_init_pos = hdr->origin;
                    }
                }
            }
            // Translates the mirror plane according to the active animation and rotates it as well
            if (active_ig != nullptr) {
                mkb::mtxa_from_identity();
                translation_factor.x = active_ig->position.x - ig_init_pos.x;
                translation_factor.y = active_ig->position.y - ig_init_pos.y;
                translation_factor.z = active_ig->position.z - ig_init_pos.z;

                mkb::mtxa_translate(&mirror_origin);
                mkb::mtxa_translate(&translation_factor);
                mkb::mtxa_rotate_x(active_ig->rotation.x);
                mkb::mtxa_rotate_y(active_ig->rotation.y);
                mkb::mtxa_rotate_z(active_ig->rotation.z);
            }
        }
        ball++;
    }

    active_ig = nullptr;
    nearest_dist_to_mir = -1.0;
    return;
}

// Hooks into the reflection-handling function, calling our function instead
void init_main_loop() {
    patch::write_branch_bl(reinterpret_cast<void*>(0x8034b270), reinterpret_cast<void*>(mirror_tick));
    patch::write_nop(reinterpret_cast<void*>(0x8034b11c));
    nearest_dist_to_mir = -1.0;
    distance_to_mirror = 0.0;
    active_ig = nullptr;
}
}// namespace extended_reflections
