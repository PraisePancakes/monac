#ifndef MONAC_CONFIG_MACRO_H
#define MONAC_CONFIG_MACRO_H

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define MONAC_STR(arg) #arg
#define MONAC_XSTR(arg) MONAC_STR(arg)

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif

#ifndef MONAC_NDEBUG
#define MONAC_ASSERT(condition, message)                                       \
    do {                                                                       \
        if (!(condition)) {                                                    \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__   \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate();                                                  \
        }                                                                      \
    } while (false)
#else
#define MONAC_ASSERT(condition, message) \
    do {                                 \
    } while (false)
#endif