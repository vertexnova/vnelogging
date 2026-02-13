# Copilot PR Review Instructions (Repo-wide)

When reviewing pull requests in this repository, optimize for: correctness, safety, performance, maintainability, and testability.

## Review output format

- Start with a brief summary (1–3 bullets): what changed + biggest risks.
- Then list issues grouped by severity:
  - **BLOCKER** (must fix before merge)
  - **MAJOR** (strongly recommended)
  - **MINOR** (nice-to-have)
- For each issue: point to the exact file/area, explain *why*, and propose a concrete fix.
- If you're unsure, ask a clarifying question rather than guessing.

## Core C++ focus areas

### Correctness & API design

- Check invariants, pre/post-conditions, edge cases, and undefined behavior.
- Prefer clear ownership semantics (RAII, no raw owning pointers).
- Avoid surprising API changes; keep interfaces stable.
- Flag implicit conversions, narrowing, lifetime issues, dangling references, and iterator invalidation.

### Memory, lifetime, and RAII

- Look for leaks, double frees, use-after-free, and lifetime coupling bugs.
- Prefer `std::unique_ptr` for ownership and references/`std::span` for non-owning views.
- Ensure exception safety: strong/basic guarantees where appropriate.

### Performance (real-world)

- Flag accidental O(N²) behavior, unnecessary allocations/copies, and hidden sync points.
- Recommend `const&`, move semantics, reserve, and `std::span` when it helps.
- Prefer algorithmic improvements over micro-optimizations.
- Avoid premature "clever" tricks—only optimize with a clear benefit.

### Concurrency

- Identify data races, thread-unsafe singletons/statics, and unsafe shared mutation.
- Check lock ordering, deadlocks, and overly broad critical sections.
- Prefer immutable data or message-passing where possible.

### Error handling & logging

- Ensure failures are handled consistently (no silent ignores).
- Avoid throwing exceptions across module boundaries unless the project already standardizes it.
- Logs must be actionable; include context and avoid noisy logs in hot paths.

### Security / safety checks

- Flag unsafe parsing, unchecked inputs, path traversal, command injection patterns, and deserialization risks.
- Watch for integer overflow, buffer overrun, format-string issues, and risky `memcpy`/casts.

## Build system & portability

- Ensure changes compile on all supported compilers/OS (don't rely on non-standard extensions).
- Prefer standard C++ features and consistent warning levels.
- Avoid introducing platform-specific behavior without guards and tests.

## Documentation and readability

- Names should reveal intent; avoid abbreviations that hide meaning.
- Keep functions small; suggest refactors when complexity grows.
- Ensure public APIs have brief doc comments if behavior isn't obvious.

## CI expectations

- If the PR changes behavior, expect: tests updated/added.
- If the PR changes public API or core behavior, request a short PR description update with rationale and migration notes.

---

## This repository (VneLogging)

### Coding standards

- Enforce **CODING_GUIDELINES.md** (repository root) and **.copilot/coding-guidelines.md** (naming summary).
- Naming: PascalCase types; camelCase functions/methods; snake_case + trailing `_` for private members; `e` for enum values; `k` for constants; `I` prefix for interface classes.

### Formatting and static analysis

- **.clang-format** and **.clang-tidy** are enforced in separate CI actions; focus review on design and logic. If you spot style/naming that automation might miss, flag it.

### Project layout

- Library: **src/vertexnova/logging/** — Public API: **include/vertexnova/logging/** — Tests: **tests/** — Examples: **examples/**
- Dependencies: **deps/internal/** (e.g. vnecommon), **deps/external/** (e.g. googletest, spdlog). No top-level `external/` or `libs/`.

### Build and test

- Build, tests, clang-format, and clang-tidy run in separate CI actions. Focus review on correctness, API design, and test quality rather than re-checking what CI already covers.

### Extra review focus here

- Missing or incorrect thread-safety documentation for public APIs.
- Unnecessary edits under **deps/external/** or **deps/internal/** unless the PR intentionally updates or configures them.

Keep comments concrete: cite the guideline or rule and suggest a specific fix when possible.
