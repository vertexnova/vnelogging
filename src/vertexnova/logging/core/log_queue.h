#pragma once
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

#include <queue>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <vector>

/**
 * @file log_queue.h
 *
 * @brief Thread-safe queue for storing log tasks.
 */

namespace vne::log {

class LogQueue {
   public:
    /**
     * @brief Pushes a new log task onto the queue.
     * @param log_task The log task to push (moved for efficiency).
     */
    void push(std::function<void()> log_task);

    /**
     * @brief Pops a log task from the queue.
     * @return The next log task.
     */
    std::function<void()> pop();

    /**
     * @brief Checks if the queue is empty.
     * @return True if the queue is empty, false otherwise.
     */
    bool empty() const;

    /**
     * @brief Drains up to max_items tasks from the queue into a vector.
     * @param max_items Maximum number of items to drain (default: 32).
     * @return Vector of log tasks.
     *
     * This is more efficient than multiple pop() calls as it only locks once.
     */
    std::vector<std::function<void()>> drain(size_t max_items = 32);

   private:
    std::queue<std::function<void()>> queue_;  //!< The queue of log tasks.
    mutable std::mutex mutex_;                 //!< Mutex for synchronizing access to the queue.
    std::condition_variable condition_;        //!< Condition variable for managing thread waiting.
};

}  // namespace vne::log
