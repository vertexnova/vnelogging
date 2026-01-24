# 04 - Performance Test Example

This example benchmarks the performance of synchronous vs asynchronous logging to help you choose the right mode for your application.

## What This Example Shows

1. **Sync vs Async Comparison**: Direct comparison of logging modes
2. **Latency Measurement**: Per-call timing in microseconds
3. **Throughput Measurement**: Logs per second
4. **Warmup Phase**: Proper benchmarking with warmup iterations

## Metrics Measured

- **Total Time**: Wall-clock time for all iterations
- **Average Latency**: Mean time per log call
- **Min/Max Latency**: Best and worst case latency
- **Throughput**: Number of log messages per second

## Building

```bash
cd build
cmake .. -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

**Note**: Always benchmark in Release mode for accurate results.

## Running

```bash
./bin/04_PerformanceTest
```

## Benchmark Environment

The results below were captured on the following system:

| Component | Details |
|-----------|---------|
| OS | macOS (Darwin 24.5.0) |
| Architecture | arm64 |
| CPU | Apple M3 Pro |
| Compiler | Apple Clang 17.0.0 |
| Build Type | Release |

## Expected Output

```
=== VNE Logging Performance Benchmark ===
Benchmark iterations: 10000
Warmup iterations: 1000

Running sync logging benchmark...

Sync Logging Results:
  Iterations:    10000
  Total Time:    19.10 ms
  Avg Latency:   1.889 us
  Min Latency:   1.583 us
  Max Latency:   28.375 us
  Throughput:    523588 logs/sec

Running async logging benchmark...

Async Logging Results:
  Iterations:    10000
  Total Time:    10.73 ms
  Avg Latency:   1.050 us
  Min Latency:   0.500 us
  Max Latency:   59.500 us
  Throughput:    932176 logs/sec

=== Comparison ===
Async vs Sync:
  Latency improvement:    +44.4%
  Throughput improvement: +78.0%

Flushing and shutting down...

=== Benchmark Complete ===
Log files created: perf_sync.log, perf_async.log
```

## When to Use Each Mode

| Mode  | Use Case                                    |
|-------|---------------------------------------------|
| Sync  | Critical paths where log order matters      |
| Async | High-throughput scenarios, render loops     |

## Key Concepts

- Async logging offloads I/O to a background thread
- Sync logging blocks until the log is written
- File-only logging is faster than console logging
- Warmup iterations ensure JIT and cache are ready
