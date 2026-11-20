
#include <atomic>
#include <iostream>
#include <mutex>
#include <string>
template <typename T> class Singleton {
  public:
    static T *getInstance() {
        T *temp = m_instance.load(std::memory_order_acquire);
        if (temp == nullptr) {
            std::lock_guard<std::mutex> lock(m_mutex);
            temp = m_instance.load(std::memory_order_relaxed);
            if (temp == nullptr) {
                temp = new T();
                RegisterCleanup();
                m_instance.store(temp, std::memory_order_release);
            }
        }
        return temp;
    }

    Singleton(const Singleton &) = delete;
    Singleton(Singleton &&) = delete;
    Singleton &operator=(const Singleton &) = delete;
    Singleton &operator=(Singleton &&) = delete;

  protected:
    Singleton() = default;
    ~Singleton() = default;

  private:
    static void Destroy() noexcept {
        T *instance = m_instance.exchange(nullptr, std::memory_order_acquire);
        delete instance;
    }

    class Cleanup {
      public:
        Cleanup() = default;
        Cleanup(const Cleanup &) = delete;
        Cleanup &operator=(const Cleanup &) = delete;

        ~Cleanup() {
            Singleton<T>::Destroy();
        }
    };

    static void RegisterCleanup() {
        static Cleanup cleanup;
        (void)cleanup;
    }

    inline static std::atomic<T *> m_instance{};
    inline static std::mutex m_mutex;
};

class Logger : public Singleton<Logger> {
    friend class Singleton<Logger>;

  public:
    void Log(std::string msg) {
        std::cout << "[LOG]" << msg << std::endl;
    }

  private:
    Logger() {
        std::cout << "Logger()" << std::endl;
    }
    ~Logger() {
        std::cout << "~Logger()" << std::endl;
    }
};

int main() {
    Logger::getInstance()->Log("Singlton");
    return 0;
}