#pragma once

#include "etl/vector.h"
#include "etl/memory.h"
#include "etl/optional.h"

// TODO: Use preprocessor stuff to figure this out on compile time
constexpr size_t PATCH_COUNT = 32;

// Helper for constructing Tickable with designated initializer lists
// Makes the ctor look a little nicer with all the optional elements they can have
struct TickableArgs {
    const char* name;
    const char* description;
    bool enabled = false;
    etl::optional<int> lower_bound;
    etl::optional<int> upper_bound;
    void (*init_main_loop)() = nullptr;
    void (*init_main_game)() = nullptr;
    void (*init_sel_ngc)() = nullptr;
    void (*disp)() = nullptr;
    void (*tick)() = nullptr;
};

struct Tickable {
    Tickable(TickableArgs args);
    const char* name;
    const char* description;
    bool enabled;
    etl::optional<int> lower_bound;
    etl::optional<int> upper_bound;
    void (*init_main_loop)() = nullptr;
    void (*init_main_game)() = nullptr;
    void (*init_sel_ngc)() = nullptr;
    void (*disp)() = nullptr;
    void (*tick)() = nullptr;
};

class TickableManager {
    typedef etl::vector<etl::unique_ptr<Tickable>, PATCH_COUNT> TickableVec;
    TickableVec m_tickables;

public:
    const auto* const get_tickables() const;
    void push(Tickable* tickable);
    void init() const;
};


