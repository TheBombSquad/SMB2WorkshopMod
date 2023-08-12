#include "death_counter.h"

#include "internal/patch.h"

namespace death_counter {

static Tickable t = Tickable((TickableArgs){
    .name = "challenge-mode-death-count",
    .description = "Challenge mode death count",
    .init_main_game = init_main_game,
});

// Death count for each of the four players
static u32 death_count[4];

// Clears the per-player death counter, then nops the instruction that
// decrements the life counter on player death. Then, hooks into the
// decrement life counter function, and runs the update_death_count func.
// Then, hooks into the monkey counter sprite tick function, and calls
// the death counter sprite tick function instead.'
void init_main_game() {
    mkb::memset(death_count, 0, sizeof(death_count));

    patch::write_nop(reinterpret_cast<void*>(0x808fa4f4));

    patch::write_branch_bl(reinterpret_cast<void*>(0x808fa560), reinterpret_cast<void*>(update_death_count));
    patch::write_branch(reinterpret_cast<void*>(mkb::sprite_monkey_counter_tick),
                        reinterpret_cast<void*>(death_counter_sprite_tick));
}

// Increments the death counter of the current player on death.
// This hooks into the life counter decrement function, so it is
// only called in situations when the player would actually lose a life.
// Returns 99, since the result of this function, normally, is the number
// of lives of the player, which determines whether or not
// the game should proceed to the continue/game over screen or not.
u32 update_death_count() {
    mkb::Ball* ball = mkb::balls;
    for (int idx = 0; idx < 4; idx++) {
        if (ball->status == mkb::STAT_NORMAL) {
            death_count[idx]++;
        }
        ball++;
    }
    return 99;
}

// Assigns our own parameters to the life counter sprite for the purposes
// of counting deaths. Green when no deaths have occured, scales the sprite
// so numbers don't go off-screen or get obstructed.
void death_counter_sprite_tick(u8* status, mkb::Sprite* sprite) {
    u32 display = death_count[mkb::curr_player_idx];

    if (display == 0) {
        sprite->mult_color.blue = 0;
    }
    else {
        sprite->mult_color.blue = 0xff;
    }

    if (display > 9999) {
        sprite->width = 0.3;
    }
    else if (display > 999) {
        sprite->width = 0.4;
    }
    else if (display > 99) {
        sprite->width = 0.5;
    }
    else if (display > 9) {
        sprite->width = 0.6;
    }
    else {
        sprite->width = 1;
    }

    mkb::sprintf(sprite->text, "%u", display);
}
}// namespace challenge_death_count
