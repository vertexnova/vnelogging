/**
 * @file main.cpp
 * @brief Demonstrates thread-safe logging from multiple concurrent threads
 *
 * This example shows:
 * - Multiple threads logging simultaneously
 * - Thread ID tracking in log output
 * - Both sync and async logging modes
 * - No log message corruption or interleaving
 */

#include <vertexnova/logging/logging.h>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>
#include <vector>

using namespace vne::log;

namespace {

CREATE_VNE_LOGGER_CATEGORY("multithreaded")

constexpr int kNumThreads = 4;
constexpr int kLogsPerThread = 100;

std::atomic<int> g_completed_threads{0};

/**
 * @brief Worker function that logs messages from a thread
 */
void workerThread(int thread_id, bool use_async) {
    const std::string mode = use_async ? "ASYNC" : "SYNC";

    for (int i = 0; i < kLogsPerThread; ++i) {
        // Vary log levels to show thread safety across all levels
        switch (i % 5) {
            case 0:
                VNE_LOG_TRACE << "[" << mode << "] Thread " << thread_id << " - Message " << i << " (trace)";
                break;
            case 1:
                VNE_LOG_DEBUG << "[" << mode << "] Thread " << thread_id << " - Message " << i << " (debug)";
                break;
            case 2:
                VNE_LOG_INFO << "[" << mode << "] Thread " << thread_id << " - Message " << i << " (info)";
                break;
            case 3:
                VNE_LOG_WARN << "[" << mode << "] Thread " << thread_id << " - Message " << i << " (warn)";
                break;
            case 4:
                VNE_LOG_ERROR << "[" << mode << "] Thread " << thread_id << " - Message " << i << " (error)";
                break;
        }

        // Small delay to increase thread interleaving
        if (i % 10 == 0) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }

    g_completed_threads++;
}

/**
 * @brief Run a test with multiple threads
 */
void runThreadedTest(bool use_async) {
    const std::string mode_name = use_async ? "Async" : "Sync";
    const std::string file_name = use_async ? "logs/threaded_async.log" : "logs/threaded_sync.log";

    std::cout << "\n=== " << mode_name << " Mode: " << kNumThreads << " threads x " << kLogsPerThread
              << " logs each ===\n";

    // Configure logger
    LoggerConfig config;
    config.name = kDefaultLoggerName;
    config.log_level = LogLevel::eTrace;
    config.sink = LogSinkType::eBoth;
    config.file_path = file_name;
    config.async = use_async;
    config.console_pattern = "%x [%t] [%l] %v";  // Include thread ID
    config.file_pattern = "%x [%t] [%l] %v";

    Logging::configureLogger(config);

    g_completed_threads = 0;

    auto start = std::chrono::high_resolution_clock::now();

    // Launch threads
    std::vector<std::thread> threads;
    threads.reserve(kNumThreads);

    for (int i = 0; i < kNumThreads; ++i) {
        threads.emplace_back(workerThread, i, use_async);
    }

    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }

    // For async, give time for logs to flush
    if (use_async) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    int total_logs = kNumThreads * kLogsPerThread;
    std::cout << "\n" << mode_name << " completed: " << total_logs << " logs in " << duration.count() << " ms\n";
    std::cout << "Throughput: " << (total_logs * 1000.0 / duration.count()) << " logs/sec\n";
    std::cout << "Log file: " << file_name << "\n";

    Logging::shutdown();
}

}  // namespace

int main() {
    std::cout << "=== VNE Logging: Multithreaded Example ===\n";
    std::cout << "Demonstrates thread-safe logging from " << kNumThreads << " concurrent threads\n";

    // Create logs directory
    std::filesystem::create_directories("logs");

    // Test synchronous mode
    runThreadedTest(false);

    // Test asynchronous mode
    runThreadedTest(true);

    std::cout << "\n=== Test Complete ===\n";
    std::cout << "Check logs/ directory for output files.\n";
    std::cout << "Each log line shows [Thread-N] to verify thread ID tracking.\n";

    return 0;
}
