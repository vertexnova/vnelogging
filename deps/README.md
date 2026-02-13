# Deps

Dependencies used by VneLogging (layout aligned with other VertexNova libs such as vneio).

| Directory   | Purpose |
|------------|---------|
| `internal/` | VertexNova internal libs (e.g. vnecommon). Used when present. |
| `external/` | Third-party deps (e.g. googletest, spdlog) as submodules or vendored. |

The project also supports the legacy layout: top-level `libs/` (internal) and `external/` (third-party). Prefer placing submodules under `deps/internal` and `deps/external` for consistency.

Clone with submodules: `git submodule update --init --recursive` (or `git clone --recursive`).
