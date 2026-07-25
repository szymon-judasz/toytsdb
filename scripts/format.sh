#!/bin/bash

# Get the project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

MODE="${1:-dry-run}"

echo "Formatting files in src and tools..."

if [[ "$MODE" == "fix" ]]; then
    find "$PROJECT_ROOT/src" "$PROJECT_ROOT/tools" -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" -o -name "*.cc" -o -name "*.cxx" -o -name "*.hxx" \) -exec clang-format -i {} +
else
    find "$PROJECT_ROOT/src" "$PROJECT_ROOT/tools" -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" -o -name "*.cc" -o -name "*.cxx" -o -name "*.hxx" \) -exec clang-format --dry-run -Werror -i {} +
fi

echo "Formatting complete."
