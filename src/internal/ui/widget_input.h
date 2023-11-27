#pragma once

#include "internal/pad.h"
#include "internal/ui/widget.h"

namespace ui {

enum InputType {
    BUTTON,   // Call the callback when the assigned button is pressed
    DIRECTION,// Call the callback when the assigned direction is pressed
};

// Class for handling input
// Has four different methods of operation:
//     - Button, non-repeating - calls a function when a button is pressed, but only once
//     - Direction, non-repeating - calls a function when a direction is pressed on the analog or D-pad, but only once
//     - Button, repeating - calls a function when a button is pressed,
//       then waits m_initial_delay frames, then calls it again every m_repeat_delay frames
//     - Button, repeating - calls a function when a direction is pressed on the analog or D-pad,
//       then waits m_initial_delay frames, then calls it again every m_repeat_delay frames
// In hindsight, this should just be something that checks if anything is pressed/held at all to allow for chords or multiple direction handling
// And we could just pass all that to a callback
// Well, TODO for another time
class Input : public Widget {
protected:
    InputType m_type;
    mkb::PadDigitalInput m_input;
    pad::Dir m_direction;
    alignas(4) uint16_t m_initial_delay = 1;
    alignas(4) uint16_t m_repeat_delay = 1;
    alignas(4) bool m_repeating = false;
    alignas(4) bool m_play_sound_effect = true;
    uint32_t m_sound_effect_id = 0x6e;
    uint32_t m_frame_timer = 0;
    uint16_t m_debounce_counter = 1;

    void dispatch_callback();

public:
    Input() = delete;
    Input(mkb::PadDigitalInput button, WidgetCallback callback);
    Input(pad::Dir direction, WidgetCallback callback);
    ~Input();
    virtual void tick() override;
    virtual void disp() override{};

    mkb::PadDigitalInput get_input() const {
        return m_input;
    }
    void set_input(mkb::PadDigitalInput mInput) {
        m_input = mInput;
    }
    pad::Dir get_direction() const {
        return m_direction;
    }
    void set_direction(pad::Dir mDirection) {
        m_direction = mDirection;
    }
    uint16_t get_initial_delay() const {
        return m_initial_delay;
    }
    void set_initial_delay(uint16_t initial_delay) {
        m_initial_delay = initial_delay;
    }
    uint16_t get_repeat_delay() const {
        return m_repeat_delay;
    }
    void set_repeat_delay(uint16_t repeat_delay) {
        m_repeat_delay = repeat_delay;
    }
    bool is_repeating() const {
        return m_repeating;
    }
    void set_repeating(bool repeat) {
        m_repeating = repeat;
    }
    InputType get_type() const {
        return m_type;
    }
    void set_type(InputType type) {
        m_type = type;
    }
    bool has_sound_effect() const {
        return m_play_sound_effect;
    }
    void set_sound_effect(bool sound_effect) {
        m_play_sound_effect = sound_effect;
    }
    uint32_t get_sound_effect_id() const {
        return m_sound_effect_id;
    }
    void set_sound_effect_id(uint32_t sound_effect_id) {
        m_sound_effect_id = sound_effect_id;
    }
};
}// namespace ui
