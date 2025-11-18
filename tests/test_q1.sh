#!/bin/bash

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

echo "=== Q1 / app1 - Basic analysis ==="
"$ROOT_DIR/q1"
echo

echo "=== Q1 / app1 - Leak check demo ==="
"$ROOT_DIR/q1" --leak-check
echo

echo "=== Q1 / app1 - All features (without monitor) ==="
"$ROOT_DIR/q1" --all
echo

echo "NOTE: For monitoring mode, run manually, e.g.:"
echo "  $ROOT_DIR/q1 --monitor 5"
echo "and stop it with Ctrl+C."
