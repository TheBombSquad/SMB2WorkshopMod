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
    static char* bad_apple;
    static mkb::Item* first_banana = nullptr;
    static int frame_counter = 0;
    static constexpr u32 WIDTH = 40;
    static constexpr u32 HEIGHT = 30;
    static constexpr u32 BANANA_COUNT = 1200;
    static constexpr u32 CHUNK_SIZE = 0x6000;
    static mkb::Item* items_big;
    static mkb::PoolInfo item_big_pool_info;
    static u8 item_big_status_list[BANANA_COUNT];
    static bool initialized = false;
    static char* active_char;
    static u32 processed_data = 0;
    static s8 chunks = 0;
    static void (*sprtick)(u8 *status,mkb::Sprite *sprite);

    void event_item_init_new() {
        gc::OSReport("enterring init free: %d to alloc %d\n", heap::get_free_space(), (BANANA_COUNT*0xb4));
        items_big = static_cast<mkb::Item*>(heap::alloc_from_heap(BANANA_COUNT * 0xb4));
        gc::OSReport("alloced items_big at %x\n", items_big);
        mkb::g_next_item_id = 0;
        mkb::pool_clear(&item_big_pool_info);
        memset(items_big, 0, 0xb4*BANANA_COUNT);
        for (int banana = 0; banana < BANANA_COUNT; banana++) {
            items_big[banana].index = banana;
            items_big[banana].id = -1;
        }

        item_big_pool_info.status_list = item_big_status_list;
        item_big_pool_info.len = BANANA_COUNT;
        gc::OSReport("enterring exiting init\n");
    }

    void event_item_tick_new() {
        if ((mkb::g_some_other_flags & 10) == 0) {
            //gc::OSReport("enterring\n");
            mkb::Item* item = items_big;
            u8* item_status = item_big_status_list;
            int item_ctr = item_big_pool_info.upper_bound;

            while (0 < item_ctr) {
                if (*item_status != 0) {
                    if (item->field_0xc != 0) {
                        item->field_0xc = item->field_0xc + -1;
                    }
                    if (0 < (short)item->g_some_flag2) {
                        item->g_some_flag2 = item->g_some_flag2 + -1;
                    }
                    if (item->g_some_flag2 == 0) {
                        *item_status = 0;
                    }
                    mkb::item_coin_tick(item);
                }
                item_ctr--;
                item_status++;
                item++;
            }
            //gc::OSReport("exiting\n");
        }
        return;
    }

    void coli_items_new() {
        mkb::Item* item = items_big;
        u8* item_status = item_big_status_list;
        int item_ctr = item_big_pool_info.upper_bound;

        mkb::PhysicsBall ball;
        mkb::init_physicsball_from_ball(mkb::current_ball, &ball);
        while (0 < item_ctr) {
            if (*item_status != 0 && (item->g_some_bitfield & 2) != 0 && (item->field_0xc == 0)) {
                u32 coli_check = mkb::g_some_collision_check(ball.ball_size, item->scale, &ball.prev_pos, &ball.pos, &item->g_position_copy, &item->position);

                if (coli_check != 0 && item->scale > 0.1) {
                    item->field_0xc = 8;
                    mkb::item_coin_coli(item, &ball);
                    item->g_some_flag = 6;
                    item->g_some_bitfield |= 1;


                }
            }
            item_ctr--;
            item_status++;
            item++;
        }
    }
    void draw_items_new() {
        mkb::Item* item = items_big;
        u8* item_status = item_big_status_list;
        int item_ctr = item_big_pool_info.upper_bound;

        while (0 < item_ctr) {
            if (*item_status != 0) {
                mkb::item_coin_disp(item);
            }
            item_ctr--;
            item_status++;
            item++;
        }
    }

    int round32(int i) {
        return (i + 0x1f) & 0xffffffe0;
    }
    void read_bad_apple(int start) {
        gc::OSReport("[d] read_bad_apple called with param %d\n", start);
        value_file_length = gc::DVDOpen(value_file_path, &value_file_info);
        if (value_file_length != 0) {
            gc::OSReport("[d] enterring function...\n");
            int length = CHUNK_SIZE;
            bool adjusted = false;

            value_file_length = round32(value_file_info.length);

            if ((start+CHUNK_SIZE) > value_file_length) {
                gc::OSReport("[d] length adjusted due to near eof (asked for %d, with left: %d)\n", (start+CHUNK_SIZE), value_file_length);
                length = round32(value_file_length-start);
            }
            gc::OSReport("[d] length: %d\n", length);

            if (start % 4 != 0) {
                start = start-2;
                adjusted = true;
            }

            gc::OSReport("[d] reading\n");
            value_file_length = gc::read_entire_file_using_dvdread_prio_async(&value_file_info, bad_apple, length, start);

            gc::OSReport("[d] read %x at %x with offset %d\n", length, bad_apple, start);

            if (adjusted) {
                bad_apple = bad_apple + 2;
            }
        }
        gc::DVDClose(&value_file_info);
    }

    void init() {
        bad_apple = reinterpret_cast<char*>(heap::alloc_from_heap(CHUNK_SIZE));
        read_bad_apple(0);

        //init
        patch::write_blr(reinterpret_cast<void*>(0x80315428));
        //tick
        patch::write_blr(reinterpret_cast<void*>(0x80315584));
        //dest
        //patch::write_blr(reinterpret_cast<void*>(0x803157e8));
        //draw
        patch::write_blr(reinterpret_cast<void*>(0x8031589c));

        // coli
        sprtick = patch::hook_function(
            mkb::sprite_hud_stage_name_tick, [](u8 *status, mkb::Sprite *sprite)
            {
                sprtick(status, sprite);

                if (mkb::itemgroups[2].playback_state == mkb::PLAYBACK_FORWARD) {
                    if (sprite->field_0x1 == 0x3) {
                        mkb::sprintf(sprite->text, "BAD BANANA\0");
                    }
                }

            });

       patch::write_blr(reinterpret_cast<void*>(0x802a5e94));

    }

    void create_bananas() {
                event_item_init_new();
                initialized = true;
                for (int x = 0; x < WIDTH; x++) {
                    for (int y = 0; y < HEIGHT; y++) {
                        gc::OSReport("Creating %d, %d\n", x, y);
                        mkb::Item banana;
                        mkb::Item* dest_item;
                        mkb::memset(&banana,0,0xb4);
                        banana.type = mkb::BANANA_SINGLE;

                        banana.position.x = static_cast<float>(x)-20;
                        banana.position.y = static_cast<float>(-y)+5;
                        banana.position.z = -70;
                        banana.itemgroup_idx = 0;

                        int item_pool_iter = mkb::pool_alloc(&item_big_pool_info, 1);
                        if (-1 < item_pool_iter) {
                            dest_item = items_big + item_pool_iter;
                            mkb::g_fancy_memcpy(dest_item, &banana, 0xb4);
                            dest_item->index = item_pool_iter;
                            items_big[item_pool_iter].g_some_frame_counter = -1;
                            mkb::item_coin_init(dest_item);
                            items_big[item_pool_iter].field_0xc = 0;
                            items_big[item_pool_iter].field_0x18 = 1.0;
                            items_big[item_pool_iter].g_position_copy.x = items_big[item_pool_iter].position.x;
                            items_big[item_pool_iter].g_position_copy.y = items_big[item_pool_iter].position.y;
                            items_big[item_pool_iter].g_position_copy.z = items_big[item_pool_iter].position.z;
                            items_big[item_pool_iter].scale = 0;
                            items_big[item_pool_iter].item_coli_func = &mkb::item_coin_coli;
                            items_big[item_pool_iter].id = mkb::g_next_item_id;
                            //items_big[item_pool_iter].shadow_intensity = 0.0;
                            //items_big[item_pool_iter].field_0x64 = 0;
                            if (first_banana == nullptr) first_banana = &items_big[item_pool_iter];
                            mkb::g_next_item_id++;
                        }

                    }
                }
    }

    void start_playback() {
                play = true;
                gc::OSReport("Now playing\n");
                frame_counter = 0;
                active_char = bad_apple;
                processed_data = 0;
    }

    void tick() {

        if (initialized) {
            event_item_tick_new();
            draw_items_new();
            coli_items_new();

            // item list replacement
            //patch::write_word(reinterpret_cast<void*>(0x803172b4), 0x3c00804d);
            //patch::write_word(reinterpret_cast<void*>(0x803172b8), 0x6000b540);
            // item pool replacement
            //patch::write_word(reinterpret_cast<void*>(0x803172c0), 0x3c60804d);
            //patch::write_word(reinterpret_cast<void*>(0x803172c4), 0x6063147c);
        }

        if (mkb::main_mode == mkb::MD_GAME && (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN)) {
            if (pad::button_pressed(gc::PAD_BUTTON_A)) {
                u32 freeHeapSpace = heap::get_free_space();
                gc::OSReport("free: %d\n", freeHeapSpace);
                gc::OSReport("big item: %x bpu: %x, big pool ub: %x\n", &items_big, &item_big_pool_info, &item_big_pool_info.upper_bound);
            }

            if (mkb::itemgroups[2].playback_state == mkb::PLAYBACK_FORWARD && !play) {
                mkb::stage_time_frames_remaining = 6520*2;
                create_bananas();
                start_playback();
            }

            if (frame_counter >= 6565) {
                start_playback();
            }

            if (play) {
                if (is_alt_frame) {
                    //gc::OSReport("Frame: %d\n", frame_counter);

                    //gc::OSReport("%x - %x %x\n", (active_char-bad_apple), pix_data, counter);

                    u8 pix_data = *active_char;
                    u8 counter = *(active_char+1);
                    short active_pix = 0;


                    while (active_pix < (WIDTH*HEIGHT)) {
                        if ((first_banana[active_pix].g_some_bitfield & 1) == 0) {
                            first_banana[active_pix].scale = (static_cast<float>(pix_data)/255.0)*0.5;
                        }
                        else {
                            first_banana[active_pix].scale = 0;
                        }
                        active_pix++;
                        counter--;

                        if (counter == 0) {
                            u32 current_pos = active_char-bad_apple;
                            if (current_pos+32 >= CHUNK_SIZE) {
                                chunks++;
                                gc::OSReport("[p] chunks read: %d\n", chunks);
                                gc::OSReport("[p] current char is %x %x.\n", *active_char, *(active_char+1));
                                read_bad_apple(current_pos + processed_data);
                                active_char = bad_apple;
                                gc::OSReport("[p] new sequence is %x %x %x %x %x %x.\n", *active_char, *(active_char+1), *(active_char+2), *(active_char+3), *(active_char+4), *(active_char+5));
                                processed_data += current_pos;
                            }

                            active_char = active_char+2;
                            pix_data = *active_char;
                            counter = *(active_char+1);
                            //gc::OSReport("%x - %x %x\n", (active_char-bad_apple), pix_data, counter);
                        }
                    }

                    frame_counter++;
                    is_alt_frame = !is_alt_frame;

                }
                else {
                    is_alt_frame = !is_alt_frame;
                }
            }
        }

    }

}
