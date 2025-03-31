import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# Set style
sns.set_style('whitegrid')
plt.rcParams.update({'font.size': 12})

# Load the data
df = pd.read_csv('gemm_times.csv')

# Create figure with multiple plots
fig, axes = plt.subplots(2, 1, figsize=(12, 14), gridspec_kw={'height_ratios': [1, 0.8]})

# Plot 1: Line plot for all implementations
ax1 = axes[0]
for column in df.columns[1:]:  # Skip the 'Matrix Size' column
    ax1.plot(df['Matrix Size'], df[column], marker='o', linewidth=2, markersize=8, label=column)

ax1.set_xlabel('Matrix Size', fontsize=14)
ax1.set_ylabel('Execution Time (seconds)', fontsize=14)
ax1.set_title('Matrix Multiplication Performance Comparison', fontsize=16)
ax1.legend(title='Loop Ordering', title_fontsize=13, fontsize=12)
ax1.grid(True)

# Add annotations for the largest matrix size
max_size = df['Matrix Size'].max()
max_size_df = df[df['Matrix Size'] == max_size]
for column in df.columns[1:]:
    ax1.annotate(f"{max_size_df[column].values[0]:.4f}s", 
                xy=(max_size, max_size_df[column].values[0]),
                xytext=(10, 0), textcoords='offset points',
                ha='left', va='center', fontsize=10)

# Plot 2: Bar chart for the largest matrix size
ax2 = axes[1]
max_size_data = max_size_df.iloc[0, 1:].reset_index()
max_size_data.columns = ['Implementation', 'Time']
max_size_data = max_size_data.sort_values('Time')  # Sort by execution time

# Create bar chart
bars = ax2.bar(max_size_data['Implementation'], max_size_data['Time'], color=sns.color_palette('husl', len(max_size_data)))

# Add data labels on top of bars
for bar in bars:
    height = bar.get_height()
    ax2.text(bar.get_x() + bar.get_width()/2., height + 0.01,
             f'{height:.4f}s', ha='center', va='bottom', fontsize=10)

ax2.set_xlabel('Loop Ordering Implementation', fontsize=14)
ax2.set_ylabel('Execution Time (seconds)', fontsize=14)
ax2.set_title(f'Performance Comparison for {max_size}×{max_size} Matrix', fontsize=16)
ax2.grid(axis='y')

# Add relative speedup compared to slowest implementation
slowest_time = max_size_data['Time'].max()
for i, (_, row) in enumerate(max_size_data.iterrows()):
    speedup = slowest_time / row['Time']
    ax2.text(i, row['Time'] / 2, f'{speedup:.2f}x', 
             ha='center', va='center', color='white', fontweight='bold', fontsize=11)

# Create a table with the raw data
df_styled = df.style.format({col: "{:.6f}" for col in df.columns if col != "Matrix Size"})
table_data = df.to_string(index=False)

# Add tight layout
plt.tight_layout()

# Save the figure
plt.savefig('gemm_performance.png', dpi=300, bbox_inches='tight')

# Print some analysis
print("\nPerformance Analysis:")
print("-" * 50)
fastest_impl = max_size_data.iloc[0]['Implementation']
slowest_impl = max_size_data.iloc[-1]['Implementation']
fastest_time = max_size_data.iloc[0]['Time']
slowest_time = max_size_data.iloc[-1]['Time']
speedup = slowest_time / fastest_time

print(f"For {max_size}×{max_size} matrices:")
print(f"Fastest implementation: {fastest_impl} ({fastest_time:.6f} seconds)")
print(f"Slowest implementation: {slowest_impl} ({slowest_time:.6f} seconds)")
print(f"Speedup factor: {speedup:.2f}x")

# Also create a heatmap to visualize the relative performance across all matrix sizes
plt.figure(figsize=(14, 10))
matrix_sizes = df['Matrix Size'].values
implementations = df.columns[1:]

# Create a new dataframe that shows the relative performance
# normalized by the fastest implementation for each matrix size
heatmap_data = []
for size in matrix_sizes:
    size_data = df[df['Matrix Size'] == size].iloc[0, 1:].reset_index()
    size_data.columns = ['Implementation', 'Time']
    min_time = size_data['Time'].min()
    row_data = {'Matrix Size': size}
    for impl in implementations:
        impl_time = df[df['Matrix Size'] == size][impl].values[0]
        # Calculate relative slowdown (1.0 means fastest)
        row_data[impl] = impl_time / min_time
    heatmap_data.append(row_data)

heatmap_df = pd.DataFrame(heatmap_data)
heatmap_df = heatmap_df.set_index('Matrix Size')

# Plot the heatmap
plt.figure(figsize=(12, 8))
sns.heatmap(heatmap_df, annot=True, cmap='YlOrRd', fmt='.2f', linewidths=.5)
plt.title('Relative Performance (higher values = slower)')
plt.tight_layout()
plt.savefig('gemm_heatmap.png', dpi=300, bbox_inches='tight')

print("\nVisualization complete! Check gemm_performance.png and gemm_heatmap.png")
print("\nRaw timing data:")
print(table_data)