#include "gtest/gtest.h"
#include "vertexnova/logging/logging.h"

#include <filesystem>
#include <fstream>

using namespace vne;
namespace fs = std::filesystem;

namespace {
constexpr const char* kTestDir = "test_dir";

CREATE_VNE_LOGGER_CATEGORY("logging.system.test");

// Redirects cout to a string stream for capturing console output
class CoutRedirect {
   public:
    CoutRedirect(std::streambuf* new_buffer)
        : old_(std::cout.rdbuf(new_buffer)) {}

    ~CoutRedirect() { std::cout.rdbuf(old_); }

   private:
    std::streambuf* old_;
};

}  // namespace

// Define a test fixture for Logging
class LoggingSystemTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Redirect std::cout to capture console output
        redirect_ = new CoutRedirect(cout_buffer_.rdbuf());
    }

    void TearDown() override {
        // Remove the test directory and clean up
        try {
            std::filesystem::remove_all(kTestDir);
        } catch (const std::exception& e) {
            std::cerr << "Exception during TearDown(): " << e.what() << std::endl;
        }

        delete redirect_;
    }

   protected:
    std::stringstream cout_buffer_;
    CoutRedirect* redirect_;
};

TEST_F(LoggingSystemTest, StartUpAndShutDown) {
    ASSERT_NO_THROW(log::Logging::initialize("test_logger"));
    log::Logging::shutdown();
}

TEST_F(LoggingSystemTest, ConfigureLoggerAndLogMessage) {
    std::string test_file = "test_file.log";
    std::string msg = "Testing output";
    log::LoggerConfig cfg;
    cfg.name = log::kDefaultLoggerName;
    cfg.console_pattern = "%x [%l] %v";
    cfg.file_pattern = "%x [%l] [%!] %v";
    cfg.log_level = log::LogLevel::eInfo;
    cfg.flush_level = log::LogLevel::eInfo;
    cfg.async = false;

#ifdef VNE_PLATFORM_WEB
    // Web platform: console-only logging
    cfg.sink = log::LogSinkType::eConsole;
    cfg.file_path = "";
#else
    // Desktop platforms: both console and file logging
    cfg.sink = log::LogSinkType::eBoth;
    cfg.file_path = log::Logging::createLoggingFolder(kTestDir, test_file);
#endif

    log::Logging::configureLogger(cfg);
    VNE_LOG_INFO << msg;

    // Verify console sink setup
    std::string output = cout_buffer_.str();
    EXPECT_NE(output.find(msg), std::string::npos);

#ifndef VNE_PLATFORM_WEB
    // Verify file sink setup (only on desktop platforms)
    std::ifstream infile(cfg.file_path);
    std::string line;
    ASSERT_TRUE(std::getline(infile, line));
    EXPECT_NE(line.find(msg), std::string::npos);
#endif
}

TEST_F(LoggingSystemTest, CreateLoggingFolder) {
#ifndef VNE_PLATFORM_WEB
    // Skip this test on web platform since file logging is not supported
    std::string base_dir = kTestDir;
    std::string filename = "test.log";
    std::string log_file_path = log::Logging::createLoggingFolder(base_dir, filename);
    EXPECT_FALSE(log_file_path.empty());
#else
    // Web platform: file logging not supported, test passes
    GTEST_SKIP() << "File logging not supported on web platform";
#endif
}

TEST_F(LoggingSystemTest, MixedSyncAndAsyncLoggers) {
    log::Logging::initialize("sync.logger", false);

    // Configure a synchronous logger
    log::LoggerConfig sync_cfg;
    sync_cfg.name = "sync.logger";
    sync_cfg.sink = log::LogSinkType::eConsole;
    sync_cfg.async = false;
    log::Logging::configureLogger(sync_cfg);

    // Configure an asynchronous logger
    log::LoggerConfig async_cfg;
    async_cfg.name = "async.logger";
    async_cfg.sink = log::LogSinkType::eConsole;
    async_cfg.async = true;
    log::Logging::configureLogger(async_cfg);

    // Verify the async state of each logger
    EXPECT_FALSE(log::Logging::isLoggerAsync("sync.logger"));
    EXPECT_TRUE(log::Logging::isLoggerAsync("async.logger"));

    // Test non-existent logger
    EXPECT_FALSE(log::Logging::isLoggerAsync("nonexistent.logger"));

    log::Logging::shutdown();
}

