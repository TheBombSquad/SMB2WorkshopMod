#pragma once

#include <mkb.h>

namespace menu
{

enum class WidgetType
{
    Text,
    Header,
    Checkbox,
    Separator,
    Menu,
    FloatView,
};

struct TextWidget
{
    char *label;
};

// Just a different color TextWidget
struct HeaderWidget
{
    char *label;
};

struct CheckboxWidget
{
    char *label;
    // We can't use std::function due to destructors in unions stuff
    bool (*get)();
    void (*set)(bool value);
};

struct MenuWidget
{
    char *label;
    struct Widget *widgets;
    u32 num_widgets;
};

struct FloatViewWidget
{
    char *label;
    f32 (*get)();
};

struct Widget
{
    WidgetType type;
    union
    {
        TextWidget text;
        HeaderWidget header;
        CheckboxWidget checkbox;
        MenuWidget menu;
        FloatViewWidget float_view;
    };
};

extern MenuWidget root_menu;

}
