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

#include <fstream>

namespace vne::log {

/**
 * @class FileLogSink
 * @brief Concrete implementation of ILogSink for file logging.
 *
 * This class provides a file-based logging implementation. It
 * outputs log messages to a specified file and allows for log
 * pattern customization.
 */
class FileLogSink : public ILogSink {
   public:
    /**
     * @brief Constructs a FileLogSink with the specified file name.
     *
     * Initializes the file sink with a file name, log pattern, and
     * an optional append mode.
     *
     * @param filename The name of the file to log to.
     * @param append A flag for opening mode append. Defaults to true.
     */
    FileLogSink(const std::string& filename, bool append = true);

    /**
     * @brief Destructor.
     *
     * Cleans up resources used by the file sink.
     */
    ~FileLogSink();

    /**
     * @brief Logs a message to the file.
     *
     * Overrides the log method from ILogSink to output messages to
     * the specified file.
     *
     * @param name The category name for the log message.
     * @param level The log level of the message.
     * @param time_stamp_type The type of timestamp to generate.
     *                        This specifies whether the timestamp should be in local time or UTC.
     * @param message The message content to log.
     * @param file The file name where the log was generated.
     * @param function The function name where the log was generated.
     * @param line The line number where the log was generated.
     */
    void log(const std::string& name,
             LogLevel level,
             TimeStampType time_stamp_type,
             const std::string& message,
             const std::string& file,
             const std::string& function,
             uint32_t line) override;

    /**
     * @brief Flushes the file output.
     *
     * Ensures that all log messages have been written to the file.
     */
    void flush() override;

    /**
     * @brief Gets the current log pattern.
     *
     * @return The current log pattern.
     */
    std::string getPattern() const override;

    /**
     * @brief Sets a new log pattern.
     *
     * @param pattern The new log pattern.
     */
    void setPattern(const std::string& pattern) override;

    /**
     * @brief Retrieves the log file name.
     *
     * This function returns the current log file name associated with this file log sink.
     *
     * @return The name of the log file.
     */
    std::string getFileName() const;

    /**
     * @brief Checks the status of the file opening mode.
     *
     * This function returns the current status of the file opening mode for this file log sink.
     * It indicates whether the sink is set to append to an existing file or overwrite it.
     *
     * @return true if the file is opened in append mode, false if it's in overwrite mode.
     */
    bool isAppend() const;

    /**
     * @brief Creates a new instance of the file log sink.
     *
     * This factory method creates a new instance of the file log sink.
     *
     * @return A unique pointer to the cloned file log sink instance.
     */
    std::unique_ptr<ILogSink> clone() const override;

   private:
    // Deleted copy constructor and assignment operator
    FileLogSink(const FileLogSink&) = delete;
    FileLogSink& operator=(const FileLogSink&) = delete;

   private:
    std::string pattern_;        //!< The log pattern used by the file sink.
    std::ofstream file_stream_;  //!< Output file stream for logging.
    std::string file_name_;      //!< The name of the file to log to.
    bool is_append_;             //!< The flag of opening mode
};

}  // namespace vne::log
