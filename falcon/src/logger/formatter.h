#ifndef FALCON_SRC_LOGGER_FORMATTER_H
#define FALCON_SRC_LOGGER_FORMATTER_H

#include "logger/buffer.h"
#include "logger/buffer_config.h"
#include "logger/logger_record.h"
#include <memory>
#include <string>

namespace falcon::detail {

using FormattedRecordBuffer = InlineBuffer<FORMATTED_RECORD_INLINE_CAPACITY>;

class Formatter {
  public:
    class FormatItem {
      public:
        virtual ~FormatItem() = default;
        virtual void format(const LogRecordView &record, FormattedRecordBuffer &output) const = 0;
    };

    explicit Formatter(std::string_view pattern);
    void format(const LogRecordView &record, FormattedRecordBuffer &output) const;
    [[nodiscard]] const std::string &getPattern() const noexcept {
        return m_pattern;
    }

  private:
    int parse();
    void addLiteral(std::string &literal);

  private:
    std::string m_pattern;
    std::vector<std::unique_ptr<FormatItem>> m_items;
};

} // namespace falcon::detail

#endif // FALCON_SRC_LOGGER_FORMATTER_H