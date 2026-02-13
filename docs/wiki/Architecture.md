# Architecture

The logging system uses a layered design:

| Layer | Purpose |
|-------|---------|
| **Logging Interface** | High-level API (`Logging`, macros) |
| **Logger Management** | Lifecycle, configuration (`LogManager`) |
| **Sink Layer** | Output destinations (console, file) |
| **Formatting** | Timestamps, patterns (`LogFormatter`) |
| **Queue** | Async message queue |

## Key components

- **Logging** — Static setup and configuration
- **LogManager** — Singleton, manages logger instances
- **ILogger** — Abstract logger interface
- **SyncLogger** / **AsyncLogger** — Implementations
- **ConsoleLogSink** / **FileLogSink** — Output sinks

For detailed design, see [logging.md](../vertexnova/logging/logging.md) in the repository.
