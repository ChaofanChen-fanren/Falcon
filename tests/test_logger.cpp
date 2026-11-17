#include <falcon/logger/logger_level.h>
#include <iostream>

void test_logger_level() {
    using namespace std;
    using namespace falcon;
    cout << LoggerLevel::ToString(LoggerLevel::Level::LOGGER_LEVEL_DEBUG) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::Level::LOGGER_LEVEL_INFO) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::Level::LOGGER_LEVEL_WARNING) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::Level::LOGGER_LEVEL_ERROR) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::Level::LOGGER_LEVEL_FATAL) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::Level::LOGGER_LEVEL_OFF) << endl;
    cout << LoggerLevel::ToString(static_cast<LoggerLevel::Level>(10)) << endl;

    cout << LoggerLevel::ToString(LoggerLevel::FromString("DEBUG")) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::FromString("INFO")) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::FromString("WARNING")) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::FromString("ERROR")) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::FromString("FATAL")) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::FromString("OFF")) << endl;
    cout << LoggerLevel::ToString(LoggerLevel::FromString("UNKNOWN")) << endl; // default OFF
}

int main() {
    test_logger_level();
}