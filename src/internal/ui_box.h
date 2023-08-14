#pragma once

#include "etl/list.h"
#include "mkb/mkb.h"

namespace ui_box {
enum class ZoomType {
    ZOOM_IN,
    ZOOM_OUT,
};

enum class AnimType {
    MODIFIER_NONE,
    MODIFIER_WIGGLE,
    MODIFIER_ZOOM,
    MODIFIER_LIFETIME,
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
    void set_state(const UIBoxState& state);
    void set_wiggle_modifier(const u16& angle, const float& period);
    void set_zoom_modifier(const u32& time, const ZoomType& zoom_type, const u32& delay = 0);
    void set_lifetime_modifier(const u32& time);
    void set_scale(const float& x, const float& y);
    void set_title(char* title);
    void set_subtitle(char* subtitle);
    void set_message(char* message);
    UIBoxState get_state() const;


private:
    UIBoxState state;
    Vec2d m_pos;
    Vec2d m_dimensions;
    Vec2d m_scale;
    s32 m_rot_z = 0;
    char* m_title = nullptr;
    char* m_subtitle = nullptr;
    char* m_message = nullptr;
    etl::list<UIBoxModifier, 4> modifiers = etl::list<UIBoxModifier, 4>();
    u8 modifier_count;

    void modifier_wiggle(UIBoxModifier* modifier);
    void modifier_zoom(UIBoxModifier* modifier);
    void modifier_lifetime(UIBoxModifier* modifier);
};

void init();
void disp_all();

extern etl::list<UIBox, 16> ui_boxes;
void draw_ui_box_ext(u32 texture_id);

float lerp(float f1, float f2, float t);
}// namespace ui_box
