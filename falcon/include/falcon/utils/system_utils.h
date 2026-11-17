#ifndef FALCON_INCLUDE_FALCON_UTILS_SYSTEM_UTILS_H
#define FALCON_INCLUDE_FALCON_UTILS_SYSTEM_UTILS_H
#include <chrono>
#include <falcon/export.h>
#include <falcon/types.h>

namespace falcon {

[[nodiscard]] FALCON_API std::chrono::steady_clock::duration GetElapsedTime() noexcept;

[[nodiscard]] FALCON_API u64 GetFiberId() noexcept;

} // namespace falcon

#endif // FALCON_INCLUDE_FALCON_UTILS_SYSTEM_UTILS_H