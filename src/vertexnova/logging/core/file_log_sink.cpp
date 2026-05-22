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

#include "file_log_sink.h"
#include "log_formatter.h"

#include <iostream>
#include <exception>
#include <filesystem>

namespace vne {  // Outer namespace
namespace log {  // Inner namespace

FileLogSink::FileLogSink(const std::string& filename, bool append)
    : pattern_("%x [%l] [%!] %v")
    , file_name_(filename)
    , is_append_(append)

{
    try {
        if (filename.empty()) {
            throw std::runtime_error("No log file specified.");
        }
        // Create directory if that doesn't exist
        std::filesystem::path filepath(filename);
        std::filesystem::path directory = filepath.parent_path();
        // Only create directory if parent path is not empty (i.e., not a simple filename)
        if (!directory.empty() && !std::filesystem::exists(directory)) {
            std::filesystem::create_directories(directory);
        }

        file_stream_.open(filename.c_str(), append ? std::ofstream::out | std::ofstream::app : std::ofstream::out);
        if (!file_stream_.is_open()) {
            throw std::runtime_error("Couldn't open file " + filename + " for write.");
        }
    } catch (std::exception& ex) {
        std::cerr << "[ERROR] : " << ex.what() << std::endl;
    }
}

FileLogSink::~FileLogSink() {
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
}

void FileLogSink::log(const std::string& name,
                      LogLevel level,
                      TimeStampType time_stamp_type,
                      const std::string& message,
                      const std::string& file,
                      const std::string& function,
                      uint32_t line) {
    std::string formatted_log =
        LogFormatter::format(name, level, time_stamp_type, message, file, function, line, pattern_);
    file_stream_ << formatted_log << '\n';
}

void FileLogSink::flush() {
    if (file_stream_.is_open()) {
        file_stream_.flush();
    }
}

std::string FileLogSink::getPattern() const {
    return pattern_;
}

void FileLogSink::setPattern(const std::string& pattern) {
    pattern_ = pattern;
}

std::string FileLogSink::getFileName() const {
    return file_name_;
}

bool FileLogSink::isAppend() const {
    return is_append_;
}

std::unique_ptr<ILogSink> FileLogSink::clone() const {
    return std::make_unique<FileLogSink>(file_name_, is_append_);
}

}  // namespace log
}  // namespace vne
