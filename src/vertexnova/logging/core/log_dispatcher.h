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

#include "log_sink.h"
#include "log_queue.h"
#include "log_queue_worker.h"

#include <memory>
#include <vector>

/**
 * @file log_dispatcher.h
 *
 * @brief Dispatches log messages to configured log_sinks.
 *
 * The LogDispatcher class manages the dispatching of log messages to
 * registered log sinks. It uses a queue and a worker thread to asynchronously
 * process log messages, ensuring efficient logging without blocking the main
 * application thread.
 *
 * @see ILogSink
 * @see LogQueue
 * @see LogQueueWorker
 */

namespace vne::log {

class LogDispatcher {
   public:
    /**
     * @brief Constructs a LogDispatcher instance.
     */
    LogDispatcher();

    /**
     * @brief Destructs the LogDispatcher instance.
     *
     * Stops the log queue worker and ensures all pending log messages
     * are flushed before destruction.
     */
    ~LogDispatcher();

    /**
     * @brief Dispatches a log message to all registered log_sinks.
     *
     * @param log_sinks The collection of log sinks to which the message should be dispatched.
     * @param name The category name for the log message.
     * @param level The log level of the message.
     * @param time_stamp_type The type of timestamp to generate.
     *                        This specifies whether the timestamp should be in local time or UTC.
     * @param message The message content to log.
     * @param file The file name where the log was generated.
     * @param function The function name where the log was generated.
     * @param line The line number where the log was generated.
     */
    void dispatch(const std::vector<std::unique_ptr<ILogSink>>& log_sinks,
                  const std::string& name,
                  LogLevel level,
                  TimeStampType time_stamp_type,
                  const std::string& message,
                  const std::string& file,
                  const std::string& function,
                  uint32_t line);

    /**
     * @brief Flushes all pending log messages in the log_sinks.
     *
     * @param log_sinks The collection of log sinks to flush.
     *
     * This method ensures that all log messages currently in the queue are
     * processed and written out by the log sinks.
     */
    void flush(const std::vector<std::unique_ptr<ILogSink>>& log_sinks);

   private:
    // Deleted copy constructor and assignment operator
    LogDispatcher(const LogDispatcher&) = delete;
    LogDispatcher& operator=(const LogDispatcher&) = delete;

   private:
    LogQueue log_queue_;               //!< Queue for storing log tasks.
    LogQueueWorker log_queue_worker_;  //!< Worker for processing log tasks.
};

}  // namespace vne::log
