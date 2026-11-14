#ifndef FALCON_SRC_LOGGER_LOGGER_RECORD_H
#define FALCON_SRC_LOGGER_LOGGER_RECORD_H

#include <chrono>
#include <falcon/logger/logger_level.h>
#include <falcon/macros.h>

namespace falcon::detail {

// Holds the data for one log entry. The referenced strings must outlive this view.
struct LogRecordView {
    LoggerLevel::Level m_level = LoggerLevel::Level::LOGGER_LEVEL_DEBUG;
    std::string_view m_loggerName;
    std::string_view m_message;
    std::chrono::system_clock::time_point m_timestamp; // Wall-clock time of the entry.
    std::chrono::steady_clock::duration m_elapsed;     // Time since a steady-clock reference point.
    u64 m_threadId = 0;
    u64 m_fiberId = 0;
    std::string_view m_threadName;
    std::string_view m_fileName;
    u32 m_line = 0;
};

} // namespace falcon::detail

#endif // FALCON_SRC_LOGGER_LOGGER_RECORD_H
