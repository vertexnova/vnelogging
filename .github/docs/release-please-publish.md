# VneLogging — release-please and publish artifacts

## Flow

1. **release-please** (on push to `main`) updates `CHANGELOG.md`, `VERSION`, and `.release-please-manifest.json`, and opens or updates a release PR.
2. After that PR is merged, a **Git tag** is created.
3. In the same workflow run triggered by a push to `main`, the **publish** job runs when `release_created == 'true'`; it builds `cmake --install` trees for Linux, macOS, Windows, Emscripten, iOS (static), and Android, then uploads `vnelogging-v<version>-<platform>-<detail>.tar.gz` to the GitHub Release.

### Workflows in this repository

- **[`.github/workflows/ci.yml`](../workflows/ci.yml)** runs on pushes and PRs to `main`. It includes **Build / iOS**, which configures with **`CI=true`**, installs twice (explicit prefix and default prefix), and verifies **`install/include/vertexnova/logging`**, **`install/include/vertexnova/common`**, and at least one **`*.a`**.
- **[`.github/workflows/release-please.yml`](../workflows/release-please.yml)** is the workflow that publishes the platform tarballs. Reuse the same install verification there before packing so release artifacts always ship **vnecommon** headers. For **`ios-static`** tarball naming, prefer **`scripts/ci_artifact_detail.sh ios-static`** instead of raw **`xcodebuild -version`** in the workflow: **`MD_APPLE_SDK_ROOT`** / **`SDKROOT`** can make **`xcodebuild`** abort with **SIGABRT** (shell exit **134**) on some runners.

## Repo settings

- **Allow auto-merge** (optional): Settings → General → Pull Requests.
- **`REPO_ACCESS_TOKEN`** (optional): PAT with `repo` scope for release-please / auto-merge API limits; otherwise `github.token` is used.

## Submodule / source tree

Public installs **require** the **vnecommon** submodule under **`deps/internal/vnecommon`**. CMake fails configuration if **`deps/internal/vnecommon/include/vertexnova/common`** is missing. Checkout with **`submodules: recursive`** in CI and release jobs.

## Artifact naming helper

**[`scripts/ci_artifact_detail.sh`](../../scripts/ci_artifact_detail.sh)** prints **`ARTIFACT_DETAIL=<slug>`** and appends it to **`GITHUB_ENV`** when set. It clears **`MD_APPLE_SDK_ROOT`**, **`SDKROOT`**, and **`IOS_SDK_VERSION`** before probing Xcode and falls back to **`Info.plist`** when needed. Example:

```bash
./scripts/ci_artifact_detail.sh ios-static
```

Pass **`linux-gcc`**, **`macos`**, **`windows`**, **`web-emscripten`**, **`ios-static`**, **`android`**, or **`generic`** to match your publish matrix **`platform`** label.

## Install layout

Tarballs contain the CMake install prefix: `include/vertexnova/logging/`, `include/vertexnova/common/` (headers from the bundled header-only **vnecommon** dependency — no separate `lib/` entry for vnecommon), the **vnelogging** static or shared library under `lib/` (e.g. `libvnelogging.a` for iOS static Release), `LICENSE`, `CHANGELOG.md`, and `lib/cmake/VneLogging/` export files.

For **iOS (Xcode)**, use `cmake --build <build> --config Release` then `cmake --install <build> --config Release`. On **GitHub Actions** (`CI` + `GITHUB_WORKSPACE` set), configure defaults `CMAKE_INSTALL_PREFIX` to **`$GITHUB_WORKSPACE/install`**, so a bare `cmake --install` matches checks for **`install/include/vertexnova/logging`** and **`install/include/vertexnova/common`**. Locally, the default is **`<binary-dir>/install`** unless you override the prefix. You can still pass **`cmake --install`** with an explicit **`--prefix`** absolute path.

## Troubleshooting

- **iOS / Xcode**: CI uses `-DCMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED=NO`. For device builds locally, configure signing in Xcode.
- **`xcodebuild` exit 134 / SIGABRT**: Often triggered when environment variables point at an Xcode **`.app`** bundle inconsistently. Use **`scripts/ci_artifact_detail.sh`** or **`env -u MD_APPLE_SDK_ROOT -u SDKROOT xcodebuild -version`** for version probes.
- **Release builds** pass `-DVNE_LOGGING_DEV=OFF` so the default dev preset does not force tests/examples on.
