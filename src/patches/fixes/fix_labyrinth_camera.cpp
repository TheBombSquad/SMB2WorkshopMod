#include "fix_labyrinth_camera.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace fix_labyrinth_camera {

TICKABLE_DEFINITION((
        .name = "fix-labyrinth-camera",
        .description = "[wsmod]  Labyrinth stage slot fix %s\n",
        .init_main_loop = init_main_loop, ))

// Always compare the stage ID to 0xFFFF when these camera functions check
// if the current stage ID is 0x15a when determining specific constants.
// 0x2c00ffff = cmpwi r0. 0xFFFF
void init_main_loop() {
    patch::write_word(reinterpret_cast<void*>(0x802858D4), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x802874BC), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x8028751C), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x802880EC), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x802881D4), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x802883B4), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x802886B8), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x8028BF44), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x8028C1CC), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x8028C650), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x8028CA84), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291338), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291420), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291664), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291904), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291930), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291960), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x8029198C), 0x2c00ffff);
    patch::write_word(reinterpret_cast<void*>(0x80291AEC), 0x2c00ffff);
}

}// namespace fix_labyrinth_camera
