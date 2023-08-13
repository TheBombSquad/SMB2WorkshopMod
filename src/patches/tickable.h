#pragma once

#include "etl/memory.h"
#include "etl/optional.h"
#include "etl/vector.h"
#include "mkb.h"

namespace tickable {
// TODO: Use preprocessor stuff to figure this out on compile time
constexpr size_t PATCH_COUNT = 32;

struct Tickable {
    const char* name;
    const char* description;
    bool enabled;
    etl::optional<int> default_value;
    etl::optional<int> lower_bound;
    etl::optional<int> upper_bound;
    void (*init_main_loop)() = nullptr;
    void (*init_main_game)() = nullptr;
    void (*init_sel_ngc)() = nullptr;
    void (*disp)() = nullptr;
    void (*tick)() = nullptr;
};

class TickableManager {
public:
    typedef etl::vector<etl::unique_ptr<Tickable>, PATCH_COUNT> TickableVec;
    const TickableVec& get_tickables() const;
    void push(Tickable* tickable);
    void init() const;

private:
    TickableVec m_tickables;
};

TickableManager& get_tickable_manager();

}// namespace tickable