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

#include "log_queue.h"

#include <thread>
#include <atomic>

/**
 * @file log_queue_worker.h
 *
 * @brief Worker that processes log tasks from a queue.
 *
 * The LogQueueWorker class is responsible for managing a worker thread
 * that continuously processes log tasks from a LogQueue instance.
 */

namespace vne::log {

/**
 * @brief Log queue worker class.
 *
 * Manages a worker thread that processes log tasks from a queue.
 */
class LogQueueWorker {
   public:
    /**
     * @brief Constructs a LogQueueWorker instance.
     *
     * @param queue Reference to a LogQueue instance.
     */
    explicit LogQueueWorker(LogQueue& queue);

    /**
     * @brief Destructor.
     *
     * Ensures the worker thread is stopped before destruction.
     */
    ~LogQueueWorker();

    /**
     * @brief Starts the log queue worker thread.
     *
     * This method launches a worker thread that continuously
     * processes log tasks from the queue.
     */
    void start();

    /**
     * @brief Stops the log queue worker thread.
     *
     * This method stops the worker thread and waits for it to
     * finish processing any remaining tasks.
     */
    void stop();

    /**
     * @brief Flushes all pending log tasks.
     *
     * This method processes all remaining log tasks in the queue.
     * It should be called before stopping the worker if there are
     * tasks that need to be processed immediately.
     */
    void flush();

   private:
    /**
     * @brief Worker thread function.
     *
     * Continuously processes log tasks from the queue while the
     * worker is running.
     */
    void run();

    // Deleted copy constructor
    LogQueueWorker(const LogQueueWorker&) = delete;

    // Deleted copy assignment operator
    LogQueueWorker& operator=(const LogQueueWorker&) = delete;

   private:
    LogQueue& queue_;            //!< Reference to the log queue.
    std::atomic<bool> running_;  //!< Flag indicating whether the worker is running.
    std::thread worker_thread_;  //!< Worker thread.
};

}  // namespace vne::log
