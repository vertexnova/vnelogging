# 05 - Multithreaded Logging

Demonstrates thread-safe logging from multiple concurrent threads.

## What This Shows

- **Thread Safety**: Multiple threads logging simultaneously without corruption
- **Thread ID Tracking**: Each log shows which thread generated it
- **Sync vs Async**: Compare both modes under concurrent load
- **No Message Interleaving**: Complete log messages, never mixed

## Running

```bash
./bin/05_Multithreaded
```

## Console Output

```
=== VNE Logging: Multithreaded Example ===
Demonstrates thread-safe logging from 4 concurrent threads

=== Sync Mode: 4 threads x 100 logs each ===
2026-01-24 02:39:05 [Thread-1] [TRACE] [SYNC] Thread 0 - Message 0 (trace)
2026-01-24 02:39:05 [Thread-2] [TRACE] [SYNC] Thread 2 - Message 0 (trace)
2026-01-24 02:39:05 [Thread-3] [TRACE] [SYNC] Thread 1 - Message 0 (trace)
2026-01-24 02:39:05 [Thread-4] [TRACE] [SYNC] Thread 3 - Message 0 (trace)
2026-01-24 02:39:05 [Thread-1] [DEBUG] [SYNC] Thread 0 - Message 1 (debug)
2026-01-24 02:39:05 [Thread-1] [INFO] [SYNC] Thread 0 - Message 2 (info)
...

Sync completed: 400 logs in 5 ms
Throughput: 80000 logs/sec
Log file: logs/threaded_sync.log

=== Async Mode: 4 threads x 100 logs each ===
2026-01-24 02:39:05 [Thread-5] [TRACE] [ASYNC] Thread 0 - Message 0 (trace)
2026-01-24 02:39:05 [Thread-6] [TRACE] [ASYNC] Thread 1 - Message 0 (trace)
2026-01-24 02:39:05 [Thread-7] [TRACE] [ASYNC] Thread 2 - Message 0 (trace)
2026-01-24 02:39:05 [Thread-8] [TRACE] [ASYNC] Thread 3 - Message 0 (trace)
...

Async completed: 400 logs in 202 ms
Throughput: 1980 logs/sec
Log file: logs/threaded_async.log

=== Test Complete ===
Check logs/ directory for output files.
Each log line shows [Thread-N] to verify thread ID tracking.
```

## Key Points

1. **Thread-Local Thread IDs**: Each thread gets a unique `Thread-N` identifier
2. **Mutex Protection**: Sync mode uses mutex to prevent interleaving
3. **Lock-Free Queue**: Async mode uses efficient queue for high throughput
4. **No Data Races**: All shared state is properly synchronized

## Generated Files

- `logs/threaded_sync.log` - Synchronous logging output
- `logs/threaded_async.log` - Asynchronous logging output
