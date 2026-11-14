#include "logger/appender_impl.h"
#include <falcon/logger/appender.h>
#include <iostream>
#include <system_error>

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

namespace falcon {

Appender::Appender(std::unique_ptr<Impl> impl) noexcept : m_impl(std::move(impl)) {
}

void Appender::setLevel(LoggerLevel::Level level) noexcept {
    m_impl->setLevel(level);
}

LoggerLevel::Level Appender::getLevel() const noexcept {
    return m_impl->getLevel();
}

void Appender::flush() {
    m_impl->flush();
}

void Appender::sync() {
    m_impl->sync();
}

void Appender::Impl::append(LoggerLevel::Level level, std::string_view formatted_record) noexcept {
    if (static_cast<u8>(level) < static_cast<u8>(getLevel())) {
        return;
    }

    // std::mutex constructor function isn't noexcept
    try {
        std::lock_guard<std::mutex> lock(m_mutex);
        writeUnlocked(formatted_record);
        if (level == LoggerLevel::Level::LOGGER_LEVEL_FATAL) {
            flushUnlocked();
        }
    } catch (...) {
        constexpr std::string_view message = "pans logger: appender operation failed\n";
        std::fwrite(message.data(), 1, message.size(), stderr);
    }
}

void Appender::Impl::setLevel(LoggerLevel::Level level) noexcept {
    m_level.store(level, std::memory_order_release);
}

LoggerLevel::Level Appender::Impl::getLevel() const noexcept {
    return m_level.load(std::memory_order_acquire);
}

// io buffer -> os buffer
void Appender::Impl::flush() {
    std::lock_guard<std::mutex> lock(m_mutex);
    flushUnlocked();
}

// os buffer -> io buffer
void Appender::Impl::sync() {
    std::lock_guard<std::mutex> lock(m_mutex);
    syncUnlocked();
}

class StdoutAppenderImpl final : public Appender::Impl {
  protected:
    void writeUnlocked(std::string_view formatted_record) noexcept override {
        std::cout.write(formatted_record.data(), static_cast<std::streamsize>(formatted_record.size()));
    }

    void flushUnlocked() noexcept override {
        std::cout.flush();
    }

    void syncUnlocked() noexcept override {
        flushUnlocked();
    }
};

class FileAppenderImpl final : public Appender::Impl {
  public:
    explicit FileAppenderImpl(const std::string &file_name) {
        if (file_name.empty()) {
            throw std::invalid_argument("logger file name cannot be empty");
        }
        m_file = std::fopen(file_name.c_str(), "ab");
        if (m_file == nullptr) {
            throw std::system_error(errno, std::generic_category(), "failed to open logger file: " + file_name);
        }
    }
    ~FileAppenderImpl() override {
        if (m_file != nullptr) {
            std::fflush(m_file);
            std::fclose(m_file);
        }
    }

  protected:
    void writeUnlocked(std::string_view formatted_record) noexcept override {
        std::fwrite(formatted_record.data(), 1, formatted_record.size(), m_file);
    }

    void flushUnlocked() noexcept override {
        std::fflush(m_file);
    }

    void syncUnlocked() noexcept override {
        flushUnlocked();
#if defined(_WIN32)
        ::_commit(::_fileno(m_file));
#else
        ::fdatasync(::fileno(m_file));
#endif
    }

  private:
    std::FILE *m_file = nullptr;
};

AppenderPtr detail::AppenderAccess::MakeStdoutAppender() {
    return AppenderPtr(new Appender(std::make_unique<StdoutAppenderImpl>()));
}

AppenderPtr detail::AppenderAccess::MakeFileAppender(std::string file_name) {
    return AppenderPtr(new Appender(std::make_unique<FileAppenderImpl>(file_name)));
}

void detail::AppenderAccess::Append(const AppenderPtr &appender, LoggerLevel::Level level,
                                    std::string_view formatted_record) noexcept {
    if (appender != nullptr) {
        appender->m_impl->append(level, formatted_record);
    }
}

AppenderPtr MakeStdoutAppender() {
    return detail::AppenderAccess::MakeStdoutAppender();
}

AppenderPtr MakeFileAppender(std::string file_name) {
    return detail::AppenderAccess::MakeFileAppender(std::move(file_name));
}

}; // namespace falcon