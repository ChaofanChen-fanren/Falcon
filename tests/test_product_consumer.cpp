#include <condition_variable>
#include <iostream>
#include <latch>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

template <typename T> class BlockingQueue {
  public:
    explicit BlockingQueue(std::size_t capacity) : capacity_(capacity) {
        if (capacity_ == 0) {
            throw std::invalid_argument("capacity must be positive");
        }
    }

    bool push(T val) {
        std::unique_lock<std::mutex> lock(mutex_);

        not_full_.wait(lock, [this] { return closed_ || queue_.size() < capacity_; });

        if (closed_) {
            return false;
        }

        queue_.push(std::move(val));

        lock.unlock();
        not_empty_.notify_one();
        return true;
    }

    bool pop(T &val) {
        std::unique_lock<std::mutex> lock(mutex_);

        not_empty_.wait(lock, [this]() { return closed_ || !queue_.empty(); });

        if (queue_.empty()) {
            return false;
        }

        val = std::move(queue_.front());
        queue_.pop();

        lock.unlock();
        not_full_.notify_one();
        return true;
    }

    void close() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        not_empty_.notify_all();
        not_full_.notify_all();
    }

  private:
    std::queue<T> queue_;
    const std::size_t capacity_;
    bool closed_;

    std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
};

int main() {

    BlockingQueue<int> queue(3);
    std::mutex output_mutex;

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    int thread_count = 4;
    std::latch ready(static_cast<ptrdiff_t>(thread_count));
    std::latch get_start(static_cast<ptrdiff_t>(1));

    for (int i = 0; i < 2; ++i) {
        consumers.emplace_back([&, i] {
            int value;
            ready.count_down();
            get_start.wait();
            while (queue.pop(value)) {
                std::lock_guard<std::mutex> lock(output_mutex);
                std::cout << "consumer " << i << ": " << value << '\n';
            }
        });
    }

    for (int i = 0; i < 2; ++i) {
        producers.emplace_back([&, i] {
            ready.count_down();
            get_start.wait();
            for (int j = 0; j < 5; ++j) {
                queue.push(i * 100 + j);
            }
        });
    }

    ready.wait();
    get_start.count_down();

    for (auto &producer : producers) {
        producer.join();
    }

    queue.close();

    for (auto &consumer : consumers) {
        consumer.join();
    }

    return 0;
}