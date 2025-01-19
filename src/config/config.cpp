#include "config.h"

#include "internal/assembly.h"
#include "internal/heap.h"
#include "internal/log.h"
#include "internal/tickable.h"
#include "patches/custom/menu_option_toggle.h"

#define STREQ(x, y) (mkb::strcmp(const_cast<char*>(x), const_cast<char*>(y)) == 0)
#define KEY_ENABLED(x) (STREQ(key, x) && STREQ(value, "enabled"))

namespace config {

static int config_file_length;
static mkb::DVDFileInfo config_file_info;
static char* config_file_buf;
static char config_file_path[] = "/config.txt";

u16* parse_stageid_list(char* buf, u16* array) {
    buf = mkb::strchr(buf, '\n') + 1;

    char* end_of_section;
    char key[64] = {0};
    char value[64] = {0};
    end_of_section = mkb::strchr(buf, '}');
    do {
        char *key_start, *key_end, *end_of_line;
        key_start = mkb::strchr(buf, 'E') + 2;
        key_end = mkb::strchr(buf, ':');
        MOD_ASSERT_MSG(key_start < key_end, "Key start after key end, did you start your key with a tab and not spaces?");
        end_of_line = mkb::strchr(buf, '\n');
        mkb::strncpy(key, key_start, (key_end - key_start));
        mkb::strncpy(value, key_end + 2, (end_of_line - key_end) - 2);
        int key_idx = mkb::atoi(key);
        u16 value_short = (u16) mkb::atoi(value);
        array[key_idx] = value_short;

        buf = end_of_line + 1;
        mkb::memset(key, '\0', 64);
        mkb::memset(value, '\0', 64);
    } while (buf < end_of_section);
    return array;
}

void parse_menu_option_toggles(char* buf) {
    buf = mkb::strchr(buf, '\n') + 1;

    char* end_of_section;
    char key[64] = {0};
    char value[64] = {0};
    end_of_section = mkb::strchr(buf, '}');
    do {
        char *key_start, *key_end, *end_of_line;
        key_start = mkb::strchr(buf, '\t') + 1;
        key_end = mkb::strchr(buf, ':');
        MOD_ASSERT_MSG(key_start < key_end, "Key start after key end, did you start your key with a tab and not spaces?");
        end_of_line = mkb::strchr(buf, '\n');
        mkb::strncpy(key, key_start, (key_end - key_start));
        mkb::strncpy(value, key_end + 2, (end_of_line - key_end) - 2);

        if KEY_ENABLED ("monkey-race")
            menu_option_toggle::party_game_bitflag |= 0x1;
        else if KEY_ENABLED ("monkey-fight")
            menu_option_toggle::party_game_bitflag |= 0x2;
        else if KEY_ENABLED ("monkey-target")
            menu_option_toggle::party_game_bitflag |= 0x4;
        else if KEY_ENABLED ("monkey-billiards")
            menu_option_toggle::party_game_bitflag |= 0x8;
        else if KEY_ENABLED ("monkey-bowling")
            menu_option_toggle::party_game_bitflag |= 0x10;
        else if KEY_ENABLED ("monkey-golf")
            menu_option_toggle::party_game_bitflag |= 0x20;
        else if KEY_ENABLED ("monkey-boat")
            menu_option_toggle::party_game_bitflag |= 0x40;
        else if KEY_ENABLED ("monkey-shot")
            menu_option_toggle::party_game_bitflag |= 0x80;
        else if KEY_ENABLED ("monkey-dogfight")
            menu_option_toggle::party_game_bitflag |= 0x100;
        else if KEY_ENABLED ("monkey-soccer")
            menu_option_toggle::party_game_bitflag |= 0x200;
        else if KEY_ENABLED ("monkey-baseball")
            menu_option_toggle::party_game_bitflag |= 0x400;
        else if KEY_ENABLED ("monkey-tennis")
            menu_option_toggle::party_game_bitflag |= 0x800;
        else if KEY_ENABLED ("party-games")
            menu_option_toggle::mode_bitflag &= ~0x2;
        else if KEY_ENABLED ("story-mode")
            menu_option_toggle::main_game_bitflag &= ~0x1;
        else if KEY_ENABLED ("challenge-mode")
            menu_option_toggle::main_game_bitflag &= ~0x2;
        else if KEY_ENABLED ("beginner")
            menu_option_toggle::level_bitflag &= ~0x1;
        else if KEY_ENABLED ("advanced")
            menu_option_toggle::level_bitflag &= ~0x2;
        else if KEY_ENABLED ("expert")
            menu_option_toggle::level_bitflag &= ~0x4;
        else if KEY_ENABLED ("master")
            menu_option_toggle::level_bitflag &= ~0x8;
        buf = end_of_line + 1;
        mkb::memset(key, '\0', 64);
        mkb::memset(value, '\0', 64);
    } while (buf < end_of_section);
}

void parse_function_toggles(char* buf) {
    // Enters from section parsing, so skip to the next line
    buf = mkb::strchr(buf, '\n') + 1;

    char* end_of_section;
    char key[64] = {0};
    char value[64] = {0};
    end_of_section = mkb::strchr(buf, '}');
    int parsed_value;

    do {
        char *key_start, *key_end, *end_of_line;
        key_start = mkb::strchr(buf, '\t') + 1;
        key_end = mkb::strchr(buf, ':');
        MOD_ASSERT_MSG(key_start < key_end, "Key start after key end, did you start your key with a tab and not spaces?");
        end_of_line = mkb::strchr(buf, '\n');
        mkb::strncpy(key, key_start, (key_end - key_start));
        mkb::strncpy(value, key_end + 2, (end_of_line - key_end) - 2);

        // Set the state of a given tickable based on the found key
        for (const auto& tickable: tickable::get_tickable_manager().get_tickables()) {
            // mkb::OSReport("debug: tickable parse\n");
            if (tickable->name != nullptr && STREQ(key, tickable->name)) {
                // 'value' is enabled, set the value to 1
                if (STREQ(value, "enabled")) {
                    tickable->enabled = true;
                    break;
                }

                // 'value' is disabled, set value to 0
                else if (STREQ(value, "disabled")) {
                    break;
                }

                // 'value' is some integer, set the value and initialize the patch if it differs from the default
                else {
                    parsed_value = mkb::atoi(value);

                    // Only set value on tickables that have a defined default active value
                    if (!tickable->active_value.has_value()) break;

                    // Check to see if the passed value is within the defined bounds
                    MOD_ASSERT_MSG(parsed_value >= tickable->lower_bound, "Passed value for patch smaller than minimum value");
                    MOD_ASSERT_MSG(parsed_value <= tickable->upper_bound, "Passed value for patch larger than maximum value");

                    // Set the enabled to the parsed value, if it differes from the default passed value
                    if (parsed_value != tickable->active_value) {
                        tickable->enabled = true;
                        tickable->active_value = parsed_value;
                        break;
                    }

                    // If the value is the default, do not enable the patch
                    else {
                        break;
                    }
                }
            }
        }

        buf = end_of_line + 1;
        mkb::memset(key, '\0', 64);
        mkb::memset(value, '\0', 64);
    } while (buf < end_of_section);
}

void parse_config() {
    bool open_success = mkb::DVDOpen(config_file_path, &config_file_info);
    if (open_success) {
        // heap::alloc rounds to a multiple of 32, necessary for DVDReadAsyncPrio
        config_file_length = (config_file_info.length + 0x1f) & 0xffffffe0;
        config_file_buf = static_cast<char*>(heap::alloc(config_file_length));
        u32 read_length = mkb::read_entire_file_using_dvdread_prio_async(&config_file_info, config_file_buf, config_file_length, 0);
        char* eof = config_file_buf + config_file_info.length;

        if (read_length > 0) {
            LOG("Now parsing config file...");
            char section[64] = {0};
            char* file = config_file_buf;
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

                    mkb::strncpy(section, section_start, (section_end - section_start));

                    LOG("Now parsing category %s...", section);

                    // Parsing function toggles
                    if (STREQ(section, "REL Patches")) {
                        parse_function_toggles(section_end);
                    }

                    else if (STREQ(section, "Menu Option Toggles")) {
                        parse_menu_option_toggles(section_end);
                    }

                    else if (STREQ(section, "Theme IDs")) {
                        parse_stageid_list(section_end, main::theme_id_lookup);
                        LOG("Theme ID list loaded at: 0x%X", &main::theme_id_lookup);
                    }

                    else if (STREQ(section, "Difficulty Layout")) {
                        LOG("%s", section);
                    }

                    else if (STREQ(section, "Music IDs")) {
                        parse_stageid_list(section_end, main::bgm_id_lookup);
                        LOG("Music ID list loaded at: 0x%X", &main::bgm_id_lookup);
                    }

                    else {
                        LOG("Unknown category %s found in config!", section);
                    }

                    file = mkb::strchr(section_end, '\n') + 1;
                    mkb::memset(section, '\0', 64);
                }
                else {
                    break;
                }
            } while (file <= eof);
        }
        mkb::DVDClose(&config_file_info);
        heap::free(config_file_buf);
    }
}

}// namespace config
