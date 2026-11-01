
#!/bin/bash

set -euo pipefail
shopt -s nullglob

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

DIR_INPUTS="$PROJECT_DIR/data/inputs"
DIR_RESULTS="$PROJECT_DIR/data/results"
DIR_BENCHMARKS="$DIR_RESULTS/benchmarks"
DIR_CHARTS="$DIR_RESULTS/charts"
DIR_BUILD="$PROJECT_DIR/build"

EXEC="$DIR_BUILD/matmult"

mkdir -p "$DIR_BENCHMARKS" "$DIR_CHARTS"

step() {
    echo
    echo "[$1/3] $2"
}

success() {
    echo "      ✓ $1"
}

# 1. Build
step 1 "Building project..."

cmake -S "$PROJECT_DIR" -B "$DIR_BUILD" \
    -DCMAKE_BUILD_TYPE=Release \
    >/dev/null

cmake --build "$DIR_BUILD" --parallel \
    >/dev/null

success "Build completed."

# 2. Benchmarks
step 2 "Running benchmarks..."

for size in grande media pequena; do

    inputs=("$DIR_INPUTS"/*"$size"*.txt)

    if [ "${#inputs[@]}" -ne 1 ]; then
        echo "Error: Expected exactly one input for '$size'." >&2
        exit 1
    fi

    input="${inputs[0]}"
    filename="$(basename "$input" .txt)"

    csv_file="$DIR_BENCHMARKS/result_${filename}.csv"
    temp_file="${csv_file}.tmp"

    echo "      Processing: $filename"

    {
        echo "matrix,configuration,threads,run,time_s"
        "$EXEC" "$input"
    } > "$temp_file"

    mv "$temp_file" "$csv_file"

    success "$filename completed."
done

# 3. Charts
step 3 "Generating charts..."

python3 "$PROJECT_DIR/scripts/generate_charts.py"

success "Charts generated."

# Summary
echo
echo "--------------------------------"
echo "Experiments completed successfully!"
echo
echo "Results:"
echo "  Benchmarks: data/results/benchmarks/"
echo "  Charts:     data/results/charts/"
