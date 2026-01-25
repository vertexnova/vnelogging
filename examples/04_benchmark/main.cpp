/* ---------------------------------------------------------------------
 * Copyright (c) 2025 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   January 2026
 *
 * Example: Performance benchmarking for sync vs async logging
 * Compares throughput and latency of different logging modes
 * ----------------------------------------------------------------------
 */

#include <vertexnova/logging/logging.h>
#include <vertexnova/common/macros.h>

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

namespace {

constexpr const char* kSyncLoggerName = "sync_perf";
constexpr const char* kAsyncLoggerName = "async_perf";
constexpr size_t kWarmupIterations = 1000;
constexpr size_t kBenchmarkIterations = 10000;

}  // namespace

// Custom macros for benchmark loggers
#define SYNC_LOG_INFO VNE_LOG_INFO_L(kSyncLoggerName)
#define ASYNC_LOG_INFO VNE_LOG_INFO_L(kAsyncLoggerName)

namespace {

CREATE_VNE_LOGGER_CATEGORY("performance.test")

}  // namespace

struct BenchmarkResult {
    double total_time_ms;
    double avg_time_us;
    double min_time_us;
    double max_time_us;
    double throughput_per_sec;
};

template<typename LogFunc>
BenchmarkResult runBenchmark(const std::string& name, size_t iterations, LogFunc log_func) {
    VNE_UNUSED(name);

    std::vector<double> times;
    times.reserve(iterations);

    // Warmup
    for (size_t i = 0; i < kWarmupIterations; ++i) {
        log_func(static_cast<int>(i));
    }

    // Benchmark
    auto total_start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        log_func(static_cast<int>(i));
        auto end = std::chrono::high_resolution_clock::now();

        double time_us = std::chrono::duration<double, std::micro>(end - start).count();
        times.push_back(time_us);
    }

    auto total_end = std::chrono::high_resolution_clock::now();

    BenchmarkResult result;
    result.total_time_ms = std::chrono::duration<double, std::milli>(total_end - total_start).count();
    result.avg_time_us = std::accumulate(times.begin(), times.end(), 0.0) / static_cast<double>(times.size());
    result.min_time_us = *std::min_element(times.begin(), times.end());
    result.max_time_us = *std::max_element(times.begin(), times.end());
    result.throughput_per_sec = (static_cast<double>(iterations) / result.total_time_ms) * 1000.0;

    return result;
}

void printResult(const std::string& name, const BenchmarkResult& result, int iterations) {
    std::cout << "\n" << name << " Results:" << std::endl;
    std::cout << "  Iterations:    " << iterations << std::endl;
    std::cout << "  Total Time:    " << std::fixed << std::setprecision(2) << result.total_time_ms << " ms"
              << std::endl;
    std::cout << "  Avg Latency:   " << std::fixed << std::setprecision(3) << result.avg_time_us << " us" << std::endl;
    std::cout << "  Min Latency:   " << std::fixed << std::setprecision(3) << result.min_time_us << " us" << std::endl;
    std::cout << "  Max Latency:   " << std::fixed << std::setprecision(3) << result.max_time_us << " us" << std::endl;
    std::cout << "  Throughput:    " << std::fixed << std::setprecision(0) << result.throughput_per_sec << " logs/sec"
              << std::endl;
}

int main() {
    std::cout << "=== VNE Logging Performance Benchmark ===" << std::endl;
    std::cout << "Benchmark iterations: " << kBenchmarkIterations << std::endl;
    std::cout << "Warmup iterations: " << kWarmupIterations << std::endl;

    // Create logs directory in the current working directory (typically the build directory)
    const std::string logs_dir = "logs";
    std::filesystem::create_directories(logs_dir);

    // Setup sync logger (file only to avoid console I/O overhead)
    vne::log::LoggerConfig sync_config;
    sync_config.name = kSyncLoggerName;
    sync_config.sink = vne::log::LogSinkType::eFile;
    sync_config.file_pattern = "[SYNC] %x [%l] %v";
    sync_config.file_path = logs_dir + "/perf_sync.log";
    sync_config.log_level = vne::log::LogLevel::eInfo;
    sync_config.async = false;
    vne::log::Logging::configureLogger(sync_config);

    // Setup async logger (file only)
    vne::log::LoggerConfig async_config;
    async_config.name = kAsyncLoggerName;
    async_config.sink = vne::log::LogSinkType::eFile;
    async_config.file_pattern = "[ASYNC] %x [%l] %v";
    async_config.file_path = logs_dir + "/perf_async.log";
    async_config.log_level = vne::log::LogLevel::eInfo;
    async_config.async = true;
    vne::log::Logging::configureLogger(async_config);

    std::cout << "\nRunning sync logging benchmark..." << std::endl;

    // Benchmark sync logging
    auto sync_result = runBenchmark("Sync Logging", kBenchmarkIterations, [](int i) {
        SYNC_LOG_INFO << "Benchmark message #" << i << " with some additional data for realistic size";
    });

    printResult("Sync Logging", sync_result, kBenchmarkIterations);

    std::cout << "\nRunning async logging benchmark..." << std::endl;

    // Benchmark async logging
    auto async_result = runBenchmark("Async Logging", kBenchmarkIterations, [](int i) {
        ASYNC_LOG_INFO << "Benchmark message #" << i << " with some additional data for realistic size";
    });

    printResult("Async Logging", async_result, kBenchmarkIterations);

    // Compare results
    std::cout << "\n=== Comparison ===" << std::endl;
    double latency_improvement = ((sync_result.avg_time_us - async_result.avg_time_us) / sync_result.avg_time_us) * 100;
    double throughput_improvement =
        ((async_result.throughput_per_sec - sync_result.throughput_per_sec) / sync_result.throughput_per_sec) * 100;

    std::cout << "Async vs Sync:" << std::endl;
    std::cout << "  Latency improvement:    " << std::fixed << std::setprecision(1)
              << (latency_improvement > 0 ? "+" : "") << latency_improvement << "%" << std::endl;
    std::cout << "  Throughput improvement: " << std::fixed << std::setprecision(1)
              << (throughput_improvement > 0 ? "+" : "") << throughput_improvement << "%" << std::endl;

    // Shutdown (flushes async logger)
    std::cout << "\nFlushing and shutting down..." << std::endl;
    vne::log::Logging::shutdown();

    std::cout << "\n=== Benchmark Complete ===" << std::endl;
    std::cout << "Log files created: " << logs_dir << "/perf_sync.log, " << logs_dir << "/perf_async.log" << std::endl;

    return 0;
}
