# 02 - Multiple Loggers Example

This example demonstrates how to set up multiple loggers with different configurations, simulating a game engine architecture with separate logging for different subsystems.

## What This Example Shows

1. **Multiple Logger Instances**: Creating separate loggers for App, Physics, and Render subsystems
2. **Different Sink Types**: Console-only, file-only, and both
3. **Different Log Levels**: Each subsystem with appropriate verbosity
4. **Async vs Sync Logging**: Using async logging for the render subsystem
5. **Custom Logging Macros**: Defining subsystem-specific macros like `APP_LOG_INFO`, `PHYSICS_LOG_DEBUG`

## Logger Configurations

| Logger   | Sinks          | Level | Async | Use Case                        |
|----------|----------------|-------|-------|----------------------------------|
| App      | Console        | INFO  | No    | User-facing messages             |
| Physics  | Console + File | DEBUG | No    | Debug physics issues             |
| Render   | File only      | TRACE | Yes   | Detailed render profiling        |

## Building

```bash
cd build
cmake .. -DBUILD_EXAMPLES=ON
cmake --build .
```

## Running

```bash
./bin/02_MultipleLoggers
```

## Expected Output

```
[APP] 2026-01-24 02:17:25 [INFO] Application starting...
[APP] 2026-01-24 02:17:25 [INFO] Loading configuration...
[APP] 2026-01-24 02:17:25 [INFO] Initializing subsystems...
[APP] 2026-01-24 02:17:25 [INFO] Starting main loop...
[PHYSICS] 2026-01-24 02:17:25 [DEBUG] Physics update started, dt=0.016
[PHYSICS] 2026-01-24 02:17:25 [DEBUG] Calculated velocity: 0.15696 m/s
[PHYSICS] 2026-01-24 02:17:25 [INFO] Collision detected between objects
[PHYSICS] 2026-01-24 02:17:25 [DEBUG] Physics update completed
[PHYSICS] 2026-01-24 02:17:25 [DEBUG] Physics update started, dt=0.016
[PHYSICS] 2026-01-24 02:17:25 [DEBUG] Calculated velocity: 0.15696 m/s
[PHYSICS] 2026-01-24 02:17:25 [INFO] Collision detected between objects
[PHYSICS] 2026-01-24 02:17:25 [DEBUG] Physics update completed
...
[PHYSICS] 2026-01-24 02:17:25 [WARN] Physics solver did not converge after 100 iterations
[APP] 2026-01-24 02:17:25 [ERROR] Critical subsystem failure detected
[APP] 2026-01-24 02:17:25 [INFO] Shutting down subsystems...
[APP] 2026-01-24 02:17:25 [INFO] Saving state...
[APP] 2026-01-24 02:17:25 [INFO] Application shutdown complete

=== Example Complete ===
Check the following files:
  - physics.log (Physics subsystem logs)
  - render.log  (Render subsystem logs - most verbose)

Note: App logs only appear on console (INFO level and above)
```

## Generated Files

- `physics.log` - Contains Physics subsystem logs (DEBUG level and above)
- `render.log` - Contains Render subsystem logs (TRACE level - most verbose)

## Key Concepts

- Different loggers can have completely independent configurations
- Use async logging for performance-critical paths (like rendering)
- Use file-only logging for high-volume debug data
- Custom macros make subsystem logging clean and consistent
