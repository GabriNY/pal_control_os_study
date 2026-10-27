#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD="${TMPDIR:-/tmp}/pal_control_os_build"
rm -rf "$BUILD"
mkdir -p "$BUILD"

count=0
while IFS= read -r -d '' src; do
    rel="${src#$ROOT/}"
    exe="$BUILD/$(echo "$rel" | tr '/ ' '__' | sed 's/\.c$//')"
    echo "[gcc] $rel"
    gcc -std=c11 -Wall -Wextra -pedantic "$src" -o "$exe"
    count=$((count+1))
done < <(find "$ROOT/10_shared_memory_semaphores" "$ROOT/11_controles_shm_semaphore" -name '*.c' -print0)

echo "OK: $count archivos C compilados. Ejecutables temporales en $BUILD"
