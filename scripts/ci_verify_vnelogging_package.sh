#!/usr/bin/env bash
# Smoke-test an installed prefix: configure and build a clean consumer that uses
# find_package(VneLogging CONFIG). Catches broken exports, namespaces, and
# missing VneCommonTargets / Threads discovery.
set -euo pipefail

INSTALL_PREFIX="${1:?usage: $0 <CMAKE_INSTALL_PREFIX> <REPO_ROOT>}"
REPO_ROOT="${2:?usage: $0 <CMAKE_INSTALL_PREFIX> <REPO_ROOT>}"
BUILD_OUT="${TMPDIR:-/tmp}/vnelogging-pkg-consumer-$$"
rm -rf "$BUILD_OUT"

cmake -S "$REPO_ROOT/ci/package_consumer" -B "$BUILD_OUT" \
    -DCMAKE_PREFIX_PATH="$INSTALL_PREFIX" \
    -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_OUT" --config Release

EXE=""
for cand in \
    "$BUILD_OUT/consumed_pkg_test" \
    "$BUILD_OUT/Release/consumed_pkg_test.exe" \
    "$BUILD_OUT/Debug/consumed_pkg_test.exe"
do
    if [[ -f "$cand" ]]; then
        EXE="$cand"
        break
    fi
done
if [[ -z "${EXE}" ]]; then
    echo "::error::Consumer executable not found under $BUILD_OUT"
    find "$BUILD_OUT" -type f \( -name 'consumed_pkg_test' -o -name 'consumed_pkg_test.exe' \) -print || true
    exit 1
fi

# Shared-library installs: resolve vnelogging at load time
export LD_LIBRARY_PATH="${INSTALL_PREFIX}/lib:${LD_LIBRARY_PATH:-}"
export DYLD_LIBRARY_PATH="${INSTALL_PREFIX}/lib:${DYLD_LIBRARY_PATH:-}"
export PATH="${INSTALL_PREFIX}/bin:${INSTALL_PREFIX}/lib:${PATH:-}"

"$EXE"
rm -rf "$BUILD_OUT"
echo "ci_verify_vnelogging_package: OK"
