#include "scratch.h"
#include "mkb/mkb.h"
#include "pad.h"
#include "patch.h"

namespace scratch
{

    static bool play = false;
    static bool is_alt_frame = false;
    static int value_file_length;
    static gc::DVDFileInfo value_file_info;
    static char* value_file_buf;
    static char value_file_path[] = "/values.txt";
    static char bad_apple[0x30000];
    static mkb::Item* first_banana = nullptr;
    static int frame_counter = 0;
    static constexpr u32 WIDTH = 16;
    static constexpr u32 HEIGHT = 12;

    void init() {
        value_file_length = gc::DVDOpen(value_file_path, &value_file_info);
        if (value_file_length != 0) {
            // Round the length of the value file to a multiple of 32, necessary for DVDReadAsyncPrio
            value_file_length = (value_file_info.length + 0x1f) & 0xffffffe0;
            value_file_buf = static_cast<char*>(heap::alloc_from_heap(value_file_length));
            value_file_length = gc::read_entire_file_using_dvdread_prio_async(&value_file_info, value_file_buf, value_file_length, 0);
            char* eof = value_file_buf + value_file_info.length;

            if (value_file_length != 0) {
                char* b = value_file_buf;
                int iter = 0;
                do {
                    bad_apple[iter] = *b;
                    b++;
                    iter++;
                }
                while (b <= eof);
                gc::OSReport("Loaded bad_apple at %x, %d long", &bad_apple, iter);
            }
        }
    }

    void tick() {
        if (mkb::main_mode == mkb::MD_GAME && (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN)) {
            if (pad::button_pressed(gc::PAD_BUTTON_X)) {
                gc::OSReport("Creating\n");

                for (int x = 0; x < WIDTH; x++) {
                    for (int y = 0; y < HEIGHT; y++) {
                        gc::OSReport("Creating %d, %d\n", x, y);
                        mkb::Item banana;
                        mkb::Item* dest_item;
                        mkb::memset(&banana,0,0xb4);
                        banana.type = mkb::BANANA_SINGLE;

                        banana.position.x = static_cast<float>(x)-8.0;
                        banana.position.y = static_cast<float>(-y)+14.0;
                        banana.position.z = 20;
                        banana.itemgroup_idx = 0;

                        int item_pool_iter = mkb::pool_alloc(&mkb::item_pool_info, 1);
                        if (-1 < item_pool_iter) {
                            dest_item = mkb::items + item_pool_iter;
                            mkb::g_fancy_memcpy(dest_item, &banana, 0xb4);
                            dest_item->index = item_pool_iter;
                            mkb::items[item_pool_iter].g_some_frame_counter = -1;
                            mkb::item_coin_init(dest_item);
                            mkb::items[item_pool_iter].field_0xc = 0;
                            mkb::items[item_pool_iter].field_0x18 = 1.0;
                            mkb::items[item_pool_iter].g_position_copy.x = mkb::items[item_pool_iter].position.x;
                            mkb::items[item_pool_iter].g_position_copy.y = mkb::items[item_pool_iter].position.y;
                            mkb::items[item_pool_iter].g_position_copy.z = mkb::items[item_pool_iter].position.z;
                            mkb::items[item_pool_iter].item_coli_func = mkb::item_coin_coli;
                            mkb::items[item_pool_iter].id = mkb::g_next_item_id;
                            mkb::items[item_pool_iter].shadow_intensity = 0.0;
                            mkb::items[item_pool_iter].field_0x64 = 0;
                            if (first_banana == nullptr) first_banana = &mkb::items[item_pool_iter];
                            mkb::g_next_item_id++;
                        }

                    }
                }
            }
            if (pad::button_pressed(gc::PAD_BUTTON_Y)) {
                play = true;
                gc::OSReport("Now playing\n");
                frame_counter = 0;
            }

            if (play) {
                if (is_alt_frame) {
                    gc::OSReport("Frame: %d\n", frame_counter);
                    char* current_frame;
                    char* current_char;
                    current_frame = bad_apple + (frame_counter*24);
                    bool move = false;

                    for (int pixel = 0; pixel < (WIDTH*HEIGHT); pixel++) {
                        current_char = (current_frame+(pixel/8));
                        move = *(current_char) & (7-(pixel%8));

                        //gc::OSReport("Pix: %d bidx: %d val: %d chidx: %d tchar: %x at %x\n", pixel, (7-(pixel%8)), move, (pixel/8), (pixel/8)+(frame_counter*24), current_char);
                        if (move) {
                            first_banana[pixel].scale = 0.5;
                        }
                        else {
                            first_banana[pixel].scale = 0.0;
                        }
                    }
                    frame_counter++;
                    is_alt_frame = !is_alt_frame;
                }
                else {
                    //gc::OSReport("alt frame ignored\n");
                    is_alt_frame = !is_alt_frame;
                }
            }
        }

    }
    /*
    for (int i = 0; i < mkb::item_pool_info.upper_bound; i++) {
        mkb::Item* item = &mkb::items[i];
        if (item->type == 0x0) {
            gc::OSReport("Banana at %x\n", item);
        }
    }*/

}
