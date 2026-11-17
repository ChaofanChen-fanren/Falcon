#include <falcon/logger/level.h>
#include <iostream>

void test_logger_level() {
    using namespace std;
    using namespace falcon;
    cout << LoggerLevel::ToString(LoggerLevel::Level::DEBUG) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::Level::INFO) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::Level::WARN) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::Level::ERROR) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::Level::FATAL) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::Level::OFF) << endl;
    cout << LoggerLevel::ToString(static_cast<LoggerLevel::Level>(10)) << endl;

    cout << LoggerLevel::ToString(LoggerLevel::FromString("DBG")) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::FromString("INF")) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::FromString("WRN")) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::FromString("ERR")) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::FromString("FAT")) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::FromString("OFF")) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::FromString("UKN")) << endl; // default OFF
}

int main() {
    test_logger_level();
}