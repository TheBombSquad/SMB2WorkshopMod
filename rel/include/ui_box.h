#pragma once

#include <mkb.h>
#include "list.h"

namespace ui_box {
    enum class ZoomType {
        ZOOM_IN,
        ZOOM_OUT,
    };

    enum class AnimType {
        MODIFIER_NONE,
        MODIFIER_WIGGLE,
        MODIFIER_ZOOM,
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
        s32 int_param_1 = 0;
        s32 int_param_2 = 0;
        s32 int_param_3 = 0;
        float float_param_1 = 0.0;
        float float_param_2 = 0.0;
        float float_param_3 = 0.0;
        u32 counter = 0;
    };

    class UIBox {

    public:
        UIBox(float x, float y, float width, float height);
        void disp();
        void set_state(const UIBoxState &state);
        void set_wiggle_modifier(const u16 &angle, const float &period);
        void set_zoom_modifier(const float &time, const ZoomType &zoom_type, const float &delay = 0.0f);
        void set_scale(const float &x, const float &y);
        UIBoxState get_state() const;

    private:
        UIBoxState state;
        Vec2d m_pos;
        Vec2d m_dimensions;
        Vec2d m_scale;
        s32 m_rot_z = 0;
        char* title = nullptr;
        char* subtitle = nullptr;
        char* message = nullptr;
        List<UIBoxModifier> modifiers = List<UIBoxModifier>();
        u8 modifier_count;

        void modifier_wiggle(UIBoxModifier* modifier);
        void modifier_zoom(UIBoxModifier* modifier);
    };

    void init();
    void disp_all();

    extern List<UIBox> ui_boxes;
    void draw_ui_box_ext(u32 texture_id);

    float lerp(float f1, float f2, float t);
}

