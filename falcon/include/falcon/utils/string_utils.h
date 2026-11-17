#ifndef FALCON_INCLUDE_FALCON_UTILS_STRING_UTILS_H
#define FALCON_INCLUDE_FALCON_UTILS_STRING_UTILS_H
#include <falcon/export.h>
#include <string_view>

namespace falcon {
class FALCON_API StringUtils final {
  public:
    StringUtils() = delete;

    [[nodiscard]] static std::string WStringToString(std::wstring_view text) noexcept;

    [[nodiscard]] static std::wstring StringToWString(std::string_view text) noexcept;

    inline static constexpr std::string_view ASCII_WHITESPACE{" \t\n\r\f\v"};

    [[nodiscard]] static std::string Trim(const std::string &text, std::string_view delimiters = ASCII_WHITESPACE);

    [[nodiscard]] static std::string_view Trim(std::string_view text,
                                               std::string_view delimiters = ASCII_WHITESPACE) noexcept;

    [[nodiscard]] static std::string TrimLeft(const std::string &text, std::string_view delimiters = ASCII_WHITESPACE);

    [[nodiscard]] static std::string_view TrimLeft(std::string_view text,
                                                   std::string_view delimiters = ASCII_WHITESPACE) noexcept;

    [[nodiscard]] static std::string TrimRight(const std::string &text, std::string_view delimiters = ASCII_WHITESPACE);

    [[nodiscard]] static std::string_view TrimRight(std::string_view text,
                                                    std::string_view delimiters = ASCII_WHITESPACE) noexcept;
};
} // namespace falcon

#endif // FALCON_INCLUDE_FALCON_UTILS_STRING_UTILS_H