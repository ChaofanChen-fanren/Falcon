#ifndef FALCON_INCLUDE_FALCON_LOGGER_LEVEL_H
#define FALCON_INCLUDE_FALCON_LOGGER_LEVEL_H

#include <falcon/export.h>
#include <falcon/types.h>
#include <string_view>

namespace falcon {

class FALCON_API LoggerLevel final {
  public:
    enum class Level : u8 {
        LOGGER_LEVEL_DEBUG = 1,
        LOGGER_LEVEL_INFO = 2,
        LOGGER_LEVEL_WARNING = 3,
        LOGGER_LEVEL_ERROR = 4,
        LOGGER_LEVEL_FATAL = 5,
        LOGGER_LEVEL_OFF = 6, // Disables all logging.
    };

    [[nodiscard]] static std::string_view ToString(LoggerLevel::Level level) noexcept;
    [[nodiscard]] static LoggerLevel::Level FromString(std::string_view value) noexcept;
};

} // namespace falcon

#endif // FALCON_INCLUDE_FALCON_LOGGER_LEVEL_H