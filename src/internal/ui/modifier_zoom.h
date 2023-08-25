#pragma once

#include "internal/log.h"
#include "internal/ui/modifier.h"
#include "internal/ui/widget.h"
#include "mkb/mkb.h"

namespace ui {

enum class ZoomType {
    ZOOM_IN,
    ZOOM_OUT,
};
class ModifierZoom : public Modifier {
    u32 m_delay_frames;
    u32 m_duration;
    float m_amplitude;
    float m_offset;

    ModifierZoom(u32 delay_frames, u32 duration, float amplitude, float offset, ZoomType zoom_type) {
        m_delay_frames = delay_frames;
        m_duration = duration;
        m_amplitude = (zoom_type == ZoomType::ZOOM_IN) ? 1.0f : -1.0f;
        m_offset = (zoom_type == ZoomType::ZOOM_IN) ? 0.0f : 1.0f;
    };

    void tick(Widget* widget) override {
        MOD_ASSERT_MSG(m_delay_frames >= 0, "Zoom modifier delay cannot be negative");
        MOD_ASSERT_MSG(m_duration > 0, "Zoom modifier duration cannot be negative or zero");

        const auto counter = widget->get_counter();
        if (counter < m_delay_frames) {
            widget->increment_counter();
            return;
        }

        if (counter - m_delay_frames > m_duration) return;

        float scale = m_amplitude *
                          mkb::sin((float) (counter - m_delay_frames) *
                                   (3.14159 / 2.0) *
                                   (1.0f / (float) (m_duration))) +
                      m_offset;

        widget->set_scale(Vec2d{scale, scale});

        widget->increment_counter();
    }
};

}// namespace ui
