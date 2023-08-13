#include "smb1_camera_toggle.h"

#include "internal/pad.h"
#include "internal/patch.h"
#include "internal/tickable.h"
#include "mkb/mkb.h"
#include "utils/ppcutil.h"

namespace smb1_camera_toggle {

TICKABLE_DEFINITION((
        .name = "smb1-camera-toggle",
        .description = "[wsmod]  SMB1 camera toggle %s\n",
        .init_main_loop = init_main_loop,
        .tick = tick, ))

static bool smb1_cam_toggled;

void init_main_loop() {
    smb1_cam_toggled = false;
}

// The write_word statement moves the camera's angle down by 2.8 degrees to match SMB1's angle
// Everything else brings the camera position/pivot values in-line with SMB1's values
// Camera mode 1 enables SMB1-like vertical camera tracking, camera mode 0x4c is SMB2's default
void tick() {
    if (mkb::main_mode == mkb::MD_GAME &&
        (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN)) {
        if (smb1_cam_toggled) {
            if (mkb::cameras[0].mode == 0x4c) mkb::cameras[0].mode = 1;
            patch::write_word(reinterpret_cast<void*>(0x802886c8), PPC_INSTR_LI(PPC_R0, 0x400));
            mkb::g_camera_turn_rate_scale = 0.6875;
            mkb::camera_pivot_height = -0.5;
            mkb::camera_height = 1;
        }
        else {
            if (mkb::cameras[0].mode == 0x1) mkb::cameras[0].mode = 0x4c;
            patch::write_word(reinterpret_cast<void*>(0x802886c8), PPC_INSTR_LI(PPC_R0, 0x200));
            mkb::g_camera_turn_rate_scale = 0.75;
            mkb::camera_pivot_height = 0.18;
            mkb::camera_height = 0.8;
        }
    }

    if (pad::button_pressed(mkb::PAD_TRIGGER_Z)) {
        smb1_cam_toggled = !smb1_cam_toggled;
    }
}
}// namespace smb1_camera_toggle
