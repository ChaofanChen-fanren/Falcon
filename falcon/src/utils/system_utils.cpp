#include <algorithm>
#include <falcon/utils/system_utils.h>
#include <memory>
#include <sstream>
#include <vector>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <dbghelp.h>
#include <windows.h>

#else
#include <cxxabi.h>
#include <elfutils/libdwfl.h>
#include <execinfo.h>
#include <unistd.h>
#endif

namespace falcon {

std::chrono::steady_clock::duration GetElapsedTime() noexcept {
    static const auto START_TIME = std::chrono::steady_clock::now();
    return std::chrono::steady_clock::now() - START_TIME;
}

u64 GetFiberId() noexcept {
    return 0;
}

namespace {

#if defined(_WIN32)

std::mutex &GetDbgHelpMutex() {
    static std::mutex mutex;
    return mutex;
}

// Manages initialization and cleanup of the DbgHelp symbol handler.
class DbgHelpContext final {
  public:
    DbgHelpContext() : m_process(GetCurrentProcess()) {
        // Load symbols on demand, enable source lines, and request readable names.
        SymSetOptions(SymGetOptions() | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

        m_initialized = SymInitialize(m_process, nullptr, TRUE) == TRUE;
    }

    ~DbgHelpContext() {
        if (m_initialized) {
            SymCleanup(m_process);
        }
    }

    DbgHelpContext(const DbgHelpContext &) = delete;
    DbgHelpContext &operator=(const DbgHelpContext &) = delete;

    [[nodiscard]] HANDLE process() const noexcept {
        return m_process;
    }

    [[nodiscard]] bool initialized() const noexcept {
        return m_initialized;
    }

  private:
    HANDLE m_process;
    bool m_initialized{false};
};

DbgHelpContext &GetDbgHelpContext() {
    static DbgHelpContext context;
    return context;
}

// Writes a symbol name or address, followed by source information if available.
void WriteWindowsFrame(std::ostringstream &stream, const DbgHelpContext &context, void *frame, bool is_return_address) {
    auto address = static_cast<DWORD64>(reinterpret_cast<std::uintptr_t>(frame));

    // Move the lookup address back into the calling instruction's range.
    if (is_return_address && address > 0) {
        --address;
    }

    bool symbol_found = false;
    if (context.initialized()) {
        SYMBOL_INFO_PACKAGE symbol_package{};
        symbol_package.si.SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol_package.si.MaxNameLen = MAX_SYM_NAME;

        // displacement is the byte offset from the symbol's start.
        DWORD64 displacement = 0;
        if (SymFromAddr(context.process(), address, &displacement, &symbol_package.si) == TRUE) {
            stream << symbol_package.si.Name;
            if (displacement != 0) {
                stream << "+0x" << std::hex << displacement << std::dec;
            }
            symbol_found = true;
        }
    }

    // Keep the address useful even when symbol lookup fails.
    if (!symbol_found) {
        stream << "0x" << std::hex << address << std::dec;
    }

    if (context.initialized()) {
        IMAGEHLP_LINE64 line{};
        line.SizeOfStruct = sizeof(line);

        DWORD displacement = 0;
        if (SymGetLineFromAddr64(context.process(), address, &displacement, &line) == TRUE &&
            line.FileName != nullptr) {
            stream << " at " << line.FileName << ':' << line.LineNumber;
        }
    }
}

#else

// These resources require free() or dwfl_end(), rather than delete.
struct FreeDeleter final {
    void operator()(void *ptr) const noexcept {
        std::free(ptr);
    }
};

// Resolves runtime addresses to source file names and line numbers.
class SourceLocationParser final {
  public:
    SourceLocationParser() : m_dwfl(dwfl_begin(&DWFL_CALLBACKS)) {
        // Register the current process's modules and finalize the report.
        // Leave the parser disabled if any initialization step fails.
        if (!m_dwfl || dwfl_linux_proc_report(m_dwfl.get(), getpid()) ||
            dwfl_report_end(m_dwfl.get(), nullptr, nullptr)) {
            m_dwfl.reset();
        }
    }

