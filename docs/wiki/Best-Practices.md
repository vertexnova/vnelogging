# Best Practices

## Logger organization

- Use descriptive names: `"physics"`, `"network"`, `"ui"`
- Organize hierarchically: `"core.init"`, `"graphics.rendering"`
- Separate concerns with different loggers

## Log levels

| Level | Use for |
|-------|---------|
| TRACE | Function entry/exit, extreme detail |
| DEBUG | Development debugging |
| INFO | Normal operation status |
| WARN | Something unusual but not broken |
| ERROR | Something failed |
| FATAL | Cannot continue |

**Tip:** Set `log_level = eInfo` in production to hide DEBUG/TRACE.

## Message quality

- Include relevant context
- Avoid logging sensitive data
- Use stream API for composing messages

## Configuration

- Configure log levels per environment (debug vs release)
- Create logs directory before logging
- Monitor log file sizes and disk usage
