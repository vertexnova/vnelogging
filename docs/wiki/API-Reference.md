# API Reference

## Main types

| Type | Purpose |
|------|---------|
| `Logging` | Static setup: `configureLogger`, `initialize`, `shutdown`, `getLogger` |
| `LoggerConfig` | Configuration struct |
| `ILogger` | Programmatic API: `log()`, `flush()`, `addLogSink()` |
| `LogLevel` | eTrace, eDebug, eInfo, eWarn, eError, eFatal |
| `LogSinkType` | eNone, eConsole, eFile, eBoth |

## Macros

| Macro | Description |
|-------|-------------|
| `CREATE_VNE_LOGGER_CATEGORY("name")` | Define category (once per file) |
| `VNE_LOG_TRACE` … `VNE_LOG_FATAL` | Log with default logger |
| `VNE_LOG_*_L("logger")` | Log with named logger |

## Key methods

**Logging (static):**
- `configureLogger(config)` — Configure in one call
- `initialize(name, async)` — Alternative setup
- `shutdown()` — Flush and close
- `getLogger(name)` — Get logger instance
- `setLogLevel(name, level)` — Change level at runtime
- `addConsoleSink(name)` / `addFileSink(name, path)` — Add sinks
- `setConsolePattern` / `setFilePattern` — Format patterns

**ILogger:**
- `log()`, `flush()`, `addLogSink()`, `setCurrentLogLevel()`

---

For full Doxygen API docs, build with:
```bash
cmake -DENABLE_DOXYGEN=ON -B build
cmake --build build --target docs
```
