#ifndef FALCON_INCLUDE_FALCON_UTILS_THREAD_UTILS_H
#define FALCON_INCLUDE_FALCON_UTILS_THREAD_UTILS_H

#include <falcon/export.h>
#include <falcon/types.h>
#include <string>

namespace falcon {

[[nodiscard]] FALCON_API u64 GetThreadId() noexcept;

FALCON_API void SetThreadName(std::string name);

[[nodiscard]] FALCON_API std::string_view GetThreadName() noexcept;

} // namespace falcon

#endif // FALCON_INCLUDE_FALCON_UTILS_THREAD_UTILS_H