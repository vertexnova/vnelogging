---
applyTo: "**/*_test.cpp,**/tests/**/*.cpp,**/test/**/*.cpp"
---
## Test Review Rules

- New features/bug fixes should include tests unless clearly justified.
- Tests must be deterministic (no timeouts/flaky sleeps).
- Prefer clear Arrange-Act-Assert structure.
- If mocking: keep mocks minimal and verify behavior, not implementation details.
- Ensure tests run fast and don't require special hardware unless explicitly marked.
