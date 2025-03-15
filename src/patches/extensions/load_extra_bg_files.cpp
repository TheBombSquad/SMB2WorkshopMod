#include "load_extra_bg_files.h"

#include "internal/patch.h"
#include "internal/tickable.h"
#include "utils/ppcutil.h"

namespace load_extra_bg_files {

TICKABLE_DEFINITION((
        .name = "load-extra-bg-files",
        .description = "Extra BG file loading",
        .init_main_loop = init_main_loop, ))

void init_main_loop() {
    mkb::g_bg_filename_list[1] = "bg_lav";
    mkb::g_bg_filename_list[12] = "bg_bil";
    mkb::g_bg_filename_list[30] = "bg_bil2";
    mkb::g_bg_filename_list[31] = "bg_boa2";
    mkb::g_bg_filename_list[34] = "bg_au_pot2";
    mkb::g_bg_filename_list[41] = "bg_au_bow2";
}

}// namespace load_extra_bg_files
