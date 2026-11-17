#ifndef FALCON_SRC_LOGGER_BUFFER_CONFIG_H
#define FALCON_SRC_LOGGER_BUFFER_CONFIG_H
#include <cstddef>
namespace falcon::detail {

inline constexpr std::size_t LOG_MESSAGE_INLINE_SIZE = 128;

inline constexpr std::size_t PRINTF_FORMAT_INLINE_SIZE = 128;

inline constexpr std::size_t FORMATTED_RECORD_INLINE_SIZE = 256;

} // namespace falcon::detail

#endif // FALCON_SRC_LOGGER_BUFFER_CONFIG_H
