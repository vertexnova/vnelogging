#!/usr/bin/env bash
# ----------------------------------------------------------------------
# Compute ARTIFACT_DETAIL for release tarball names (GitHub Actions).
# Safer than raw `xcodebuild -version` when MD_APPLE_SDK_ROOT / SDKROOT
# point at an Xcode .app and trigger SIGABRT (shell exit 134) on some runners.
#
# Usage: ci_artifact_detail.sh <platform>
#   platform: linux-gcc | macos | windows | web-emscripten | ios-static | android | generic
#
# Environment (platform-specific):
#   android — REQUIRED: ANDROID_PLATFORM=android-<API> (e.g. android-24), same form as CMake
#             -DANDROID_PLATFORM. The API level is embedded in the artifact slug; without this
#             set, the script exits with an error (do not rely on passing only "android").
#             Optional: ANDROID_NDK_ROOT → include Pkg.Revision in the slug when source.properties
#             exists (e.g. ...-ndk27.2.12479018-...); if unset, the slug omits the ndk segment.
#
# Writes ARTIFACT_DETAIL=<slug> to stdout and appends to GITHUB_ENV when set.
# ----------------------------------------------------------------------
set -euo pipefail

_xcode_version_raw() {
  local v=""
  v="$(env -u MD_APPLE_SDK_ROOT -u SDKROOT -u IOS_SDK_VERSION \
    xcodebuild -version 2>/dev/null | awk '/^Xcode / { print $2; exit }' || true)"
  if [[ -z "$v" && -n "${DEVELOPER_DIR:-}" ]]; then
    local inf=""
    inf="${DEVELOPER_DIR%/}/../Info.plist"
    if [[ -f "$inf" ]]; then
      v="$(defaults read "$inf" CFBundleShortVersionString 2>/dev/null || true)"
    fi
  fi
  printf '%s' "$v"
}

base="${1:-generic}"
arch="$(uname -m 2>/dev/null || true)"
arch="${arch//$'\r'/}"
arch="${arch//[[:space:]]/}"
if [[ -z "$arch" || "$(printf '%s' "$arch" | tr '[:upper:]' '[:lower:]')" == "unknown" ]]; then
  arch=""
fi
arch_suffix=""
if [[ -n "$arch" ]]; then
  arch_suffix="-${arch}"
fi

detail=""
case "$base" in
  linux-gcc)
    # shellcheck disable=SC1091
    . /etc/os-release 2>/dev/null || true
    vid="${VERSION_ID:-}"
    gcv="$(gcc -dumpfullversion 2>/dev/null || gcc -dumpversion 2>/dev/null || true)"
    if [[ -n "$gcv" && -n "$vid" ]]; then
      detail="ubuntu${vid}-gcc${gcv}${arch_suffix}"
    elif [[ -n "$gcv" ]]; then
      detail="gcc${gcv}${arch_suffix}"
    else
      detail="linux-gcc${arch_suffix}"
    fi
    ;;
  macos)
    macv="$(sw_vers -productVersion 2>/dev/null || true)"
    macv="${macv//$'\r'/}"
    xv_raw="$(_xcode_version_raw)"
    if [[ "$xv_raw" == "unknown" ]]; then xv_raw=""; fi
    if [[ -n "$xv_raw" ]]; then
      if [[ -n "$macv" ]]; then
        detail="macos${macv}-xcode${xv_raw}${arch_suffix}"
      else
        detail="xcode${xv_raw}${arch_suffix}"
      fi
    elif [[ -n "$macv" ]]; then
      detail="macos${macv}${arch_suffix}"
    else
      detail="macos${arch_suffix}"
    fi
    ;;
  windows)
    detail="vs2022-x64"
    ;;
  web-emscripten)
    emv="$(emcc -dumpversion 2>/dev/null || true)"
    if [[ -z "$emv" ]]; then
      echo "::error::emcc -dumpversion failed"
      exit 1
    fi
    detail="emcc${emv}${arch_suffix}"
    ;;
  ios-static)
    xv_raw="$(_xcode_version_raw)"
    if [[ -z "$xv_raw" || "$xv_raw" == "unknown" ]]; then
      echo "::warning::Could not read Xcode version for ios-static artifact name; using ios-arm64 fallback."
      detail="ios-arm64"
    else
      detail="xcode${xv_raw}-arm64"
    fi
    ;;
  android)
    android_plat="${ANDROID_PLATFORM:-}"
    if [[ -z "$android_plat" ]]; then
      echo "::error::ANDROID_PLATFORM is required for android artifact name generation." >&2
      exit 1
    fi
    if [[ "$android_plat" != android-* ]]; then
      echo "::error::ANDROID_PLATFORM must start with 'android-'; got '${android_plat}'." >&2
      exit 1
    fi
    api="${android_plat#android-}"
    if [[ -z "$api" ]]; then
      echo "::error::ANDROID_PLATFORM must include an API level after 'android-'; got '${android_plat}'." >&2
      exit 1
    fi
    if [[ ! "$api" =~ ^[0-9]+$ ]]; then
      echo "::error::ANDROID_PLATFORM API level must be numeric (e.g. android-24); got '${android_plat}'." >&2
      exit 1
    fi
    abi_slug="arm64v8a"
    ndk_root="${ANDROID_NDK_ROOT:-}"
    ndk_rev=""
    if [[ -n "$ndk_root" && -f "$ndk_root/source.properties" ]]; then
      ndk_rev="$(grep -E '^[[:space:]]*Pkg.Revision[[:space:]]*=' "$ndk_root/source.properties" | head -1 | cut -d= -f2- | tr -d ' \t\r\n')"
    fi
    if [[ -n "$ndk_rev" ]]; then
      detail="android${api}-ndk${ndk_rev}-${abi_slug}"
    else
      detail="android${api}-${abi_slug}"
    fi
    ;;
  *)
    detail="generic${arch_suffix}"
    ;;
esac

echo "ARTIFACT_DETAIL=${detail}"
if [[ -n "${GITHUB_ENV:-}" ]]; then
  echo "ARTIFACT_DETAIL=${detail}" >>"$GITHUB_ENV"
fi
