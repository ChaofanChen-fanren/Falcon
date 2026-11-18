#include <falcon/logger/level.h>
#include <falcon/logger/log.h>
#include <filesystem>
#include <fstream>
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

void test_logger() {
    const std::filesystem::path output_path = std::filesystem::temp_directory_path() / "test_logger.log";
    std::ofstream(output_path, std::ios::binary | std::ios::trunc);

    auto logger = std::make_shared<falcon::Logger>("test");
    logger->addAppender(falcon::MakeStdoutAppender());
    logger->addAppender(falcon::MakeFileAppender(output_path.string()));

    FALCON_LOG_DEBUG(logger) << "debug message";
    FALCON_LOG_INFO(logger) << "info message";
    FALCON_LOG_WARN(logger) << "warning message";
    FALCON_LOG_ERROR(logger) << "error message";
    FALCON_LOG_FATAL(logger) << "fatal message";
    std::cout << "----------------------------------------------------" << std::endl;
    FALCON_LOG_FMT_DEBUG(logger, "debug message");
    FALCON_LOG_FMT_INFO(logger, "info message");
    FALCON_LOG_FMT_WARN(logger, "warn message");
    FALCON_LOG_FMT_ERROR(logger, "error message");
    FALCON_LOG_FMT_FATAL(logger, "fatal message");

    char c = 'a';
    int i = 42;
    const char msg1[] = "hello";
    const std::string msg2 = "world";
    const std::string_view msg3 = "falcon";

    auto g_logger = logger;
    std::cout << "----------------------------------------------------" << std::endl;
    LOG_INFO << "c=" << c << " i=" << i << " msg1: " << msg1 << " msg2: " << msg2 << " msg3: " << msg3;
    FALCON_LOG_FMT_INFO(logger, "c=%c i=%d msg1: %s msg2: %s msg3: %s", c, i, msg1, msg2.c_str(), msg3.data());

    logger->sync();
    std::cout << "\nThe same records were written to: " << output_path << '\n';
}

int main() {
    // test_logger_level();
    test_logger();
}