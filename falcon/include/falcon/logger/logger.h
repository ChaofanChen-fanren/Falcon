#ifndef FALCON_INCLUDE_LOGGER_LOGGER_H
#define FALCON_INCLUDE_LOGGER_LOGGER_H
#include <falcon/export.h>
#include <falcon/logger/appender.h>
#include <falcon/logger/logger_level.h>
#include <memory>
#include <string>

namespace falcon {
namespace detail {
class LoggerAccess;
} // namespace detail

class FALCON_API Logger final {
  public:
    class Impl;
    explicit Logger(std::string name = "root");
    ~Logger();

    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger &operator=(Logger &&);

    [[nodiscard]] bool shouldLog(LoggerLevel::Level level) const noexcept;

    void setLevel(LoggerLevel::Level level) noexcept;
    [[nodiscard]] LoggerLevel::Level getLevel() const noexcept;

    [[nodiscard]] std::string_view getName() const noexcept;

    void setFormatter(std::string_view pattern);
    [[nodiscard]] std::string getFormatterPattern() const;

    void addAppender(AppenderPtr appender);
    void removeAppender(const AppenderPtr &appender);
    void clearAppenders();

    void flush();
    void sync();

  private:
    std::unique_ptr<Impl> m_impl;
    friend class detail::LoggerAccess;
};

using LoggerPtr = std::shared_ptr<Logger>;
[[nodiscard]] FALCON_API LoggerPtr GetRootLogger();
[[nodiscard]] FALCON_API LoggerPtr GetLogger(std::string_view name);

} // namespace falcon

#endif // FALCON_INCLUDE_LOGGER_LOGGER_H