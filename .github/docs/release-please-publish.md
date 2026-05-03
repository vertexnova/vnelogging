# VneLogging — release-please and publish artifacts

## Flow

1. **release-please** (on push to `main`) updates `CHANGELOG.md`, `VERSION`, and `.release-please-manifest.json`, and opens or updates a release PR.
2. After that PR is merged, a **Git tag** is created.
3. In the same workflow run triggered by a push to `main`, the **publish** job runs when `release_created == 'true'`; it builds `cmake --install` trees for Linux, macOS, Windows, Emscripten, iOS (static), and Android, then uploads `vnelogging-v<version>-<platform>-<detail>.tar.gz` to the GitHub Release.

## Repo settings

- **Allow auto-merge** (optional): Settings → General → Pull Requests.
- **`REPO_ACCESS_TOKEN`** (optional): PAT with `repo` scope for release-please / auto-merge API limits; otherwise `github.token` is used.

## Install layout

Tarballs contain the CMake install prefix: `include/vertexnova/logging/`, `include/vertexnova/common/` (headers from the bundled header-only **vnecommon** dependency — no separate `lib/` entry for vnecommon), the **vnelogging** static or shared library under `lib/` (e.g. `libvnelogging.a` for iOS static Release), `LICENSE`, `CHANGELOG.md`, and `lib/cmake/VneLogging/` export files.

For **iOS (Xcode)**, use `cmake --build <build> --config Release` then `cmake --install <build> --config Release`. Pass an **absolute** `-DCMAKE_INSTALL_PREFIX=...` or `cmake --install ... --prefix <abs path>` so staging paths match your packaging checks; the project defaults iOS installs to `<binary-dir>/install` when the prefix was left at CMake’s default.

## Troubleshooting

- **iOS / Xcode**: CI uses `-DCMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED=NO`. For device builds locally, configure signing in Xcode.
- **Release builds** pass `-DVNE_LOGGING_DEV=OFF` so the default dev preset does not force tests/examples on.
