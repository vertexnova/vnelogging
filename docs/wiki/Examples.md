# Examples

Build examples with:
```bash
cmake -B build -DBUILD_EXAMPLES=ON
cmake --build build
```

| Example | Description |
|---------|-------------|
| **01_HelloLogging** | Basic setup, all log levels, stream API |
| **02_subsystem_logging** | Multiple loggers (physics, render, audio) |
| **03_spdlog_integration** | Using vnelogging alongside spdlog |
| **04_benchmark** | Sync vs async performance comparison |
| **05_multithreaded** | Thread-safe logging from concurrent threads |

Run:
```bash
./build/bin/01_HelloLogging
./build/bin/02_SubsystemLogging
# etc.
```

---

**Tutorial:** For a step-by-step walkthrough, see [Adding Logging](https://learnvertexnova.com/docs/docs/learn/adding-logging/) on the VertexNova Engine docs.