    // Returns "file:line", or an empty string if unavailable.
    [[nodiscard]] std::string parse(void *frame, bool is_return_address) const {
        if (!m_dwfl) {
            return {};
        }

        auto address = static_cast<Dwarf_Addr>(reinterpret_cast<std::uintptr_t>(frame));

        // Move the lookup address back into the calling instruction's range.
        if (is_return_address && address > 0) {
            --address;
        }

        Dwfl_Line *line = dwfl_getsrc(m_dwfl.get(), address);
        if (line == nullptr) {
            return {};
        }

        int line_number = 0;
        const char *file_name = dwfl_lineinfo(line, nullptr, &line_number, nullptr, nullptr, nullptr);

        if (file_name == nullptr || line_number <= 0) {
            return {};
        }

        return std::string(file_name) + ':' + std::to_string(line_number);
    }

  private:
    struct DwflDeleter final {
        void operator()(Dwfl *dwfl) const noexcept {
            dwfl_end(dwfl);
        }
    };
    std::unique_ptr<Dwfl, DwflDeleter> m_dwfl;

    inline static constexpr Dwfl_Callbacks DWFL_CALLBACKS{
        dwfl_linux_proc_find_elf,     // Locate ELF files for process modules.
        dwfl_standard_find_debuginfo, // Locate debug information.
        nullptr,                      // No custom section address callback.
        nullptr,                      // Use the default debug information path.
    };
};

// Expected input: module(mangled_name+offset) [address]
std::string DemangleBacktraceSymbol(const char *symbol) {
    const std::string_view text(symbol == nullptr ? "" : symbol);
    const auto name_begin = text.find('(');
    if (name_begin == std::string_view::npos) {
        return std::string(text);
    }

    const auto mangled_begin = name_begin + 1;
    auto name_end = text.find('+', mangled_begin);
    if (name_end == std::string_view::npos) {
        name_end = text.find(')', mangled_begin);
    }

    if (name_end == std::string_view::npos || name_end == mangled_begin) {
        return std::string(text);
    }

    // Copy the extracted name into a null-terminated string.
    const std::string mangled_name(text.substr(mangled_begin, name_end - mangled_begin));

    // The demangler allocates a buffer that must be released with free().
    int status = 0;
    std::unique_ptr<char, FreeDeleter> demangled_name(
        abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status));

    if (status == 0 && demangled_name) {
        // Copy into the returned string before the buffer is freed.
        return demangled_name.get();
    }

    // Keep the extracted name if demangling fails.
    return mangled_name;
}

#endif

} // namespace

// size limits captured frames; skip omits leading frames from the output.
std::string GetBackTrace(int size, int skip, const std::string &prefix) {
    if (size <= 0) {
        return {};
    }

#if defined(_WIN32)

    const int capture_size = std::min(size, static_cast<int>(std::numeric_limits<USHORT>::max()));

    std::vector<void *> frames(static_cast<std::size_t>(capture_size));
    const int frame_count =
        static_cast<int>(CaptureStackBackTrace(0, static_cast<DWORD>(frames.size()), frames.data(), nullptr));

    if (frame_count <= 0) {
        return {};
    }

    std::ostringstream stream;
    const int first_frame = std::clamp(skip, 0, frame_count);

    // DbgHelp calls must be serialized across threads.
    // Hold the lock during shared initialization and all symbol lookups.
    std::lock_guard<std::mutex> lock(GetDbgHelpMutex());
    const DbgHelpContext &context = GetDbgHelpContext();

    for (int i = first_frame; i < frame_count; ++i) {
        stream << prefix << "#" << i - first_frame << " ";
        WriteWindowsFrame(stream, context, frames[i], i > 0);
        stream << '\n';
    }

    return stream.str();

#else

    std::vector<void *> frames(static_cast<std::size_t>(size));
    const int frame_count = ::backtrace(frames.data(), size);
    if (frame_count <= 0) {
        return {};
    }

    // backtrace_symbols() returns one allocation containing the string table
    // and its strings. Free only the returned pointer, not each string.
    std::unique_ptr<char *, FreeDeleter> symbols(::backtrace_symbols(frames.data(), frame_count));

    if (!symbols) {
        return {};
    }

    std::ostringstream stream;
    const SourceLocationParser source_location_parser;
    const int first_frame = std::clamp(skip, 0, frame_count);

    for (int i = first_frame; i < frame_count; ++i) {
        stream << prefix << "#" << i - first_frame << " " << DemangleBacktraceSymbol(symbols.get()[i]);

        // Missing source information does not prevent printing the frame.
        const std::string source_location = source_location_parser.parse(frames[i], i > 0);

        if (!source_location.empty()) {
            stream << " at " << source_location;
        }

        stream << '\n';
    }

    return stream.str();

#endif
}

} // namespace falcon
