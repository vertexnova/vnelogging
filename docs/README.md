# Vertexnova Logging Documentation

Documentation for the VertexNova Logging library.

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
