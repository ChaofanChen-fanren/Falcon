#include <falcon/singleton.hpp>
#include <iostream>
#include <string>

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
    Logger::getInstance().Log("Singlton");
    return 0;
}