#pragma once

#include "mkb/mkb.h"

#define DEBUG_LOGGING 1

constexpr char* LOG_PREFIX = "[wsmod] ";
constexpr char* LOG_PREFIX_LINE = "[wsmod] At %s:%d: ";
// These seem terribly hacky, maybe a better replacement could be made in the future
// Maybe we could even show a custom crash screen!

#define MOD_ASSERT(exp)                                                               \
    ({                                                                                \
        if (!(exp)) {                                                                 \
            mkb::OSPanic(__FILE__, __LINE__, "Failed assertion " #exp);          \
            LOG("Failed assertion in %s line %d: %s", __FILE__, __LINE__, #exp); \
            while (true)                                                              \
                ;                                                                     \
        }                                                                             \
    })

#define MOD_ASSERT_MSG(exp, msg)                                                       \
    ({                                                                                 \
        if (!(exp)) {                                                                  \
            mkb::OSPanic(__FILE__, __LINE__, msg);                                \
            LOG("Failed assertion in %s line %d: %s", __FILE__, __LINE__, (msg)); \
            while (true)                                                               \
                ;                                                                      \
        }                                                                              \
    })

// Fancy logging - note that this is less space-efficient than just calling mkb::OSReport
#define LOG(msg, ...)                                       \
    {                                                       \
        mkb::printf(const_cast<char*>(LOG_PREFIX));         \
        mkb::printf(const_cast<char*>(msg), ##__VA_ARGS__); \
        mkb::printf("\n");                                  \
    }

#if DEBUG_LOGGING
#define LOG_DEBUG(msg, ...)                                                       \
                                                                                  \
    {                                                                             \
        mkb::printf(const_cast<char*>(LOG_PREFIX_LINE), __FILE__, __LINE__); \
        mkb::printf(const_cast<char*>(msg), ##__VA_ARGS__);                       \
        mkb::printf("\n");                                                        \
    }
#else
#define LOG_DEBUG(msg, ...)
#endif
