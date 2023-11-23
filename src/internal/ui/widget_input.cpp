#include "widget_input.h"

namespace ui {

void Input::dispatch_callback() {
    if (m_play_sound_effect) mkb::call_SoundReqID_arg_2(m_sound_effect_id);
    m_callback();
}
void Input::tick() {
    if (!m_repeating) {
        bool has_pressed_input = (m_type == BUTTON) ? pad::button_pressed(m_input) : pad::dir_pressed(m_direction);
        if (has_pressed_input) {
            dispatch_callback();
        }
    }

    else {
        bool has_down_input = (m_type == BUTTON) ? pad::button_down(m_input) : pad::dir_down(m_direction);
        if (has_down_input) {
            if (m_frame_timer == 0) {
                dispatch_callback();
            }
            else if (m_frame_timer >= m_initial_delay && m_frame_timer % m_repeat_delay == 0) {
                dispatch_callback();
            }
            m_frame_timer++;
        }
        else {
            m_frame_timer = 0;
        }
    }

    Widget::tick();
}
}