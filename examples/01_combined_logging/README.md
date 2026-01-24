# Combined Logging Example

This example demonstrates a common real-world scenario where:

- **mylib**: A library uses `vne::logging` internally
- **myapp**: An application uses `spdlog` but links to `mylib`

Both logging systems are configured to write to the **same file** (`combined.log`) and console, so you can see logs from both systems in one place.

## Building

```bash
# Configure with examples enabled
cmake -B build -DBUILD_EXAMPLES=ON

# Build
cmake --build build

# Run the example
./build/examples/01_combined_logging/myapp/01_ExampleCombinedLogging
```

## What You'll See

When you run the example, you'll see output like this in both the console and `combined.log`:

```
[SPDLOG-APP] 2025-01-23 10:30:45.123 [info] Application starting up
[VNE-LIB] 2025-01-23 10:30:45.124 [INFO] [mylib] MyLibrary initialized successfully
[SPDLOG-APP] 2025-01-23 10:30:45.125 [info] Library initialized, starting operations
[VNE-LIB] 2025-01-23 10:30:45.126 [DEBUG] [mylib] Starting operation: operation1
[VNE-LIB] 2025-01-23 10:30:45.226 [INFO] [mylib] Operation 'operation1' completed successfully
[SPDLOG-APP] 2025-01-23 10:30:45.227 [warn] This is a warning from the application
```

## Coding Guidelines Applied

This example follows the VertexNova coding guidelines:

| Guideline | Example |
|-----------|---------|
| File names | `my_library.h`, `my_library.cpp` (snake_case) |
| Classes | `MyLibrary` (PascalCase) |
| Methods | `initialize()`, `doWork()` (camelCase) |
| Private members | `is_initialized_` (snake_case + `_`) |
| Constants | `kLogFilePath`, `kConsolePattern` (`k` + PascalCase) |
| Namespaces | `example` (lowercase) |
| Include order | Corresponding → Project → System → Third-party |

## Pattern Used

The library follows the VertexNova logging pattern:

1. **Logger Category**: Created at namespace scope using `CREATE_VNE_LOGGER_CATEGORY(kLoggerCategory)`
2. **Initialization**: Configure the logger with `ConfigureLogger()`
3. **Usage**: Use macros like `VNE_LOG_INFO`, `VNE_LOG_DEBUG`, `VNE_LOG_WARN` with stream operator `<<`
4. **Shutdown**: Call `Logging_C::Shutdown()` in cleanup

## Use Case

This pattern is useful when:
- You're developing a library that needs logging
- You want to use `vne::logging` in your library
- Applications using your library may use different logging systems (spdlog, glog, etc.)
- You want all logs to appear in the same place for debugging

## File Structure

```
01_combined_logging/
├── mylib/               # Library using vne::logging
│   ├── my_library.h
│   ├── my_library.cpp
│   └── CMakeLists.txt
├── myapp/               # Application using spdlog
│   ├── main.cpp
│   └── CMakeLists.txt
├── CMakeLists.txt       # Example CMake
└── README.md            # This file
```
