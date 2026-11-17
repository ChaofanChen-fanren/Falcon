#ifndef FALCON_SRC_LOGGER_BUFFER_H
#define FALCON_SRC_LOGGER_BUFFER_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <streambuf>
#include <string_view>
#include <vector>

namespace falcon::detail {

// Stack-allocated buffer with heap fallback.
// Typical log lines are ~120 bytes, so 250 bytes covers most without heap allocation.
template <std::size_t STACK_SIZE = 250> class InlineBuffer {
  public:
    InlineBuffer() = default;
    InlineBuffer(const InlineBuffer &) = delete;
    InlineBuffer &operator=(const InlineBuffer &) = delete;
    InlineBuffer(const InlineBuffer &&other) noexcept : m_heap_storage(std::move(other.m_heap_storage)) {
        if (other.m_heap) {
            m_heap = &m_heap_storage;
        } else {
            m_size = other.m_size;
            std::memcpy(m_stack, other.m_stack, m_size);
        }
        other.clear();
    }
    InlineBuffer &operator=(InlineBuffer &&) = delete;

    void append(const char *data, std::size_t size) {
        if (size == 0) {
            return;
        }

        // already on heap - append directly
        if (m_heap) {
            m_heap->append(data, size);
            return;
        }

        // fits in stack - memcpy
        if (m_size + size <= STACK_SIZE) {
            std::memcpy(m_stack + m_size, data, size);
            m_size += size;
            return;
        }

        // doesn't fit - move stack content to heap, then append
        spill_to_heap();
        m_heap->append(data, size);
    }

    void append(std::string_view value) {
        append(value.data(), value.size());
    }

    void append(char value) {
        append(&value, 1);
    }

    [[nodiscard]] const char *data() const noexcept {
        return m_heap ? m_heap_storage.data() : m_stack;
    }

    [[nodiscard]] char *data() noexcept {
        return m_heap ? m_heap_storage.data() : m_stack;
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return m_heap ? m_heap_storage.size() : m_size;
    }

    [[nodiscard]] std::string_view view() const noexcept {
        return {data(), size()};
    }

  private:
    void spill_to_heap() {
        m_heap_storage.assign(m_stack, m_size);
        m_heap = &m_heap_storage;
    }

    void clear() {
        m_size = 0;
        m_heap = nullptr;
    }

  private:
    // stack storage
    char m_stack[STACK_SIZE];
    std::size_t m_size{0};

    // heap storage
    std::string *m_heap{nullptr};
    std::string m_heap_storage;
};

// Adapts an InlineBuffer for use with std::ostream.
// The referenced buffer must outlive this adapter.
template <std::size_t STACK_SIZE> class InlineStreamBuffer final : public std::streambuf {
  public:
    explicit InlineStreamBuffer(InlineBuffer<STACK_SIZE> &buffer) noexcept : m_buffer(buffer) {
    }

  protected:
    std::streamsize xsputn(const char *data, std::streamsize size) override {
        if (size <= 0) {
            return 0;
        }

        m_buffer.append(data, static_cast<std::size_t>(size));
        return size;
    }

    int_type overflow(int_type character) override {
        // An EOF argument supplies no character to append.
        // return a non-EOF value to report success.
        if (traits_type::eq_int_type(character, traits_type::eof())) {
            return traits_type::not_eof(character);
        }

        m_buffer.append(traits_type::to_char_type(character));
        return character;
    }

  private:
    InlineBuffer<STACK_SIZE> &m_buffer;
};

} // namespace falcon::detail

#endif // FALCON_SRC_LOGGER_BUFFER_H
