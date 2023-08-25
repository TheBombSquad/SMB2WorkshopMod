#pragma once

#include "etl/memory.h"
#include "etl/optional.h"
#include "etl/vector.h"
#include "mkb.h"

#define _TOKEN_CONCAT(x, y) x##y
#define TOKEN_CONCAT(x, y) _TOKEN_CONCAT(x, y)
#define UNPAREN(...) __VA_ARGS__

#define _TICKABLE_DEFINITION(in, idx)                                                                                    \
    tickable::Tickable* TOKEN_CONCAT(active_tickable_ptr, idx) = []() {                                                  \
        static uint8_t TOKEN_CONCAT(s_tickable_buf, idx)[sizeof(tickable::Tickable)];                                    \
        tickable::Tickable* t = new (TOKEN_CONCAT(s_tickable_buf, idx)) tickable::Tickable{                              \
            in};                                                                                                         \
        tickable::get_tickable_manager().push(reinterpret_cast<tickable::Tickable*>(TOKEN_CONCAT(s_tickable_buf, idx))); \
        return t;                                                                                                        \
    }();

#define TICKABLE_DEFINITION_IDX(in) _TICKABLE_DEFINITION(UNPAREN in, TOKEN_CONCAT(_, __COUNTER__))

// Defines a new tickable, accessible by active_tickable_ptr
// Adds the new tickable to the tickable manager
#define TICKABLE_DEFINITION(in) _TICKABLE_DEFINITION(UNPAREN in, )// NOLINT

namespace tickable {

// Capacity of the tickable manager vector, increase if needed
// This only stores pointers, so memory impact should be low
constexpr size_t PATCH_CAPACITY = 32;

// Represents a patch, or code that ticks every frame
struct Tickable {
    const char* name = nullptr;
    const char* description = nullptr;
    bool enabled = false;
    etl::optional<int> active_value;
    etl::optional<int> lower_bound;
    etl::optional<int> upper_bound;
    void (*init_main_loop)() = nullptr;
    void (*init_main_game)() = nullptr;
    void (*init_sel_ngc)() = nullptr;
    void (*disp)() = nullptr;
    void (*tick)() = nullptr;
};

// Manages all tickables
class TickableManager {
public:
    typedef etl::vector<etl::unique_ptr<Tickable>, PATCH_CAPACITY> TickableVec;
    const TickableVec& get_tickables() const;
    bool get_tickable_status(const char* name) const;
    void push(Tickable* tickable);
    void init() const;

private:
    TickableVec m_tickables;
};

TickableManager& get_tickable_manager();

}// namespace tickable