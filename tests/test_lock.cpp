#include <atomic>
#include <falcon/types.h>
#include <iomanip>
#include <iostream>
#include <latch>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

int g_counter = 0;
int total_count = 0;
std::atomic<u64> g_atomicCounter{0};
std::mutex g_mutex;
std::shared_mutex g_shared_mutex;

template <typename Operation>
std::chrono::nanoseconds BenchmarkThreads(std::size_t thread_count, Operation &&operation) {
    std::latch ready(static_cast<ptrdiff_t>(thread_count));
    std::latch get_start(static_cast<ptrdiff_t>(1));
    std::latch finish(static_cast<ptrdiff_t>(thread_count));

    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (std::size_t thread_index = 0; thread_index < thread_count; ++thread_index) {
        threads.emplace_back([&, thread_index] {
            ready.count_down();
            get_start.wait();
            operation(thread_index, [thread_count](std::size_t thread_idx) {
                u64 avg = total_count / thread_count;
                u64 remainder = total_count % thread_count;
                return avg + (remainder > thread_idx ? 1 : 0);
            }(thread_index));
            finish.count_down();
        });
    }

    ready.wait();
    auto begin = std::chrono::steady_clock::now();
    get_start.count_down();
    finish.wait();
    auto end = std::chrono::steady_clock::now();

    for (std::thread &thread : threads) {
        thread.join();
    }

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
}

template <typename Benchmark> double Run(Benchmark &&benchmark) {
    constexpr int RUNS = 10;
    long double total_nanoseconds = 0.0;
    for (int i = 0; i < RUNS; i++) {
        total_nanoseconds += static_cast<long double>(benchmark().count());
    }
    return static_cast<double>(total_nanoseconds / RUNS);
}

void PrintResult(std::string_view name, double total_nanoseconds) {
    const double nanoseconds_per_operation = total_nanoseconds / static_cast<double>(g_counter);
    std::cout << std::left << std::setw(14) << name << std::right << total_nanoseconds << " ns \ttotal, "
              << nanoseconds_per_operation << " ns/op\n";
}

std::chrono::nanoseconds BenchmarkMutex(std::size_t thread_count) {
    g_counter = 0;
    const auto elapsed = BenchmarkThreads(thread_count, [](std::size_t, u64 operations) {
        for (u64 index = 0; index < operations; ++index) {
            std::lock_guard<std::mutex> lock(g_mutex);
            ++g_counter;
        }
    });
    return elapsed;
}

std::chrono::nanoseconds BenchmarkSharedMutex(std::size_t thread_count) {
    g_counter = 0;
    const auto elapsed = BenchmarkThreads(thread_count, [](std::size_t, u64 operations) {
        for (u64 index = 0; index < operations; ++index) {
            std::unique_lock<std::shared_mutex> lock(g_shared_mutex);
            ++g_counter;
        }
    });
    return elapsed;
}

std::chrono::nanoseconds BenchmarkAtomic(std::size_t thread_count) {
    g_atomicCounter.store(0, std::memory_order_relaxed);
    const auto elapsed = BenchmarkThreads(thread_count, [](std::size_t, u64 operations) {
        for (u64 index = 0; index < operations; ++index) {
            g_atomicCounter.fetch_add(1, std::memory_order_relaxed);
        }
    });
    return elapsed;
}

std::chrono::nanoseconds BenchmarkAtomicCas(std::size_t thread_count) {
    g_atomicCounter.store(0, std::memory_order_relaxed);
    const auto elapsed = BenchmarkThreads(thread_count, [](std::size_t, u64 operations) {
        for (u64 index = 0; index < operations; ++index) {
            u64 expected = g_atomicCounter.load(std::memory_order_relaxed);
            while (!g_atomicCounter.compare_exchange_weak(expected, expected + 1, std::memory_order_relaxed,
                                                          std::memory_order_relaxed)) {
            }
        }
    });
    return elapsed;
}

int main(int argc, char **argv) {

    if (argc != 2) {
        std::cout << "Usage:  ./test_lock count" << std::endl;
    }

    total_count = atoi(argv[1]);
    const std::vector<std::size_t> thread_counts = {1, 2, 4, 8, 10};

    for (auto thread_count : thread_counts) {
        std::cout << "threads: " << thread_count << std::endl;
        PrintResult("mutex", Run([&]() { return BenchmarkMutex(thread_count); }));
        PrintResult("shared mutex", Run([&]() { return BenchmarkSharedMutex(thread_count); }));
        PrintResult("atomic", Run([&]() { return BenchmarkAtomic(thread_count); }));
        PrintResult("atomic CAS", Run([&]() { return BenchmarkAtomicCas(thread_count); }));
        std::cout << '\n';
    }

    return 0;
}
