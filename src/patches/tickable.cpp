#include "tickable.h"
#include "internal/patch.h"

#include "etl/iterator.h"

#define STREQ(x, y) (mkb::strcmp(const_cast<char*>(x), const_cast<char*>(y)) == 0)
static TickableManager mgr;

Tickable::Tickable(TickableArgs args) {
    mgr.push(this);
}

void TickableManager::push(Tickable* tickable) {
    auto tick_ptr = etl::unique_ptr<Tickable>(tickable);
    m_tickables.push_back(std::move(tick_ptr));
}

const auto* const TickableManager::get_tickables() const {
    return &m_tickables;
}

void TickableManager::init() const {
    static patch::Tramp<decltype(&mkb::draw_debugtext)> s_draw_debugtext_tramp;
    static patch::Tramp<decltype(&mkb::load_additional_rel)> s_load_additional_rel_tramp;

    // Hook for mkb::draw_debugtext
    patch::hook_function(s_draw_debugtext_tramp, mkb::draw_debugtext, []() {
        // Drawing hook for UI elements.
        // Gets run at the start of smb2's function which draws debug text windows,
        // which is called at the end of smb2's function which draws the UI in general.

        // Disp functions (REL patches)
        for (const auto& tickable: *mgr.get_tickables()) {
            if (tickable->enabled) {
                tickable->disp();
            }
        }
        s_draw_debugtext_tramp.dest();
    });

    // Hook for mkb::load_additional_rel
    patch::hook_function(
        s_load_additional_rel_tramp,
        mkb::load_additional_rel, [](char* rel_filepath, mkb::RelBufferInfo* rel_buffer_ptrs) {
            s_load_additional_rel_tramp.dest(rel_filepath, rel_buffer_ptrs);

            // Functions that need to be initialized when mkb2.main_game.rel is loaded
            if (STREQ(rel_filepath, "mkb2.main_game.rel")) {
                for (const auto& tickable: *mgr.get_tickables()) {
                    if (tickable->enabled) {
                        tickable->init_main_game();
                    }
                }
            }

            // Functions that need to be initialized when mkb2.sel_ngc.rel is loaded
            else if (STREQ(rel_filepath, "mkb2.sel_ngc.rel")) {
                for (const auto& tickable: *mgr.get_tickables()) {
                    if (tickable->enabled) {
                        tickable->init_sel_ngc();
                    }
                }
            }
        });
}