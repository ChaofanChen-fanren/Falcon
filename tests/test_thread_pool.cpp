#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
  public:
    explicit ThreadPool(std::size_t thread_count) : stopping_(false) {
        if (thread_count == 0) {
            throw std::invalid_argument("thread_count must be positive");
        }

        try {
            for (std::size_t i = 0; i < thread_count; i++) {
                workers_.emplace_back([this] {
                    while (true) {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(mutex_);

                            cv_.wait(lock, [this]() { return stopping_ || !tasks_.empty(); });

                            if (stopping_ && tasks_.empty()) {
                                break;
                            }

                            task = std::move(tasks_.front());
                            tasks_.pop();
                        }

                        task();
                    }
                });
            }
        } catch (...) {
            stopAndJoin();
            throw;
        }
    }

    template <typename F, typename... Args>
    std::future<typename std::result_of<F(Args...)>::type> submit(F &&f, Args &&...args) {
        using ReturnType = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<ReturnType> result = task->get_future();
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stopping_) {
                throw std::runtime_error("thread pool is stopping");
            }
            tasks_.emplace([task] { (*task)(); });
        }
        cv_.notify_one();
        return result;
    }

    ~ThreadPool() {
        stopAndJoin();
    }

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

  private:
    void stopAndJoin() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopping_ = true;
        }

        cv_.notify_all();

        for (auto &worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex mutex_;
    std::condition_variable cv_;
    bool stopping_;
};

int main() {
    ThreadPool pool(4);

    auto result = pool.submit([](int a, int b) { return a + b; }, 10, 20);

    auto failed = pool.submit([]() -> int { throw std::runtime_error("task failed"); });

    std::cout << result.get() << '\n'; // 30

    try {
        failed.get();
    } catch (const std::exception &e) {
        std::cout << e.what() << '\n';
    }

    return 0;
}