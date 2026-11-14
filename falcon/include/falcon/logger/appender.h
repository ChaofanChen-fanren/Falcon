#ifndef FALCON_INCLUDE_FALCON_LOGGER_APPENDER_H
#define FALCON_INCLUDE_FALCON_LOGGER_APPENDER_H
#include <falcon/export.h>
#include <falcon/logger/logger_level.h>
#include <memory>
#include <string>

namespace falcon {

namespace detail {
class AppenderAccess;
}

class FALCON_API Appender final {
  public:
    class Impl;
    ~Appender() = default;

    Appender(const Appender &) = delete;
    Appender &operator=(const Appender &) = delete;
    Appender(Appender &&) = delete;
    Appender &operator=(Appender &&) = delete;

    void setLevel(LoggerLevel::Level level) noexcept;
    [[nodiscard]] LoggerLevel::Level getLevel() const noexcept;

    void flush();
    void sync();

  private:
    explicit Appender(std::unique_ptr<Impl> impl) noexcept;
    std::unique_ptr<Impl> m_impl;
    friend class detail::AppenderAccess;
};

using AppenderPtr = std::shared_ptr<Appender>;

[[nodiscard]] FALCON_API AppenderPtr MakeStdoutAppender();
[[nodiscard]] FALCON_API AppenderPtr MakeFileAppender(std::string file_name);

} // namespace falcon

#endif // FALCON_INCLUDE_FALCON_LOGGER_APPENDER_H