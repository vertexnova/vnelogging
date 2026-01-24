# Examples

This directory contains example code demonstrating how to use `vne::logging` in various scenarios.

## Available Examples

| Example | Description |
|---------|-------------|
| [01_combined_logging](01_combined_logging/) | Library using vne::logging + app using spdlog, shared output |

## Building Examples

```bash
# Configure with examples enabled
cmake -B build -DBUILD_EXAMPLES=ON

# Build
cmake --build build
```

## Running Examples

```bash
# Run the combined logging example
./build/examples/01_combined_logging/myapp/01_ExampleCombinedLogging
```

## Adding New Examples

To add a new example:

1. Create a new directory under `examples/` with numbered prefix (e.g., `02_basic_usage`)
2. Add your source files and `CMakeLists.txt`
3. Add `add_subdirectory(02_basic_usage)` to `examples/CMakeLists.txt`
4. Add documentation to this README