TEST_F(LoggingSystemTest, LoggerSpecificMacros) {
    // Create two loggers with different settings
    std::string logger1_name = "test_logger1";
    std::string logger2_name = "test_logger2";
    std::string test_file1 = "logger1.log";
    std::string test_file2 = "logger2.log";
    std::string cat1 = "category1";
    std::string cat2 = "category2";

    // Create the first logger
    log::LoggerConfig cfg1;
    cfg1.name = logger1_name;
    cfg1.console_pattern = "[L1] %v";
    cfg1.file_pattern = "[L1-FILE] %v";
    cfg1.log_level = log::LogLevel::eInfo;
    cfg1.flush_level = log::LogLevel::eError;
    cfg1.async = false;

#ifdef VNE_PLATFORM_WEB
    // Web platform: console-only logging
    cfg1.sink = log::LogSinkType::eConsole;
    cfg1.file_path = "";
#else
    // Desktop platforms: both console and file logging
    cfg1.sink = log::LogSinkType::eBoth;
    cfg1.file_path = log::Logging::createLoggingFolder(kTestDir, test_file1);
#endif

    log::Logging::configureLogger(cfg1);

    // Create the second logger
    log::LoggerConfig cfg2;
    cfg2.name = logger2_name;
    cfg2.console_pattern = "[L2] %v";
    cfg2.file_pattern = "[L2-FILE] %v";
    cfg2.log_level = log::LogLevel::eInfo;
    cfg2.flush_level = log::LogLevel::eError;
    cfg2.async = false;

#ifdef VNE_PLATFORM_WEB
    // Web platform: console-only logging
    cfg2.sink = log::LogSinkType::eConsole;
    cfg2.file_path = "";
#else
    // Desktop platforms: both console and file logging
    cfg2.sink = log::LogSinkType::eBoth;
    cfg2.file_path = log::Logging::createLoggingFolder(kTestDir, test_file2);
#endif

    log::Logging::configureLogger(cfg2);

    // Reset the console capture
    cout_buffer_.str("");

    // Use logger-specific macros
    CREATE_VNE_LOGGER_CATEGORY("test_category");

    // Log to logger 1
    _VNE_LOG_INFO_L(logger1_name.c_str()) << "Message to logger 1";

    // Log to logger 2
    _VNE_LOG_INFO_L(logger2_name.c_str()) << "Message to logger 2";

    // Log with explicit category
    _VNE_LOG_INFO_LC(logger1_name.c_str(), cat1) << "Message to logger 1 with category 1";
    _VNE_LOG_INFO_LC(logger2_name.c_str(), cat2) << "Message to logger 2 with category 2";

    log::Logging::shutdown();  // Ensure all loggers are properly shutdown

    // Verify console output
    std::string output = cout_buffer_.str();
    EXPECT_NE(output.find("[L1] Message to logger 1"), std::string::npos);
    EXPECT_NE(output.find("[L2] Message to logger 2"), std::string::npos);
    EXPECT_NE(output.find("[L1] Message to logger 1 with category 1"), std::string::npos);
    EXPECT_NE(output.find("[L2] Message to logger 2 with category 2"), std::string::npos);

#ifndef VNE_PLATFORM_WEB
    // Verify file output (only on desktop platforms)
    std::ifstream infile1(cfg1.file_path);
    std::string line1;
    ASSERT_TRUE(std::getline(infile1, line1));
    EXPECT_NE(line1.find("[L1-FILE] Message to logger 1"), std::string::npos);

    std::ifstream infile2(cfg2.file_path);
    std::string line2;
    ASSERT_TRUE(std::getline(infile2, line2));
    EXPECT_NE(line2.find("[L2-FILE] Message to logger 2"), std::string::npos);
#endif
}

