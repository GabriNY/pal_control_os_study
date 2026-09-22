#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
fail=0
while IFS= read -r -d '' f; do
  dir=$(dirname "$f")
  base=$(basename "$f" .c)
  out="$dir/$base"
  echo "[CC] ${f#$ROOT/}"
  if ! gcc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra "$f" -o "$out"; then
    echo "FAILED: $f" >&2
    fail=1
  fi
done < <(find "$ROOT" -name '*.c' -print0 | sort -z)
exit "$fail"
