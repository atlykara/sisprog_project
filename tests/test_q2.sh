#!/bin/bash

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

echo "=== Scan mode ==="
"$ROOT_DIR/q2" -s "$ROOT_DIR/tests/samples/vulnerable1.c" "$ROOT_DIR/tests/samples/vulnerable2.c"

echo
echo "=== Recommendation mode ==="
"$ROOT_DIR/q2" -r "$ROOT_DIR/tests/samples/vulnerable1.c"

echo
echo "=== Extended analysis ==="
"$ROOT_DIR/q2" -x "$ROOT_DIR/tests/samples/vulnerable2.c"