// Test to demonstrate using two different loggers with custom macros
TEST_F(LoggingSystemTest, DefaultAndClientLoggers) {
    // Create a default logger (vnelogging)
    log::LoggerConfig default_cfg;
    default_cfg.name = log::kDefaultLoggerName;
    default_cfg.sink = log::LogSinkType::eConsole;
    default_cfg.console_pattern = "[DEFAULT] %v";
    default_cfg.log_level = log::LogLevel::eInfo;
    default_cfg.async = false;
    log::Logging::configureLogger(default_cfg);

    // Create a client logger
    const char* kClientLoggerName = "client_logger";
    log::LoggerConfig client_cfg;
    client_cfg.name = kClientLoggerName;
    client_cfg.sink = log::LogSinkType::eConsole;
    client_cfg.console_pattern = "[CLIENT] %v";
    client_cfg.log_level = log::LogLevel::eInfo;
    client_cfg.async = false;
    log::Logging::configureLogger(client_cfg);

    // Reset the console capture
    cout_buffer_.str("");

// Define custom macros for client logger
#define CLIENT_LOG_TRACE _VNE_LOG_TRACE_L(kClientLoggerName)
#define CLIENT_LOG_DEBUG _VNE_LOG_DEBUG_L(kClientLoggerName)
#define CLIENT_LOG_INFO _VNE_LOG_INFO_L(kClientLoggerName)
#define CLIENT_LOG_WARN _VNE_LOG_WARN_L(kClientLoggerName)
#define CLIENT_LOG_ERROR _VNE_LOG_ERROR_L(kClientLoggerName)
#define CLIENT_LOG_FATAL _VNE_LOG_FATAL_L(kClientLoggerName)

    // Use default logger macros
    VNE_LOG_INFO << "Message using default logger";
    VNE_LOG_ERROR << "Error message using default logger";

    // Use client logger macros
    CLIENT_LOG_INFO << "Message using client logger";
    CLIENT_LOG_ERROR << "Error message using client logger";

    // Verify console output
    std::string output = cout_buffer_.str();

    // Default logger messages should have [DEFAULT] prefix
    EXPECT_NE(output.find("[DEFAULT] Message using default logger"), std::string::npos);
    EXPECT_NE(output.find("[DEFAULT] Error message using default logger"), std::string::npos);

    // Client logger messages should have [CLIENT] prefix
    EXPECT_NE(output.find("[CLIENT] Message using client logger"), std::string::npos);
    EXPECT_NE(output.find("[CLIENT] Error message using client logger"), std::string::npos);

// Clean up the macros
#undef CLIENT_LOG_TRACE
#undef CLIENT_LOG_DEBUG
#undef CLIENT_LOG_INFO
#undef CLIENT_LOG_WARN
#undef CLIENT_LOG_ERROR
#undef CLIENT_LOG_FATAL
}

#ifdef VNE_PLATFORM_WEB
// Web platform specific test for console-only logging
TEST_F(LoggingSystemTest, WebPlatformConsoleOnlyLogging) {
    // Test that web platform forces console-only logging regardless of configuration
    std::string msg = "Web platform console test";

    // Try to configure with file sink - should be ignored on web
    log::LoggerConfig cfg;
    cfg.name = "web_test_logger";
    cfg.sink = log::LogSinkType::eFile;  // Try to use file sink
    cfg.file_path = "/some/file/path.log";
    cfg.console_pattern = "[WEB] %v";
    cfg.log_level = log::LogLevel::eInfo;
    cfg.async = false;

    log::Logging::configureLogger(cfg);

    // Log a message
    _VNE_LOG_INFO_L("web_test_logger") << msg;

    // Verify console output (should work even though we tried to use file sink)
    std::string output = cout_buffer_.str();
    EXPECT_NE(output.find(msg), std::string::npos);
    EXPECT_NE(output.find("[WEB]"), std::string::npos);

    // Verify that no file was created (web platform doesn't support file logging)
    EXPECT_FALSE(std::filesystem::exists(cfg.file_path));

    log::Logging::shutdown();
}

TEST_F(LoggingSystemTest, WebPlatformDefaultConfig) {
    // Test that default configuration on web platform is console-only
    log::LoggerConfig default_cfg = log::Logging::defaultLoggerConfig();

    EXPECT_EQ(default_cfg.sink, log::LogSinkType::eConsole);
    EXPECT_TRUE(default_cfg.file_path.empty());
    EXPECT_FALSE(default_cfg.async);

    // Test that the configuration works
    log::Logging::configureLogger(default_cfg);

    std::string msg = "Default web config test";
    VNE_LOG_INFO << msg;

    std::string output = cout_buffer_.str();
    EXPECT_NE(output.find(msg), std::string::npos);

    log::Logging::shutdown();
}
#endif
