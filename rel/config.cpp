#include "config.h"
#include "relpatches.h"
#include "patch.h"
#include "savestate.h"
#include "tetris.h"
#include "draw.h"
#include "timer.h"
#include "iw.h"
#include "pad.h"
#include "menu_impl.h"
#include "jump.h"
#include "scratch.h"
#include "assembly.h"
#include "inputdisp.h"
#include "gotostory.h"

#define STREQ(x,y) (mkb::strcmp(const_cast<char*>(x),const_cast<char*>(y))==0)
#define KEY_ENABLED(x) (STREQ(key, x) && STREQ(value, "enabled"))

namespace config {

static int config_file_length;
static mkb::DVDFileInfo config_file_info;
static char* config_file_buf;
static char config_file_path[] = "/config.txt";

bool tetris_enabled = false;
bool apesphere_toggle_enabled = false;

static void unlock_everything()
{
    // Don't yet know how to unlock the staff credits game from a fresh save...
    mkb::unlock_info.master_unlocked = true;
    mkb::unlock_info.monkeys = 99;
    mkb::unlock_info.staff_credits_game_unlocked = true;
    mkb::unlock_info.play_points = 99999;
    mkb::unlock_info.newest_play_point_record = 99999;
    mkb::unlock_info.movies = 0x0fff;
    mkb::unlock_info.party_games = 0x0001b600;
    mkb::unlock_info.g_movies_watched = 0x0fff;
    mkb::memset(mkb::cm_unlock_entries, 0xff, sizeof(mkb::cm_unlock_entries));
    mkb::memset(mkb::storymode_unlock_entries, 0xff, sizeof(mkb::storymode_unlock_entries));
}

static void misc_apesphere_init()
{
    mkb::strcpy(reinterpret_cast<char *>(0x8047f4ec), "APESPHERE PRACTICE MOD");
    patch::write_branch(reinterpret_cast<void *>(0x8032ad0c),
                        reinterpret_cast<void *>(main::custom_titlescreen_text_color));

    Tetris::get_instance().init();
    tetris_enabled = true;
}

u16* parse_stageid_list(char* buf, u16* array) {
    buf = mkb::strchr(buf, '\n')+1;

    char *end_of_section;
    char key[64] = {0};
    char value[64] = {0};
    end_of_section = mkb::strchr(buf, '}');
    do {
        char *key_start, *key_end, *end_of_line;
        key_start = mkb::strchr(buf, 'E')+2;
        key_end = mkb::strchr(buf, ':');
        MOD_ASSERT_MSG(key_start < key_end, "Key start after key end, did you start your key with a tab and not spaces?");
        end_of_line = mkb::strchr(buf, '\n');
        mkb::strncpy(key, key_start, (key_end-key_start));
        mkb::strncpy(value, key_end+2, (end_of_line-key_end)-2);
        int key_idx = mkb::atoi(key);
        u16 value_short = (u16)mkb::atoi(value);
        array[key_idx] = value_short;

        buf = end_of_line+1;
        mkb::memset(key, '\0', 64);
        mkb::memset(value, '\0', 64);
    }
    while (buf < end_of_section);
    return array;
}

relpatches::Tickable apesphere_tickables[] = {
        {
            .tick_func = unlock_everything,
        },
        {
            .main_loop_init_func = draw::init,
            .disp_func = draw::predraw,
        },
        {
            .disp_func = draw::disp,
        },
        {   .main_loop_init_func = iw::init,
            .disp_func = iw::disp,
            .tick_func = iw::tick,
        },
        {
            .main_loop_init_func = savestate::init,
            .tick_func = savestate::tick,
        },
        {
            .main_loop_init_func = timer::init,
            .disp_func = timer::disp,
        },
        {
            .disp_func = menu::disp,
            .tick_func = menu::tick,
        },
        {
            .tick_func = jump::tick,
        },
        {
            .main_loop_init_func = scratch::init,
            .tick_func = scratch::tick,
        },
        {
            .main_loop_init_func = misc_apesphere_init,
        },
        {
            .main_loop_init_func = inputdisp::init,
            .disp_func = inputdisp::disp,
        },
        {
            .tick_func = gotostory::tick,
        }
};

const unsigned int APESPHERE_TICKABLE_COUNT = sizeof(apesphere_tickables)/sizeof(apesphere_tickables[0]);

void parse_party_game_toggles(char* buf) {
    buf = mkb::strchr(buf, '\n')+1;

    char *end_of_section;
    char key[64] = {0};
    char value[64] = {0};
    end_of_section = mkb::strchr(buf, '}');
    do {
        char *key_start, *key_end, *end_of_line;
        key_start = mkb::strchr(buf, '\t')+1;
        key_end = mkb::strchr(buf, ':');
        MOD_ASSERT_MSG(key_start < key_end, "Key start after key end, did you start your key with a tab and not spaces?");
        end_of_line = mkb::strchr(buf, '\n');
        mkb::strncpy(key, key_start, (key_end-key_start));
        mkb::strncpy(value, key_end+2, (end_of_line-key_end)-2);

        if KEY_ENABLED("monkey-race")           relpatches::party_game_toggle::party_game_bitflag |= 0x1;
        else if KEY_ENABLED("monkey-fight")     relpatches::party_game_toggle::party_game_bitflag |= 0x2;
        else if KEY_ENABLED("monkey-target")    relpatches::party_game_toggle::party_game_bitflag |= 0x4;
        else if KEY_ENABLED("monkey-billiards") relpatches::party_game_toggle::party_game_bitflag |= 0x8;
        else if KEY_ENABLED("monkey-bowling")   relpatches::party_game_toggle::party_game_bitflag |= 0x10;
        else if KEY_ENABLED("monkey-golf")      relpatches::party_game_toggle::party_game_bitflag |= 0x20;
        else if KEY_ENABLED("monkey-boat")      relpatches::party_game_toggle::party_game_bitflag |= 0x40;
        else if KEY_ENABLED("monkey-shot")      relpatches::party_game_toggle::party_game_bitflag |= 0x80;
        else if KEY_ENABLED("monkey-dogfight")  relpatches::party_game_toggle::party_game_bitflag |= 0x100;
        else if KEY_ENABLED("monkey-soccer")    relpatches::party_game_toggle::party_game_bitflag |= 0x200;
        else if KEY_ENABLED("monkey-baseball")  relpatches::party_game_toggle::party_game_bitflag |= 0x400;
        else if KEY_ENABLED("monkey-tennis")    relpatches::party_game_toggle::party_game_bitflag |= 0x800;

        buf = end_of_line+1;
        mkb::memset(key, '\0', 64);
        mkb::memset(value, '\0', 64);
    }
    while (buf < end_of_section);
}

void init_apesphere_tickables() {
    mkb::OSReport("[mod]  Enabling ApeSphere practice mod!");
    for (unsigned int i = 0; i < APESPHERE_TICKABLE_COUNT; i++) {
        if (!apesphere_tickables[i].status) {
            apesphere_tickables[i].status = true;
            if (apesphere_tickables[i].main_loop_init_func != nullptr) {
                apesphere_tickables[i].main_loop_init_func();
            }
        }

    }
}

void parse_function_toggles(char* buf) {
    // Enters from section parsing, so skip to the next line
    buf = mkb::strchr(buf, '\n')+1;

    char *end_of_section;
    char key[64] = {0};
    char value[64] = {0};
    end_of_section = mkb::strchr(buf, '}');
    int parsed_value;

    do {
        char *key_start, *key_end, *end_of_line;
        key_start = mkb::strchr(buf, '\t')+1;
        key_end = mkb::strchr(buf, ':');
        MOD_ASSERT_MSG(key_start < key_end, "Key start after key end, did you start your key with a tab and not spaces?");
        end_of_line = mkb::strchr(buf, '\n');
        mkb::strncpy(key, key_start, (key_end-key_start));
        mkb::strncpy(value, key_end+2, (end_of_line-key_end)-2);

        if (KEY_ENABLED("apesphere-practice") || KEY_ENABLED("apesphere-always-enabled")) {
            mkb::OSReport("[mod]  ApeSphere practice mod is always enabled!\n");
            init_apesphere_tickables();
        }

        else if KEY_ENABLED("apesphere-togglable") {
            mkb::OSReport("[mod]  ApeSphere practice mod can be enabled by pressing L+R on the title screen!\n");
            apesphere_toggle_enabled = true;
        }

        else {
            // Iterate through all the REL patch tickables, looking for match of key name
            for (unsigned int i = 0; i < relpatches::PATCH_COUNT; i++) {
                if (relpatches::patches[i].name != nullptr && STREQ(key, relpatches::patches[i].name)) {

                    // 'value' is enabled, set the value to 1
                    if (STREQ(value, "enabled")) {
                        relpatches::patches[i].status = true;

                        // Execute the main_loop init func, if it exists
                        if (relpatches::patches[i].main_loop_init_func != nullptr) {
                            relpatches::patches[i].main_loop_init_func();
                        }

                        // Print init message, if it exists
                        if (relpatches::patches[i].message != nullptr) {
                            mkb::OSReport(relpatches::patches[i].message, "ENABLED!");
                        }

                        break;
                    }

                    // 'value' is disabled, set value to 0
                    else if (STREQ(value, "disabled")) {
                        if (relpatches::patches[i].message != nullptr) {
                            mkb::OSReport(relpatches::patches[i].message, "disabled!");
                        }
                        break;
                    }

                    // 'value' is some integer, set the value and initialize the patch if it differs from the default
                    else {
                        parsed_value = mkb::atoi(value);


                        // Check to see if the passed value is within the defined bounds
                        MOD_ASSERT_MSG(parsed_value >= relpatches:: patches[i].minimum_value, "Passed value for patch smaller than minimum value");
                        MOD_ASSERT_MSG(parsed_value <= relpatches:: patches[i].maximum_value, "Passed value for patch larger than maximum value");

                        // Set the status to the parsed value, if it differes from the default passed value
                        if (parsed_value != relpatches::patches[i].default_value) {
                            relpatches::patches[i].status = parsed_value;

                            // Execute the main_loop init func, if it exists
                            if (relpatches::patches[i].main_loop_init_func != nullptr) {
                                relpatches::patches[i].main_loop_init_func();
                            }

                            // Print init message, if it exists
                            if (relpatches::patches[i].message != nullptr) {
                                mkb::OSReport(relpatches::patches[i].message, "ENABLED! (custom value passed)", parsed_value);
                            }

                            break;
                        }

                        // If the value is the default, do not enable the patch
                        else {
                            if (relpatches::patches[i].message != nullptr) {
                                mkb::OSReport(relpatches::patches[i].message, "disabled! (default value passed)", parsed_value);
                            }

                            break;
                        }

                    }
                }
            }
        }

        buf = end_of_line+1;
        mkb::memset(key, '\0', 64);
        mkb::memset(value, '\0', 64);
    }
    while (buf < end_of_section);
}

void parse_config() {
    config_file_length = mkb::DVDOpen(config_file_path, &config_file_info);
    if (config_file_length != 0) {
        // Round the length of the config file to a multiple of 32, necessary for DVDReadAsyncPrio
        config_file_length = (config_file_info.length + 0x1f) & 0xffffffe0;
        config_file_buf = static_cast<char*>(heap::alloc_from_heap(config_file_length));
        config_file_length = mkb::read_entire_file_using_dvdread_prio_async(&config_file_info, config_file_buf, config_file_length, 0);
        char* eof = config_file_buf + config_file_info.length;

        if (config_file_length != 0) {
            mkb::OSReport("[mod] Now parsing config file...\n");
            char section[64] = {0};
            char *file = config_file_buf;
            do {
                char *section_start, *section_end;
                // Parse the start of a section of the config starting with # and ending with {
                // Example: # Section {
                section_start = mkb::strchr(file, '#');
                section_end = mkb::strchr(file, '{');
                if (section_start != nullptr && section_end != nullptr) {
                    MOD_ASSERT_MSG(section_start < section_end, "Section end before section start, are you sure you started/ended the section segment properly?");
                    // Strip out the '# ' at the start of string, strip out the ' ' at the end
                    section_start += 2;
                    section_end -= 1;

                    mkb::strncpy(section, section_start, (section_end-section_start));

                    mkb::OSReport("[mod] Now parsing category %s...\n", section);

                    // Parsing function toggles
                    if (STREQ(section, "REL Patches") || STREQ(section, "ApeSphere")) {
                        parse_function_toggles(section_end);
                    }

                    else if (STREQ(section, "Party Game Toggles")) {
                        parse_party_game_toggles(section_end);
                    }

                    else if (STREQ(section, "Theme IDs")) {
                        parse_stageid_list(section_end, main::theme_id_lookup);
                        mkb::OSReport("[mod]  Theme ID list loaded at: 0x%X\n", &main::theme_id_lookup);
                    }

                    else if (STREQ(section, "Difficulty Layout")) {
                        mkb::OSReport("%s\n", section);
                    }

                    else if (STREQ(section, "Music IDs")) {
                        parse_stageid_list(section_end, main::bgm_id_lookup);
                        mkb::OSReport("[mod]  Music ID list loaded at: 0x%X\n", &main::bgm_id_lookup);
                    }

                    else {
                        mkb::OSReport("[mod]  Unknown category %s found in config!\n", section);
                    }

                    file = mkb::strchr(section_end, '\n')+1;
                    mkb::memset(section, '\0', 64);
                }
                else {
                    break;
                }
            }
            while (file <= eof);

        }
        mkb::DVDClose(&config_file_info);
        heap::free_to_heap(config_file_buf);
    }
}

}
