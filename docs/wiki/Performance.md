# Performance

## Benchmark (Apple M3 Pro, Release build)

| Mode | Throughput | Latency |
|------|------------|---------|
| Synchronous | ~500K logs/sec | ~2 μs/log |
| Asynchronous | ~1.2M logs/sec | ~0.8 μs/log |

## When to use which

| Use case | Mode |
|----------|------|
| Debugging, small apps | Synchronous |
| Production, high throughput | Asynchronous |
| Critical errors | Sync (or set `flush_level` to eError) |

## Tips

- Use `config.async = true` for production workloads
- Set `flush_level = LogLevel::eError` so errors flush immediately
- Use `log_level` to filter in production (e.g. eInfo) and avoid DEBUG/TRACE overhead
