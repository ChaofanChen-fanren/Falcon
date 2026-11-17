#ifndef FALCON_INCLUDE_LOGGER_LOG_H
#define FALCON_INCLUDE_LOGGER_LOG_H
#include <falcon/export.h>
#include <falcon/logger/level.h>
#include <falcon/logger/logger.h>

namespace falcon::detail {
class FALCON_API LogLine final {
  public:
    LogLine(Logger &logger, LoggerLevel::Level level, u32 line, std::string_view file_name);
    ~LogLine();

    LogLine(const LogLine &) = delete;
    LogLine &operator=(const LogLine &) = delete;
    LogLine(LogLine &&) = delete;
    LogLine &operator=(LogLine &&) = delete;

    [[nodiscard]] std::ostream &stream() noexcept;

  private:
    struct Impl;
    static constexpr std::size_t LOG_LINE_IMPL_SIZE = 1024;
    alignas(std::max_align_t) std::byte m_impl_storage[LOG_LINE_IMPL_SIZE];

    [[nodiscard]] Impl &getImpl() noexcept;
};

FALCON_API void LogPrintf(Logger &logger, LoggerLevel::Level level, u32 line, std::string_view file_name,
                          const char *format, ...);

} // namespace falcon::detail

#define FALCON_LOG_LEVEL(logger, level)                                                                                \
    if (auto falcon_log_logger = (logger); !falcon_log_logger) {                                                       \
    } else if (const auto falcon_log_level = (level); !falcon_log_logger->shouldLog(falcon_log_level)) {               \
    } else                                                                                                             \
        falcon::detail::LogLine(*falcon_log_logger, falcon_log_level, __LINE__, __FILE__).stream()

#define FALCON_LOG_DEBUG(logger) FALCON_LOG_LEVEL((logger), falcon::LogLevel::Level::DEBUG)
#define FALCON_LOG_INFO(logger) FALCON_LOG_LEVEL((logger), falcon::LogLevel::Level::INFO)
#define FALCON_LOG_WARN(logger) FALCON_LOG_LEVEL((logger), falcon::LogLevel::Level::WARN)
#define FALCON_LOG_ERROR(logger) FALCON_LOG_LEVEL((logger), falcon::LogLevel::Level::ERROR)
#define FALCON_LOG_FATAL(logger) FALCON_LOG_LEVEL((logger), falcon::LogLevel::Level::FATAL)

#define PANS_LOG_FMT_LEVEL(logger, level, format, ...)                                                                 \
    if (auto falcon_log_logger = (logger); !falcon_log_logger) {                                                       \
    } else if (const auto falcon_log_level = (level); !falcon_log_logger->shouldLog(falcon_log_level)) {               \
    } else                                                                                                             \
        pans::detail::LogPrintf(*falcon_log_logger, falcon_log_level, __LINE__, __FILE__,                              \
                                (format)__VA_OPT__(, ) __VA_ARGS__)

#define FALCON_LOG_FMT_DEBUG(logger, format, ...)                                                                      \
    FALCON_LOG_FMT_LEVEL((logger), falcon::LogLevel::Level::DEBUG, (format)__VA_OPT__(, ) __VA_ARGS__)
#define PANS_LOG_FMT_INFO(logger, format, ...)                                                                         \
    PANS_LOG_FMT_LEVEL((logger), falcon::LogLevel::Level::INFO, (format)__VA_OPT__(, ) __VA_ARGS__)
#define PANS_LOG_FMT_WARN(logger, format, ...)                                                                         \
    PANS_LOG_FMT_LEVEL((logger), falcon::LogLevel::Level::WARN, (format)__VA_OPT__(, ) __VA_ARGS__)
#define PANS_LOG_FMT_ERROR(logger, format, ...)                                                                        \
    PANS_LOG_FMT_LEVEL((logger), falcon::LogLevel::Level::ERROR, (format)__VA_OPT__(, ) __VA_ARGS__)
#define PANS_LOG_FMT_FATAL(logger, format, ...)                                                                        \
    PANS_LOG_FMT_LEVEL((logger), falcon::LogLevel::Level::FATAL, (format)__VA_OPT__(, ) __VA_ARGS__)

#define FALCON_LOG_ROOT() falcon::GetRootLogger()
#define FALCON_LOG_NAME(name) falcon::GetLogger((name))

#define LOG_DEBUG FALCON_LOG_DEBUG(g_logger)
#define LOG_INFO FALCON_LOG_INFO(g_logger)
#define LOG_WARN FALCON_LOG_WARN(g_logger)
#define LOG_ERROR FALCON_LOG_ERROR(g_logger)
#define LOG_FATAL FALCON_LOG_FATAL(g_logger)

#endif // FALCON_INCLUDE_LOGGER_LOG_H