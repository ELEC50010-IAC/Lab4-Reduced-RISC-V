#!/bin/bash

# Usage: ./doit.sh [tests/file_tb.cpp ...]
# Relative test paths are resolved from repo/tb, as in the group runner.
set -u
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd) || exit 1
RTL_FOLDER=$(cd "$SCRIPT_DIR/../rtl" && pwd) || exit 1
CXX_FLAGS="-std=c++17"
LINK_FLAGS="-lgtest -lgtest_main -lpthread"
if [[ -n "${GTEST_ROOT:-}" ]]; then
    GTEST_DIR=$(cd "$GTEST_ROOT" && pwd) || exit 1
    CXX_FLAGS="$CXX_FLAGS -I\"$GTEST_DIR/include\""
    LINK_FLAGS="-L\"$GTEST_DIR/lib\" $LINK_FLAGS"
fi

cd "$SCRIPT_DIR" || exit 1
shopt -s nullglob
if [[ $# -eq 0 ]]; then
    files=("$SCRIPT_DIR"/tests/*.cpp)
else
    files=("$@")
fi
if [[ ${#files[@]} -eq 0 ]]; then
    echo "No test files found" >&2
    exit 1
fi

passes=0
fails=0
for file in "${files[@]}"; do
    if [[ ! -f "$file" ]]; then
        echo "Missing test file: $file" >&2
        fails=$((fails + 1))
        continue
    fi
    base=$(basename "$file")
    case "$base" in
        verify.cpp) name=top ;;
        *_tb.cpp) name=${base%_tb.cpp} ;;
        *) echo "Unrecognised test file: $file" >&2
           fails=$((fails + 1)); continue ;;
    esac
    source_dir=$(cd "$(dirname "$file")" && pwd) || exit 1
    source_file="$source_dir/$base"
    echo "Testing $base ($name)"

    # Each target must build its own executable; a failed build cannot reuse one.
    rm -rf "$SCRIPT_DIR/obj_dir" || exit 1
    if ! verilator -Wall --trace --top-module "$name" \
            -cc "$RTL_FOLDER/$name.sv" -y "$RTL_FOLDER" \
            --exe "$source_file" --prefix Vdut -o Vdut \
            -CFLAGS "$CXX_FLAGS" -LDFLAGS "$LINK_FLAGS"; then
        echo "Verilator failed: $base" >&2
        fails=$((fails + 1))
        continue
    fi
    if ! make -j 2 -C obj_dir -f Vdut.mk; then
        echo "C++ build failed: $base" >&2
        fails=$((fails + 1))
        continue
    fi
    # Preserve this working directory for compile.sh and ../rtl/program.hex.
    if ./obj_dir/Vdut; then
        passes=$((passes + 1))
    else
        echo "Simulation failed: $base" >&2
        fails=$((fails + 1))
    fi
done

echo "Result: $passes test file(s) passed, $fails failed"
[[ $fails -eq 0 ]]
