import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import argparse
import os

def plot_execution_times(csv_file, output_dir='plots'):
    """
    Plot the benchmark results from the CSV file.
    
    Args:
        csv_file (str): Path to the CSV file with benchmark results
        output_dir (str): Directory to save the plots
    """
    # Create output directory if it doesn't exist
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # Read the CSV file
    df = pd.read_csv(csv_file)
    
    # Get column names (implementation names)
    implementations = df.columns[1:]
    
    # Set up colors and markers for different implementations
    colors = ['blue', 'green', 'red', 'purple']
    markers = ['o', 's', '^', 'D']
    
    # Create line plot with all implementations
    plt.figure(figsize=(12, 7))
    
    for i, impl in enumerate(implementations):
        plt.plot(df['Matrix Size'], df[impl], 
                 marker=markers[i], 
                 color=colors[i], 
                 label=impl,
                 linewidth=2,
                 markersize=8)
    
    plt.xlabel('Matrix Size', fontsize=14)
    plt.ylabel('Execution Time (seconds)', fontsize=14)
    plt.title('GEMM Implementation Performance Comparison', fontsize=16)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend(fontsize=12)
    plt.tight_layout()
    
    # Save the plot
    line_plot_path = os.path.join(output_dir, 'gemm_performance_comparison.png')
    plt.savefig(line_plot_path, dpi=300)
    print(f"Saved line plot to {line_plot_path}")
    
    # Create a log-scale plot for better visibility of differences
    plt.figure(figsize=(12, 7))
    
    for i, impl in enumerate(implementations):
        plt.semilogy(df['Matrix Size'], df[impl], 
                    marker=markers[i], 
                    color=colors[i], 
                    label=impl,
                    linewidth=2,
                    markersize=8)
    
    plt.xlabel('Matrix Size', fontsize=14)
    plt.ylabel('Execution Time (seconds) - Log Scale', fontsize=14)
    plt.title('GEMM Implementation Performance Comparison (Log Scale)', fontsize=16)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend(fontsize=12)
    plt.tight_layout()
    
    # Save the log-scale plot
    log_plot_path = os.path.join(output_dir, 'gemm_performance_comparison_log.png')
    plt.savefig(log_plot_path, dpi=300)
    print(f"Saved log-scale plot to {log_plot_path}")
    
    # Calculate and plot speedup relative to the original implementation
    plt.figure(figsize=(12, 7))
    
    baseline = df[implementations[0]]  # Original implementation as baseline
    
    for i, impl in enumerate(implementations[1:], 1):  # Skip the first one (baseline)
        speedup = baseline / df[impl]
        plt.plot(df['Matrix Size'], speedup, 
                marker=markers[i], 
                color=colors[i], 
                label=f"{impl} Speedup",
                linewidth=2,
                markersize=8)
    
    plt.xlabel('Matrix Size', fontsize=14)
    plt.ylabel('Speedup (×)', fontsize=14)
    plt.title(f'Speedup Relative to {implementations[0]}', fontsize=16)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.axhline(y=1, color='blue', linestyle='-', alpha=0.3)  # Reference line at y=1
    plt.legend(fontsize=12)
    plt.tight_layout()
    
    # Save the speedup plot
    speedup_plot_path = os.path.join(output_dir, 'gemm_speedup_comparison.png')
    plt.savefig(speedup_plot_path, dpi=300)
    print(f"Saved speedup plot to {speedup_plot_path}")
    
    # Create a bar chart for a few selected matrix sizes
    selected_sizes = [100, 200, 400]  # Select a few sizes to display as bars
    selected_df = df[df['Matrix Size'].isin(selected_sizes)]
    
    fig, ax = plt.subplots(figsize=(14, 8))
    
    # Set the positions of the bars on the x-axis
    bar_width = 0.2
    index = np.arange(len(selected_sizes))
    
    for i, impl in enumerate(implementations):
        values = [selected_df[selected_df['Matrix Size'] == size][impl].values[0] 
                 for size in selected_sizes]
        ax.bar(index + i*bar_width, values, bar_width, 
               label=impl, color=colors[i], alpha=0.7)
    
    ax.set_xlabel('Matrix Size', fontsize=14)
    ax.set_ylabel('Execution Time (seconds)', fontsize=14)
    ax.set_title('GEMM Performance for Selected Matrix Sizes', fontsize=16)
    ax.set_xticks(index + bar_width * (len(implementations) - 1) / 2)
    ax.set_xticklabels(selected_sizes)
    ax.legend(fontsize=12)
    ax.grid(True, axis='y', linestyle='--', alpha=0.7)
    
    # Add values on top of bars
    for i, impl in enumerate(implementations):
        values = [selected_df[selected_df['Matrix Size'] == size][impl].values[0] 
                 for size in selected_sizes]
        for j, v in enumerate(values):
            ax.text(j + i*bar_width, v + 0.01, f"{v:.3f}", 
                   ha='center', va='bottom', fontsize=9, rotation=45)
    
    plt.tight_layout()
    
    # Save the bar plot
    bar_plot_path = os.path.join(output_dir, 'gemm_selected_sizes_comparison.png')
    plt.savefig(bar_plot_path, dpi=300)
    print(f"Saved bar plot to {bar_plot_path}")
    
    # Generate performance metrics table
    print("\nPerformance Metrics Summary:")
    print("-" * 80)
    
    # Display maximum speedups
    print("Maximum Speedups:")
    for i, impl in enumerate(implementations[1:], 1):
        speedup = baseline / df[impl]
        max_speedup = speedup.max()
        max_idx = speedup.idxmax()
        max_size = df.loc[max_idx, 'Matrix Size']
        print(f"  {impl}: {max_speedup:.2f}x at matrix size {max_size}")
    
    print("\nExecution times for largest matrix size (seconds):")
    largest_size = df['Matrix Size'].max()
    largest_df = df[df['Matrix Size'] == largest_size]
    for impl in implementations:
        time = largest_df[impl].values[0]
        print(f"  {impl}: {time:.4f}")

def main():
    parser = argparse.ArgumentParser(description='Plot GEMM benchmark results')
    parser.add_argument('--csv_file', '-f', default='mnk_optimized_times.csv', 
                       help='Path to the CSV file with benchmark results (default: mnk_optimized_times.csv)')
    parser.add_argument('--output', '-o', default='plots', help='Directory to save the plots')
    
    args = parser.parse_args()
    
    # Check if the CSV file exists
    if not os.path.exists(args.csv_file):
        print(f"Error: CSV file '{args.csv_file}' not found.")
        print(f"Please run your benchmark first or provide the correct path to your CSV file.")
        print(f"Usage example: python {os.path.basename(__file__)} --csv_file /path/to/your/mnk_optimized_times.csv")
        return
    
    try:
        plot_execution_times(args.csv_file, args.output)
        print(f"\nPlots have been saved to the '{args.output}' directory.")
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
    
if __name__ == "__main__":
    main()