#include "ui_manager.h"

namespace ui {

WidgetManager& get_widget_manager() {
    static uint8_t s_widget_manager[sizeof(WidgetManager)];
    static bool s_buf_init = false;

    if (!s_buf_init) {
        new (s_widget_manager) WidgetManager();
        s_buf_init = true;
    }
    auto ptr = reinterpret_cast<WidgetManager*>(s_widget_manager);
    return *ptr;
}

}// namespace ui