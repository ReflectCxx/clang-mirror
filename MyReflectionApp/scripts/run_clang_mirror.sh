#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

MIRROR_EXE="$PROJECT_ROOT/../bin/clang-mirror"
SOURCE_LIST="$PROJECT_ROOT/registration_srcs.txt"
OUT_DIR="$PROJECT_ROOT"

if [[ ! -f "$MIRROR_EXE" ]]; then
    echo "ERROR: clang-mirror not found."
    echo "$MIRROR_EXE"
    exit 1
fi

if [[ ! -f "$SOURCE_LIST" ]]; then
    echo "ERROR: reflection_srcs.txt not found."
    echo "$SOURCE_LIST"
    exit 1
fi

# Collect source files into an array
FILE_ARGS=()

while IFS= read -r LINE || [[ -n "$LINE" ]]; do
    [[ -z "$LINE" ]] && continue
    [[ "$LINE" =~ ^# ]] && continue

    FILE_ARGS+=("$PROJECT_ROOT/$LINE")
done < "$SOURCE_LIST"

echo "Running:"
echo "$MIRROR_EXE ${FILE_ARGS[*]} -out-dir=$OUT_DIR -- -I"$PROJECT_ROOT/inc" -std=c++20"

"$MIRROR_EXE" \
    "${FILE_ARGS[@]}" \
    -out-dir="$OUT_DIR" \
    -- -I"$PROJECT_ROOT/inc" -std=c++20

echo "Code generation process finished."
echo