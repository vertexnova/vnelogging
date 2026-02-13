# Vertexnova Logging Documentation

Documentation for the VertexNova Logging library.

## Wiki Content

User guide, quick start, and reference pages are in [wiki/](wiki/). These can be copied to the [GitHub Wiki](https://github.com/vertexnova/vnelogging/wiki) — see [wiki/README.md](wiki/README.md) for instructions.

## Generating API Documentation

Generate API documentation with Doxygen:

```bash
cmake -DENABLE_DOXYGEN=ON -B build
cmake --build build --target docs
```

Or use the script:

```bash
./scripts/generate-docs.sh --api-only
```

Documentation will be available at `build/docs/html/index.html`.

## Requirements

- Doxygen 1.8.13 or higher
- Graphviz (for class diagrams)
