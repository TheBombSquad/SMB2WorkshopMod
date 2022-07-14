#pragma once

#include <mkb.h>

namespace ui_box {

    // Number of UI boxes that can be rendered at once. Arbitrarily set to 16
    static constexpr u8 UI_BOX_LEN = 16;


    enum class AnimType {
        MODIFIER_NONE,
        MODIFIER_WIGGLE,
    };

    enum class UIBoxState {
        STATE_VISIBLE,
        STATE_VISIBLE_NO_TICK,
        STATE_INVISIBLE,
        STATE_INVISIBLE_NO_TICK,
        STATE_DESTROY,
    };

    struct UIBoxModifier {
        AnimType type = AnimType::MODIFIER_NONE;
        s32 int_param = 0;
        float float_param = 0.0;
        char* str_param_1 = nullptr;
        char* str_param_2 = nullptr;
        char* str_param_3 = nullptr;
        s32 counter;
    };

    class UIBox {

    public:
        UIBox(float x, float y, float width, float height);
        void disp();
        void set_state(UIBoxState state);
        void set_wiggle_attribute(u16 angle);
        UIBoxState getState() const;

        private:
        UIBoxState state;
        Vec2d m_pos;
        Vec2d m_dimensions;
        s32 m_rot_z = 0;
        UIBoxModifier* modifiers[8] = {nullptr};

        void modifier_wiggle(UIBoxModifier* modifier);
    };

    extern UIBox* ui_box_stack[UI_BOX_LEN];
    extern u8 ui_box_count;

    void init();
    void disp_all();

    void push(UIBox* box);
    void pop();

    void draw_ui_box_ext(u32 texture_id);
}

