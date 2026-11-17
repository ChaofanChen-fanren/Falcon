#ifndef FALCON_INCLUDE_LOGGER_LOG_LINE_H
#define FALCON_INCLUDE_LOGGER_LOG_LINE_H
#include <falcon/export.h>
#include <falcon/logger/logger.h>
#include <falcon/logger/logger_level.h>

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
    alignas(std::max_align_t) std::byte m_implStorage[LOG_LINE_IMPL_SIZE];

    [[nodiscard]] Impl &getImpl() noexcept;
};

FALCON_API void LogPrintf(Logger &logger, LoggerLevel::Level level, u32 line, std::string_view file_name,
                          const char *format, ...);

} // namespace falcon::detail

#endif // FALCON_INCLUDE_LOGGER_LOG_LINE_H