# VNE Logging Examples

This directory contains examples demonstrating various features and use cases of the VNE Logging library.

## Building Examples

```bash
cd build
cmake .. -DBUILD_EXAMPLES=ON
cmake --build .
```

## Available Examples

### 01_hello_logging - Getting Started

A simple "hello world" example showing fundamental logging setup:
- Logger configuration with console and file sinks
- All log levels (TRACE, DEBUG, INFO, WARN, ERROR, FATAL)
- Stream-style logging with variables

**Run:** `./bin/01_HelloLogging`

### 02_subsystem_logging - Game Engine Style

Demonstrates game engine-style logging with separate subsystem loggers:
- **App Logger**: Console only, INFO level
- **Physics Logger**: Console + File, DEBUG level
- **Render Logger**: File only, TRACE level, async

**Run:** `./bin/02_SubsystemLogging`

### 03_spdlog_integration - Dual Logging Systems

Shows how to integrate vne::logging with spdlog in the same application:
- Library using vne::logging
- Application using spdlog
- Both writing to the same output file

**Run:** `./bin/03_SpdlogIntegration`

### 04_benchmark - Performance Testing

Benchmarks sync vs async logging performance:
- Latency measurements (microseconds per log)
- Throughput measurements (logs per second)
- Comparison between modes

**Run:** `./bin/04_Benchmark`

### 05_multithreaded - Concurrent Logging

Demonstrates thread-safe logging from multiple threads:
- 4 threads logging simultaneously
- Thread ID tracking in output
- Sync vs async under concurrent load
- No message corruption or interleaving

**Run:** `./bin/05_Multithreaded`

## Quick Reference

| Example | Focus | Key Concepts |
|---------|-------|--------------|
| 01_hello_logging | Getting started | LoggerConfig, VNE_LOG_* macros |
| 02_subsystem_logging | Architecture | Multiple loggers, custom macros |
| 03_spdlog_integration | Integration | spdlog interop, shared output |
| 04_benchmark | Performance | Async vs sync, benchmarking |
| 05_multithreaded | Thread safety | Concurrent threads, thread IDs |
