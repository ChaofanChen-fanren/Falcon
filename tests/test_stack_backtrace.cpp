#include <cstdlib>
#include <execinfo.h>
#include <falcon/utils/system_utils.h>
#include <iostream>
#include <memory>
#include <vector>

void func() {
    // int size = 10;
    // std::vector<void *> frames(static_cast<std::size_t>(size));
    // const int frame_count = ::backtrace(frames.data(), size);

    // if (frame_count <= 0) {
    //     std::cout << frame_count << std::endl;
    //     return;
    // }

    // std::unique_ptr<char *, decltype(&std::free)> symbols(::backtrace_symbols(frames.data(), frame_count),
    // &std::free); if (symbols == nullptr) {
    //     std::cout << frame_count << std::endl;
    //     return;
    // }

    // for (int i = 0; i < frame_count; i++) {
    //     std::cout << symbols.get()[i] << std::endl;
    // }
    std::string prefix = "$BackTrack$";
    std::cout << falcon::GetBackTrace(20, 0, prefix) << std::endl;
}

void funb() {
    func();
}

void funa() {
    funb();
}

int main() {
    funa();
}