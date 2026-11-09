#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <latch>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <falcon/macros.h>
#include <falcon/mutex.h>
#include <falcon/types.h>

constexpr int RUNS = 10;
u64 g_counter = 0;
u64 total_count = 0;

class Spinlock_atomic_bool {
  public:
    Spinlock_atomic_bool() noexcept = default;
    ~Spinlock_atomic_bool() noexcept = default;

    Spinlock_atomic_bool(const Spinlock_atomic_bool &) = delete;
    Spinlock_atomic_bool &operator=(const Spinlock_atomic_bool &) = delete;

    void lock() noexcept {
        while (m_mutex.exchange(true, std::memory_order_acquire)) {
            while (m_mutex.load(std::memory_order_relaxed)) {
                cpu_relax();
            }
        }
    }

    [[nodiscard]] bool try_lock() noexcept {
        return !m_mutex.exchange(true, std::memory_order_acquire);
    }

    void unlock() noexcept {
        m_mutex.store(false, std::memory_order_release);
    }

  private:
    std::atomic_bool m_mutex{false};
}; // Spinlock_atomic_bool

class Spinlock_atomic_bool_wait {
  public:
    Spinlock_atomic_bool_wait() noexcept = default;
    ~Spinlock_atomic_bool_wait() noexcept = default;

    Spinlock_atomic_bool_wait(const Spinlock_atomic_bool_wait &) = delete;
    Spinlock_atomic_bool_wait &operator=(const Spinlock_atomic_bool_wait &) = delete;

    void lock() noexcept {
        bool expected = false;
        while (!m_mutex.compare_exchange_weak(expected, true, std::memory_order_acquire, std::memory_order_relaxed)) {
            std::atomic_wait_explicit(&m_mutex, true, std::memory_order_relaxed);
            expected = false;
        }
    }

    [[nodiscard]] bool try_lock() noexcept {
        bool expected = false;
        return m_mutex.compare_exchange_strong(expected, true, std::memory_order_acquire, std::memory_order_relaxed);
    }

    void unlock() noexcept {
        m_mutex.store(false, std::memory_order_release);
        std::atomic_notify_one(&m_mutex);
    }

  private:
    std::atomic_bool m_mutex{false};
};

class Spinlock {
  public:
    Spinlock() noexcept = default;
    ~Spinlock() noexcept = default;

    Spinlock(const Spinlock &) = delete;
    Spinlock &operator=(const Spinlock &) = delete;

    void lock() noexcept {
        while (m_mutex.test_and_set(std::memory_order_acquire)) {
            while (m_mutex.test(std::memory_order_relaxed)) {
                cpu_relax();
            }
        }
    }

    [[nodiscard]] bool try_lock() noexcept {
        return !m_mutex.test_and_set(std::memory_order_acquire);
    }

    void unlock() noexcept {
        m_mutex.clear(std::memory_order_release);
    }

  private:
    std::atomic_flag m_mutex = ATOMIC_FLAG_INIT;
};

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

    long double total_nanoseconds = 0.0;
    for (int i = 0; i < RUNS; i++) {
        total_nanoseconds += static_cast<long double>(benchmark().count());
    }
    return static_cast<double>(total_nanoseconds / RUNS);
}

void PrintResult(std::string_view name, double total_nanoseconds) {
    const double nanoseconds_per_operation = total_nanoseconds / static_cast<double>(g_counter);
    std::cout << std::left << std::setw(20) << name << std::right << total_nanoseconds << " ns \ttotal, "
              << nanoseconds_per_operation << " ns/op\n";
}

template <typename Mutex> std::chrono::nanoseconds BenchmarkMutex(std::size_t thread_count) {
    g_counter = 0;
    Mutex mutex;
    const auto elapsed = BenchmarkThreads(thread_count, [&mutex](std::size_t, u64 operations) {
        for (u64 index = 0; index < operations; ++index) {
            std::lock_guard<Mutex> lock(mutex);
            ++g_counter;
        }
    });
    if (g_counter != total_count) {
        throw std::runtime_error("mutex correctness check failed");
    }
    return elapsed;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./test_spin_lock count\n";
        return EXIT_FAILURE;
    }
    total_count = std::stoull(argv[1]);
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "operations: " << total_count << ", average of " << RUNS << " runs\n\n";

    const std::vector<std::size_t> thread_counts = {1, 2, 4, 8, 10};
    for (const std::size_t thread_count : thread_counts) {
        std::cout << "threads: " << thread_count << '\n';
        PrintResult("pthread_spinlock",
                    Run([&]() { return BenchmarkMutex<falcon::Spinlock>(thread_count); }));
        PrintResult("atomic_flag", Run([&]() { return BenchmarkMutex<Spinlock>(thread_count); }));
        PrintResult("atomic_wait", Run([&]() { return BenchmarkMutex<Spinlock_atomic_bool_wait>(thread_count); }));
        PrintResult("atomic_bool", Run([&]() { return BenchmarkMutex<Spinlock_atomic_bool>(thread_count); }));
        PrintResult("std::mutex", Run([&]() { return BenchmarkMutex<std::mutex>(thread_count); }));
        std::cout << '\n';
    }

    return EXIT_SUCCESS;
}