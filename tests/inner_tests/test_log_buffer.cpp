#include "logger/buffer.h"
#include "logger/buffer_config.h"
#include <array>
#include <atomic>
#include <chrono>
#include <falcon/types.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string_view>

constexpr int RUNS = 10;
u64 num_total = 0;
std::atomic<u64> BENCHMARK_SINK{0};

u64 Observe(std::string_view value) noexcept {
    return static_cast<u64>(value.size()) + static_cast<unsigned char>(value.front()) +
           static_cast<unsigned char>(value[value.size() / 2]) + static_cast<unsigned char>(value.back());
}

// innnerbuffer
u64 WriteWithSmallStreamBuffer(std::string_view message) {
    falcon::detail::InlineBuffer<falcon::detail::LOG_MESSAGE_INLINE_CAPACITY> buffer;
    falcon::detail::SmallStreamBuffer<falcon::detail::LOG_MESSAGE_INLINE_CAPACITY> stream_buffer(buffer);
    std::ostream in(&stream_buffer);
    in << message;
    return Observe(buffer.view());
}

// stringstream
u64 WriteWithStringStream(std::string_view message) {
    std::stringstream in;
    in << message;
    return Observe(std::string_view(in.str()));
}

template <typename Operation> std::chrono::nanoseconds BenchmarkWrites(Operation &&operation) {
    u64 checksum = 0;
    const auto begin = std::chrono::steady_clock::now();
    for (u64 iteration = 0; iteration < num_total; ++iteration) {
        checksum += operation();
    }
    const auto end = std::chrono::steady_clock::now();
    BENCHMARK_SINK.fetch_xor(checksum, std::memory_order_relaxed);
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
}

template <typename Benchmark> double Run(Benchmark &&benchmark) {
    long double total_nanoseconds = 0.0L;
    for (int i = 0; i < RUNS; i++) {
        total_nanoseconds += static_cast<long double>(benchmark().count());
    }
    return static_cast<double>(total_nanoseconds / RUNS);
}

void PrintResult(std::string_view name, double total_nanoseconds) {
    std::cout << std::left << std::setw(14) << name << std::right << total_nanoseconds << " ns \ttotal, "
              << total_nanoseconds / static_cast<double>(num_total) << " ns/op\n";
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./test_log_buffer count\n";
        return 1;
    }
    num_total = std::stoull(argv[1]);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "operations: " << num_total << ", average of " << RUNS << " runs\n\n";
    constexpr std::array<std::size_t, 6> MESSAGE_SIZES = {50, 100, 200, 500, 1000, 2048};
    for (const std::size_t message_size : MESSAGE_SIZES) {
        const std::string message(message_size, 'x');
        const double small_stream_buffer_nanoseconds =
            Run([&]() { return BenchmarkWrites([&]() { return WriteWithSmallStreamBuffer(message); }); });
        const double stringstream_nanoseconds =
            Run([&]() { return BenchmarkWrites([&]() { return WriteWithStringStream(message); }); });

        std::cout << "message: " << message_size << " characters" << '\n';
        PrintResult("SmallStreamBuffer", small_stream_buffer_nanoseconds);
        PrintResult("std::stringstream", stringstream_nanoseconds);
        std::cout << "speedup: " << stringstream_nanoseconds / small_stream_buffer_nanoseconds << "x\n\n";
    }
}