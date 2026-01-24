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
void LogQueue::push(const std::function<void()>& log_task) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(log_task);
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
}  // namespace log
}  // namespace vne
