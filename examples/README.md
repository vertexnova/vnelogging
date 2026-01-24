# VNE Logging Examples

This directory contains examples demonstrating various features and use cases of the VNE Logging library.

## Building Examples

```bash
cd build
cmake .. -DBUILD_EXAMPLES=ON
cmake --build .
```

## Available Examples

### 01_basic - Basic Logging

A simple "hello world" example showing fundamental logging setup:
- Logger configuration with console and file sinks
- All log levels (TRACE, DEBUG, INFO, WARN, ERROR, FATAL)
- Stream-style logging with variables

**Run:** `./bin/01_BasicLogging`

### 02_multiple_loggers - Multiple Loggers

Demonstrates game engine-style logging with separate subsystem loggers:
- **App Logger**: Console only, INFO level
- **Physics Logger**: Console + File, DEBUG level
- **Render Logger**: File only, TRACE level, async

**Run:** `./bin/02_MultipleLoggers`

### 03_combined_logging - Combined Logging Systems

Shows how to integrate vne::logging with spdlog in the same application:
- Library using vne::logging
- Application using spdlog
- Both writing to the same output file

**Run:** `./bin/03_ExampleCombinedLogging`

### 04_performance_test - Performance Benchmarking

Benchmarks sync vs async logging performance:
- Latency measurements (microseconds per log)
- Throughput measurements (logs per second)
- Comparison between modes

**Run:** `./bin/04_PerformanceTest`

## Quick Reference

| Example | Focus | Key Concepts |
|---------|-------|--------------|
| 01_basic | Getting started | LoggerConfig, VNE_LOG_* macros |
| 02_multiple_loggers | Architecture | Multiple loggers, custom macros |
| 03_combined_logging | Integration | spdlog interop, shared output |
| 04_performance_test | Performance | Async vs sync, benchmarking |
