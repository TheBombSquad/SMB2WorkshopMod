#pragma once

#include <mkb.h>

namespace ui_box {

    static constexpr u8 UI_BOX_LEN = 16;

    class UIBox {

    public:
        UIBox(float x, float y, float width, float height);
        void disp();

        Vec2d m_pos;
        Vec2d m_dimensions;
        s32 m_rot_z = 0;
        char* m_title = nullptr;
        char* m_subtitle = nullptr;
        char* m_msg = nullptr;
        s32 counter_1 = 0;
        s32 counter_2 = 0;
        s32 counter_3 = 0;
        void(*m_tick_1)(UIBox* b) = nullptr;
        void(*m_tick_2)() = nullptr;
        void(*m_tick_3)() = nullptr;
    private:
    };

    extern UIBox* ui_box_list[UI_BOX_LEN];
    extern u8 ui_box_count;

    void init();
    void disp();

    void push(UIBox* box);
    void pop();

    void wiggle_box(UIBox* box);
}

