#include <algorithm>
#include <array>
#include <cctype>
#include <falcon/logger/level.h>
#include <falcon/macros.h>
#include <string>

namespace falcon {

std::string_view LoggerLevel::ToString(LoggerLevel::Level lvl) noexcept {
    const auto index = static_cast<std::size_t>(lvl);
    ASSERT_RETVAL_MSG(index < m_levels_count, m_level_names[static_cast<std::size_t>(Level::INFO)],
                      "Invalid log level: " << index);
    return m_level_names[index];
}

LoggerLevel::Level LoggerLevel::FromString(std::string_view name) noexcept {
    auto it =
        std::find_if(std::begin(m_level_names), std::end(m_level_names), [&name](const std::string_view &level_name) {
            return level_name.size() == name.size() &&
                   std::equal(name.begin(), name.end(), level_name.begin(),
                              [](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); });
        });

    if (it != std::end(m_level_names))
        return static_cast<LoggerLevel::Level>(std::distance(std::begin(m_level_names), it));
    return LoggerLevel::Level::OFF;
}
} // namespace falcon
