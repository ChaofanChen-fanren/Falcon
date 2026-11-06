#ifndef FALCON_INCLUDE_FALCON_MACROS_H
#define FALCON_INCLUDE_FALCON_MACROS_H

#include <cassert>
#include <iostream>

// Internal helper: invoke only after the condition has failed.
// Keep message unparenthesized to support stream expressions such as "fd=" << fd.
#define FALCON_DETAIL_ASSERT_FAIL(expression, message)                                                                 \
    do {                                                                                                               \
        std::cerr << "Assertion failed: " << (expression) << "\nLocation: " << __FILE__ << ':' << __LINE__ << message  \
                  << std::endl;                                                                                        \
        assert(false && (expression));                                                                                 \
    } while (0)

// Without NDEBUG: log and abort. With NDEBUG: log and continue.
#define FALCON_ASSERT(x)                                                                                               \
    do {                                                                                                               \
        if (!(x)) [[unlikely]] {                                                                                       \
            FALCON_DETAIL_ASSERT_FAIL(#x, "");                                                                         \
        }                                                                                                              \
    } while (0)

#define FALCON_ASSERT_MSG(x, msg)                                                                                      \
    do {                                                                                                               \
        if (!(x)) [[unlikely]] {                                                                                       \
            FALCON_DETAIL_ASSERT_FAIL(#x, "\nMessage: " << msg);                                                       \
        }                                                                                                              \
    } while (0)

// With NDEBUG: log and perform the specified return or loop control action.
#define ASSERT_RETVAL(x, val)                                                                                          \
    do {                                                                                                               \
        if (!(x)) [[unlikely]] {                                                                                       \
            FALCON_DETAIL_ASSERT_FAIL(#x, "");                                                                         \
            return (val);                                                                                              \
        }                                                                                                              \
    } while (0)

#define ASSERT_RETVAL_MSG(x, val, msg)                                                                                 \
    do {                                                                                                               \
        if (!(x)) [[unlikely]] {                                                                                       \
            FALCON_DETAIL_ASSERT_FAIL(#x, "\nMessage: " << msg);                                                       \
            return (val);                                                                                              \
        }                                                                                                              \
    } while (0)

#define ASSERT_RETNONE(x)                                                                                              \
    do {                                                                                                               \
        if (!(x)) [[unlikely]] {                                                                                       \
            FALCON_DETAIL_ASSERT_FAIL(#x, "");                                                                         \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0)

#define ASSERT_RETNONE_MSG(x, msg)                                                                                     \
    do {                                                                                                               \
        if (!(x)) [[unlikely]] {                                                                                       \
            FALCON_DETAIL_ASSERT_FAIL(#x, "\nMessage: " << msg);                                                       \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0)

// These have the same behavior as the base assertions.
#define ASSERT_NOEFFECT(x) FALCON_ASSERT(x)
#define ASSERT_NOEFFECT_MSG(x, msg) FALCON_ASSERT_MSG(x, msg)

// Do not wrap these in do/while: continue and break must target the caller.
// The final else (void)0 consumes the caller's semicolon safely in if/else.
#define ASSERT_CONTINUE(x)                                                                                             \
    if (!(x)) [[unlikely]] {                                                                                           \
        FALCON_DETAIL_ASSERT_FAIL(#x, "");                                                                             \
        continue;                                                                                                      \
    } else                                                                                                             \
        (void)0

#define ASSERT_CONTINUE_MSG(x, msg)                                                                                    \
    if (!(x)) [[unlikely]] {                                                                                           \
        FALCON_DETAIL_ASSERT_FAIL(#x, "\nMessage: " << msg);                                                           \
        continue;                                                                                                      \
    } else                                                                                                             \
        (void)0

#define ASSERT_BREAK(x)                                                                                                \
    if (!(x)) [[unlikely]] {                                                                                           \
        FALCON_DETAIL_ASSERT_FAIL(#x, "");                                                                             \
        break;                                                                                                         \
    } else                                                                                                             \
        (void)0

#define ASSERT_BREAK_MSG(x, msg)                                                                                       \
    if (!(x)) [[unlikely]] {                                                                                           \
        FALCON_DETAIL_ASSERT_FAIL(#x, "\nMessage: " << msg);                                                           \
        break;                                                                                                         \
    } else                                                                                                             \
        (void)0

#endif // FALCON_INCLUDE_FALCON_MACROS_H
