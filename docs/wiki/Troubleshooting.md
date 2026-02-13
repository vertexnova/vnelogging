# Troubleshooting

## No output / wrong level

- Check `config.log_level` — messages below this level are filtered
- Ensure `Logging::configureLogger(config)` is called before any log macros
- Ensure `CREATE_VNE_LOGGER_CATEGORY` is in scope

## Colors show as escape codes

Some terminals (e.g. Xcode debugger) don't support ANSI colors:

```cpp
vne::log::Logging::setColorEnabled(false);
```

Or set the `NO_COLOR` environment variable.

## Log file not created

- Ensure the directory exists: `std::filesystem::create_directories("logs");`
- Check `config.file_path` is valid
- Ensure `config.sink` includes `LogSinkType::eFile` or `eBoth`

## Async logs missing on exit

Always call `Logging::shutdown()` before exit. It flushes pending messages and closes files.

## Link errors

- Ensure `target_link_libraries(your_target PRIVATE vne::logging)` is set
- Use `--recursive` when cloning: `git clone --recursive ...`
