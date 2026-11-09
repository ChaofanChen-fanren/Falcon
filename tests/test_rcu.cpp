#include <chrono>
#include <iostream>
#include <thread>
#include <urcu.h>
#include <vector>

struct Data {
    int value;
};

static Data *ptr;

int main() {
    // init data
    rcu_init();
    Data *init_ptr = new Data{0};
    rcu_assign_pointer(ptr, init_ptr);

    // reader threads
    std::vector<std::thread> readers;
    for (int id = 0; id < 5; id++) {
        readers.emplace_back([&, id] {
            while (true) {
                rcu_read_lock();
                Data *p = rcu_dereference(ptr);
                int value = p->value;
                rcu_read_unlock();

                std::cout << "id: " << id << " value: " << value << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        });
    }

    // one write thread
    auto write_thread = std::thread([&] {
        while (true) {
            Data *old_data_ptr = rcu_dereference(ptr);
            Data *new_data_ptr = new Data{0};

            *new_data_ptr = *old_data_ptr;
            new_data_ptr->value++;

            rcu_assign_pointer(ptr, new_data_ptr);
            std::cout << "write modify value" << ptr->value << std::endl;

            synchronize_rcu();
            delete old_data_ptr;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }
    });

    for (auto &reader : readers) {
        reader.join();
    }
    write_thread.join();
    delete ptr;
    return 0;
}