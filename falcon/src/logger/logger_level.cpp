#include <algorithm>
#include <array>
#include <cctype>
#include <falcon/logger/logger_level.h>
#include <falcon/macros.h>
#include <string>

namespace falcon {
std::string_view LoggerLevel::ToString(LoggerLevel::Level level) noexcept {
    switch (level) {
    case Level::LOGGER_LEVEL_DEBUG:
        return "DEBUG";
    case Level::LOGGER_LEVEL_INFO:
        return "INFO";
    case Level::LOGGER_LEVEL_WARNING:
        return "WARNING";
    case Level::LOGGER_LEVEL_ERROR:
        return "ERROR";
    case Level::LOGGER_LEVEL_FATAL:
        return "FATAL";
    case Level::LOGGER_LEVEL_OFF:
        return "OFF";
    }
    return "UNKNOWN";
}

LoggerLevel::Level LoggerLevel::FromString(std::string_view value) noexcept {
    std::array<char, 8> normalized{};
    ASSERT_RETVAL(value.size() < normalized.size(), Level::LOGGER_LEVEL_OFF);

    std::transform(value.begin(), value.end(), normalized.begin(),
                   [](unsigned char character) { return static_cast<char>(std::toupper(character)); });

    const std::string_view upper_string(normalized.data(), value.size());
    if (upper_string == "DEBUG")
        return Level::LOGGER_LEVEL_DEBUG;
    if (upper_string == "INFO")
        return Level::LOGGER_LEVEL_INFO;
    if (upper_string == "WARNING")
        return Level::LOGGER_LEVEL_WARNING;
    if (upper_string == "ERROR")
        return Level::LOGGER_LEVEL_ERROR;
    if (upper_string == "FALTAL")
        return Level::LOGGER_LEVEL_FATAL;
    if (upper_string == "OFF")
        return Level::LOGGER_LEVEL_OFF;

    return Level::LOGGER_LEVEL_OFF;
}
} // namespace falcon
