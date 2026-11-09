#ifndef FALCON_INCLUDE_FALCON_MUTEX_H
#define FALCON_INCLUDE_FALCON_MUTEX_H
#include <atomic>
#include <mutex>

#if defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#elif defined(__aarch64__) || defined(__arm__)
#include <arm_acle.h>
#endif

#if defined(__linux__) && defined(__GLIBC__)
#include <pthread.h>
#endif

inline void cpu_relax() noexcept {
#if defined(__x86_64__) || defined(__i386__)
    _mm_pause();
#elif defined(__aarch64__) || defined(__arm__)
    __asm__ __volatile__("yield" ::: "memory");
#endif
}

namespace falcon {
#if defined(__linux__) && defined(__GLIBC__)

class Spinlock {
  public:
    Spinlock() noexcept {
        pthread_spin_init(&m_mutex, PTHREAD_PROCESS_PRIVATE);
    }
    ~Spinlock() noexcept {
        pthread_spin_destroy(&m_mutex);
    }

    Spinlock(const Spinlock &) = delete;
    Spinlock &operator=(const Spinlock &) = delete;

    void lock() noexcept {
        pthread_spin_lock(&m_mutex);
    }

    [[nodiscard]] bool try_lock() {
        return pthread_spin_trylock(&m_mutex) == 0;
    }

    void unlock() {
        pthread_spin_unlock(&m_mutex);
    }

  private:
    pthread_spinlock_t m_mutex{};
};

#else

class Spinlock {
  public:
    using Lock = std::lock_guard<Spinlock>;

    Spinlock() noexcept = default;
    ~Spinlock() noexcept = default;

    Spinlock(const Spinlock &) = delete;
    Spinlock &operator=(const Spinlock &) = delete;

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
    std::atomic<bool> m_mutex{false};
}; // Spinlock

#endif

}; // namespace falcon

#endif // FALCON_INCLUDE_FALCON_MUTEX_H