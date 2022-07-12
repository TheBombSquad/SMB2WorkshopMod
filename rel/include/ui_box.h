#pragma once

#include <mkb.h>

namespace ui_box {

    static constexpr u8 UI_BOX_LEN = 16;

    class UIBox {

    enum class AnimType {
        ANIM_NONE,
        ANIM_WIGGLE,
    };

    public:
        UIBox(float x, float y, float width, float height);
        void disp();
        void tick(AnimType type);

    private:
        Vec2d m_pos;
        Vec2d m_dimensions;
        s32 m_rot_z = 0;
        char* m_title = nullptr;
        char* m_subtitle = nullptr;
        char* m_msg = nullptr;
        s32 m_counter_1 = 0;
        s32 m_counter_2 = 0;
        s32 m_counter_3 = 0;
        AnimType m_anim_type_1 = AnimType::ANIM_NONE;
        AnimType m_anim_type_2 = AnimType::ANIM_NONE;
        AnimType m_anim_type_3 = AnimType::ANIM_NONE;

        void anim_wiggle();
    };

    extern UIBox* ui_box_list[UI_BOX_LEN];
    extern u8 ui_box_count;

    void init();
    void disp();

    void push(UIBox* box);
    void pop();

    void draw_ui_box_ext(u32 texture_id);
}

