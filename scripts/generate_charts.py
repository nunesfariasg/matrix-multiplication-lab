
from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt


# Project directories
PROJECT_DIR = Path(__file__).resolve().parent.parent

DIR_BENCHMARKS = PROJECT_DIR / "data" / "results" / "benchmarks"
DIR_CHARTS = PROJECT_DIR / "data" / "results" / "charts"

DIR_CHARTS.mkdir(parents=True, exist_ok=True)

# Expected CSV columns
REQUIRED_COLUMNS = {
    "matrix",
    "configuration",
    "threads",
    "run",
    "time_s"
}

# Locate benchmark CSV files
csv_files = sorted(DIR_BENCHMARKS.glob("result_*.csv"))

if not csv_files:
    print(f"No CSV files found in {DIR_BENCHMARKS}")
    raise SystemExit(1)


# Process each benchmark file
for csv_file in csv_files:

    matrix_name = csv_file.stem.replace("result_", "", 1)

    print(f"Analyzing: {matrix_name}")

    df = pd.read_csv(csv_file)

    # Validate CSV format
    if not REQUIRED_COLUMNS.issubset(df.columns):
        print(f"Invalid CSV format: {csv_file.name}")
        raise SystemExit(1)

    # Convert columns to numeric
    for column in ["threads", "run", "time_s"]:
        df[column] = pd.to_numeric(df[column], errors="coerce")

    df = df.dropna(
        subset=["configuration", "threads", "run", "time_s"]
    )

    if df.empty or (df["time_s"] <= 0).any():
        print(f"Invalid benchmark measurements: {csv_file.name}")
        raise SystemExit(1)

    # Calculate mean and standard deviation
    averages = (
        df.groupby(
            ["configuration", "threads"],
            sort=False
        )["time_s"]
        .agg(["mean", "std"])
        .reset_index()
    )

    # Get sequential execution time
    sequential = averages.loc[
        averages["configuration"] == "sequential",
        "mean"
    ]

    if sequential.empty:
        print(f"Sequential baseline unavailable for {matrix_name}")
        raise SystemExit(1)

    sequential_time = sequential.iloc[0]

    # Calculate speedup
    averages["speedup"] = sequential_time / averages["mean"]

    # Calculate parallel efficiency
    effective_threads = averages["threads"].replace(0, 1)

    averages["efficiency"] = (
            averages["speedup"] / effective_threads
    )

    # Configuration labels
    averages["label"] = averages["configuration"].map({
        "sequential": "Sequential",
        "2_threads": "2 Threads",
        "4_threads": "4 Threads",
        "8_threads": "8 Threads",
        "max_cpus": "Max CPUs"
    }).fillna(averages["configuration"])

    configurations = averages["label"]

    # Create charts
    fig, (ax1, ax2, ax3) = plt.subplots(
        1, 3, figsize=(20, 6)
    )

    # Chart 1: Average execution time
    ax1.bar(
        configurations,
        averages["mean"],
        yerr=averages["std"].fillna(0),
        capsize=4,
        edgecolor="black"
    )

    ax1.set_title(f"Average Execution Time - {matrix_name}")
    ax1.set_ylabel("Execution Time (seconds)")
    ax1.set_xlabel("Configuration")
    ax1.tick_params(axis="x", rotation=30)
    ax1.grid(axis="y", linestyle="--", alpha=0.7)

    # Chart 2: Speedup
    ax2.plot(
        configurations,
        averages["speedup"],
        marker="o",
        markersize=8,
        linewidth=2
    )

    ax2.axhline(
        y=1,
        linestyle="--",
        label="Baseline (Speedup = 1)"
    )

    ax2.set_title(f"Speedup - {matrix_name}")
    ax2.set_ylabel("Speedup")
    ax2.set_xlabel("Configuration")
    ax2.tick_params(axis="x", rotation=30)
    ax2.grid(True, linestyle="--", alpha=0.7)
    ax2.legend()

    # Chart 3: Parallel efficiency
    ax3.plot(
        configurations,
        averages["efficiency"],
        marker="o",
        markersize=8,
        linewidth=2
    )

    ax3.axhline(
        y=1,
        linestyle="--",
        label="Ideal Efficiency (100%)"
    )

    ax3.set_title(f"Parallel Efficiency - {matrix_name}")
    ax3.set_ylabel("Efficiency")
    ax3.set_xlabel("Configuration")
    ax3.tick_params(axis="x", rotation=30)
    ax3.grid(True, linestyle="--", alpha=0.7)
    ax3.legend()

    # Save charts
    plt.tight_layout()

    output_file = DIR_CHARTS / f"chart_{matrix_name}.png"

    plt.savefig(output_file, dpi=300)
    plt.close(fig)

    print(f"Chart generated: {output_file.relative_to(PROJECT_DIR)}")

print("Chart generation completed successfully.")
