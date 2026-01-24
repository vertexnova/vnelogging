/* ---------------------------------------------------------------------
 * Copyright (c) 2024 Ajeet Singh Yadav. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Author:    Ajeet Singh Yadav
 * Created:   MAY-2024
 *
 * Autodoc:   yes
 * ----------------------------------------------------------------------
 */

#include "log_queue_worker.h"

namespace {

constexpr size_t kBatchSize = 32;  //!< Number of messages to process per batch

}  // namespace

namespace vne {  // Outer namespace
namespace log {  // Inner namespace

LogQueueWorker::LogQueueWorker(LogQueue& queue)
    : queue_(queue)
    , running_(false) {}

LogQueueWorker::~LogQueueWorker() {
    stop();
}

void LogQueueWorker::start() {
    running_ = true;
    worker_thread_ = std::thread(&LogQueueWorker::run, this);
}

void LogQueueWorker::stop() {
    running_ = false;
    queue_.push([]() {});  // Push a dummy task to wake up the worker thread
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

void LogQueueWorker::flush() {
    while (!queue_.empty()) {
        auto log_task = queue_.pop();
        log_task();
    }
}

void LogQueueWorker::run() {
    while (running_) {
        // Drain multiple tasks at once to reduce lock contention
        auto batch = queue_.drain(kBatchSize);

        // Execute all tasks in the batch
        for (auto& log_task : batch) {
            if (log_task) {
                log_task();
            }
        }
    }
}

}  // namespace log
}  // namespace vne
