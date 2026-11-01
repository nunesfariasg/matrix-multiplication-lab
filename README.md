# Matrix Multiplication Lab

A C project comparing sequential and parallel matrix multiplication using POSIX Threads (Pthreads).

This project investigates the impact of input size and thread count on execution time, speedup, and parallel efficiency.

## Objectives

- Implement sequential matrix multiplication in C.
- Implement parallel matrix multiplication using Pthreads.
- Compare the correctness of both implementations.
- Measure execution time under different thread configurations.
- Analyze the effects of parallelism on performance.

## Implementation

Given two square matrices, `A` and `B`, the program computes their product:

\[
C = A \times B
\]

Each element of the resulting matrix is calculated as:

\[
C_{ij} = \sum_{k=0}^{N-1} A_{ik} \times B_{kj}
\]

The parallel implementation distributes the computation among multiple threads, allowing different portions of the resulting matrix to be processed concurrently.

## Technologies

- **C (C11)** — Core implementations.
- **Pthreads** — Parallel execution.
- **CMake** — Build system.
- **Bash** — Experiment automation.
- **Python** — Benchmark analysis and chart generation.
- **Pandas** — Data processing.
- **Matplotlib** — Data visualization.

## Project Structure

```text
matrix-multiplication-lab/
├── data/
│   ├── inputs/
│   └── results/
│       ├── benchmarks/
│       └── charts/
├── include/
├── scripts/
│   └── generate_charts.py
├── src/
│   ├── main.c
│   ├── matrix.c
│   ├── sequential.c
│   └── parallel.c
├── CMakeLists.txt
├── run_experiment.sh
└── README.md
```

## Requirements

- GCC or another compatible C compiler.
- CMake 3.10 or later.
- POSIX Threads support.
- Python 3.
- Python packages: Pandas and Matplotlib.

## Build and Execution

Clone the repository:

```bash
git clone https://github.com/nunesfariasg/matrix-multiplication-lab.git

cd matrix-multiplication-lab
```

### Build

Configure and compile the project using CMake:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

cmake --build build --parallel
```

The executable will be generated at:

```text
build/matmult
```

### Run experiments

Install the Python dependencies:

```bash
pip install pandas matplotlib
```

Execute the experiment script:

```bash
chmod +x run_experiment.sh

./run_experiment.sh
```

The script automates the build process, benchmark execution, and chart generation.

## Experimental Configurations

The experiments compare the following execution configurations:

| Configuration | Execution model |
|---|---|
| Sequential | 1 execution thread |
| 2 Threads | Pthreads |
| 4 Threads | Pthreads |
| 8 Threads | Pthreads |
| Maximum CPUs | Number of online logical CPUs |

Three input sizes are considered:

- Small
- Medium
- Large

Each input and execution configuration must be evaluated at least three times to provide a basis for statistical comparison.

## Performance Metrics

The benchmark analysis considers the following metrics:

**1. Average Execution Time**

The arithmetic mean of the execution times recorded across repeated runs.

**2. Speedup**

Measures the performance improvement of the parallel implementation relative to the sequential baseline.

\[
S_p = \frac{T_{seq}}{T_p}
\]

**3. Parallel Efficiency**

Measures how effectively the available threads contribute to performance improvement.

\[
E_p = \frac{S_p}{p}
\]

Where:

- \(T_{seq}\) — Sequential execution time.
- \(T_p\) — Parallel execution time using \(p\) threads.
- \(S_p\) — Speedup.
- \(E_p\) — Parallel efficiency.
- \(p\) — Number of parallel threads.

## Results

The experiment generates CSV files containing individual benchmark measurements.

Each CSV file uses the following columns:

| Column | Description |
|---|---|
| `matrix` | Input matrix identifier |
| `configuration` | Execution configuration |
| `threads` | Number of threads |
| `run` | Execution number |
| `time_s` | Execution time in seconds |

The analysis script processes these measurements and generates charts for:

- Average execution time.
- Speedup.
- Parallel efficiency.

Generated files are stored in:

```text
data/results/
├── benchmarks/
│   └── result_*.csv
└── charts/
    └── chart_*.png
```

## Analysis

The results are used to investigate how matrix size and thread count influence parallel performance.

The analysis considers:

- Performance gains compared to sequential execution.
- The relationship between thread count and execution time.
- Parallel efficiency as the number of threads increases.
- Performance saturation and overhead.
- The influence of input size on parallelization benefits.

Parallel execution is not necessarily faster in every configuration. Thread management overhead, memory access patterns, cache behavior, and synchronization can affect the results.

## Methodology

To ensure a fair comparison:

- Both implementations use the same input data.
- The same computational operation is measured in both versions.
- Both implementations use the same compiler optimization policy.
- At least three measurements are collected per configuration.
- Individual execution times are preserved.
- The maximum number of online logical CPUs is obtained using `sysconf(_SC_NPROCESSORS_ONLN)`.

## Academic Context

This project was developed as part of a **Operating Systems** course assignment, focusing on sequential and parallel processing with Pthreads.

**Problem:** Matrix Multiplication  
**Language:** C  
**Parallelization:** POSIX Threads (Pthreads)

## Experiment Environment

| Component | Specification |
|---|---|
| Operating System | Fedora Linux 44 (Workstation Edition) |
| CPU | Intel Core i5-11260H (11th Gen) |
| Physical Cores | 6 |
| Logical CPUs | 12 |
| RAM | 8 GB |
| Compiler | GCC 16.2.1 |
| Optimization | -O3 |
| C Standard | C23 |

## Authors

Developed by:

- Gustavo Nunes
- Gabriel Dias
- Matheus Emanuel
- Matheus Buratto
- Luiz Otavio

**Course:** Operating Systems  
**Institution:** Centro Universitário FAG