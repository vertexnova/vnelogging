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

#include "log_queue.h"

namespace vne {  // Outer namespace
namespace log {  // Inner namespace
void LogQueue::push(std::function<void()> log_task) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(std::move(log_task));
    condition_.notify_one();
}

std::function<void()> LogQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (queue_.empty()) {
        condition_.wait(lock);
    }

    auto log_task = queue_.front();
    queue_.pop();
    return log_task;
}

bool LogQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

std::vector<std::function<void()>> LogQueue::drain(size_t max_items) {
    std::unique_lock<std::mutex> lock(mutex_);

    // Wait for at least one item
    while (queue_.empty()) {
        condition_.wait(lock);
    }

    // Drain up to max_items
    std::vector<std::function<void()>> batch;
    batch.reserve(std::min(max_items, queue_.size()));

    while (!queue_.empty() && batch.size() < max_items) {
        batch.push_back(std::move(queue_.front()));
        queue_.pop();
    }

    return batch;
}
}  // namespace log
}  // namespace vne
