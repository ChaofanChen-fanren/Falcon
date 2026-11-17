#ifndef FALCON_INCLUDE_FALCON_LEVEL_H
#define FALCON_INCLUDE_FALCON_LEVEL_H

#include <array>
#include <falcon/export.h>
#include <falcon/types.h>
#include <string_view>

namespace falcon {

class FALCON_API LoggerLevel final {
  public:
    enum class Level : u8 {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5,
        OFF = 6,
        N_LEVELS = 7
    };

    [[nodiscard]] static std::string_view ToString(LoggerLevel::Level level) noexcept;
    [[nodiscard]] static LoggerLevel::Level FromString(std::string_view name) noexcept;

  private:
    static constexpr auto m_levels_count = static_cast<std::size_t>(LoggerLevel::Level::N_LEVELS);
    // fixed-width 3-char labels - every level prints to exactly level_width bytes so
    // the formatter can patch a fixed-size slot in its cached header.
    static constexpr std::size_t m_level_width = 3;
    static constexpr std::array<std::string_view, m_levels_count> m_level_names{"TRC", "DBG", "INF", "WRN",
                                                                                "ERR", "FAT", "OFF"};
};

} // namespace falcon

#endif // FALCON_INCLUDE_FALCON_LEVEL_H