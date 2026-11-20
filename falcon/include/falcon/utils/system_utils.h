#ifndef FALCON_INCLUDE_FALCON_UTILS_SYSTEM_UTILS_H
#define FALCON_INCLUDE_FALCON_UTILS_SYSTEM_UTILS_H
#include <chrono>
#include <falcon/export.h>
#include <falcon/types.h>
#include <string>

namespace falcon {

[[nodiscard]] FALCON_API std::chrono::steady_clock::duration GetElapsedTime() noexcept;

[[nodiscard]] FALCON_API u64 GetFiberId() noexcept;

[[nodiscard]] FALCON_API std::string GetBackTrace(int size = 20, int skip = 1, const std::string &prefix = "");

} // namespace falcon

#endif // FALCON_INCLUDE_FALCON_UTILS_SYSTEM_UTILS_H