#ifndef FALCON_SRC_LOGGER_APPENDER_IMPL_H
#define FALCON_SRC_LOGGER_APPENDER_IMPL_H

#include <atomic>
#include <falcon/logger/appender.h>
#include <mutex>

namespace falcon {
class Appender::Impl {
  public:
    virtual ~Impl() = default;
    void append(LoggerLevel::Level level, std::string_view forrmatted_record) noexcept;
    void setLevel(LoggerLevel::Level level) noexcept;
    [[nodiscard]] LoggerLevel::Level getLevel() const noexcept;

    void flush();
    void sync();

  protected:
    virtual void writeUnlocked(std::string_view formatted_record) noexcept = 0;
    virtual void flushUnlocked() noexcept = 0;
    virtual void syncUnlocked() noexcept = 0;

  private:
    std::mutex m_mutex;
    std::atomic<LoggerLevel::Level> m_level{LoggerLevel::Level::LOGGER_LEVEL_DEBUG};
};

namespace detail {

class AppenderAccess final {
  public:
    [[nodiscard]] static AppenderPtr MakeStdoutAppender();
    [[nodiscard]] static AppenderPtr MakeFileAppender(std::string file_name);
    static void Append(const AppenderPtr &appender, LoggerLevel::Level level,
                       std::string_view formatted_record) noexcept;
};

} // namespace detail

} // namespace falcon

#endif // FALCON_SRC_LOGGER_APPENDER_IMPL_H