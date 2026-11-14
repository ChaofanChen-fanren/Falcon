#ifndef FALCON_SRC_LOGGER_LOGGER_IMPL_H
#define FALCON_SRC_LOGGER_LOGGER_IMPL_H
#include "logger/formatter.h"
#include "logger/logger_record.h"
#include <atomic>
#include <falcon/logger/logger.h>
#include <shared_mutex>
#include <vector>

namespace falcon {
class Logger::Impl {
  public:
    explicit Impl(std::string name);

    [[nodiscard]] bool shouldLog(LoggerLevel::Level level) const noexcept;

    void setLevel(LoggerLevel::Level level) noexcept;
    [[nodiscard]] LoggerLevel::Level getLevel() const noexcept;

    [[nodiscard]] std::string_view getName() const noexcept;

    void setFormatter(std::shared_ptr<const detail::Formatter> formatter);
    [[nodiscard]] std::shared_ptr<const detail::Formatter> getFormatter() const noexcept;

    void addAppender(AppenderPtr appender);
    void removeAppender(const AppenderPtr &appender);
    void clearAppenders();

    void flush();
    void sync();

    void submit(const detail::LogRecordView &record) noexcept;
    void setRoot(const LoggerPtr &root) noexcept;

  private:
    std::string m_name;
    std::atomic<LoggerLevel::Level> m_level{LoggerLevel::Level::LOGGER_LEVEL_DEBUG};

    mutable std::shared_mutex m_mutex;
    std::shared_ptr<const detail::Formatter> m_formatter;
    std::vector<AppenderPtr> m_appenders;

    LoggerPtr m_root;

  private:
    static constexpr std::string_view DEFAULT_LOG_PATTERN =
        "%d{%Y-%m-%d %H:%M:%S}.%u%Tthread=%t%Tfiber=%F%T[%p]%T%f:%l%T%m%n";
};

namespace detail {
class LoggerAccess final {
  public:
    static void Submit(Logger &logger, const LogRecordView &record) noexcept;
    static void SetRoot(Logger &logger, const LoggerPtr &root) noexcept;
};
}; // namespace detail

} // namespace falcon

#endif // FALCON_SRC_LOGGER_LOGGER_IMPL_H