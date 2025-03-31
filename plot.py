import pandas as pd
import matplotlib.pyplot as plt
import io

# Your CSV data (simulated file content)
csv_data = """Matrix Size,Memory Usage (KB),MWK (seconds),NKM (seconds),NMM (seconds),KMM (seconds),KMM (seconds)
100,234.38,0.010607,0.008232,0.006688,0.006434,0.005107,0.005387
200,937.50,0.049729,0.035004,0.045535,0.041664,0.038968,0.042442
300,2109.38,0.165559,0.121958,0.159294,0.152627,0.135501,0.152614
400,3750.00,0.429323,0.288641,0.391532,0.404647,0.324930,0.417911
500,5859.38,0.832577,0.554209,0.767495,0.878514,0.640108,0.870468
600,8437.50,1.427147,0.973875,1.298299,1.472849,1.046965,1.424216
700,11484.38,2.553888,1.543311,2.071998,2.354806,1.890232,2.474665
800,15000.00,3.809718,2.260437,3.192867,3.666493,2.466089,3.750977
900,18984.38,6.332193,3.244167,4.576203,5.743080,3.523851,5.680168
1000,23437.50,8.166172,4.418896,6.266733,8.849952,5.482032,8.726711"""

# Read CSV data
df = pd.read_csv(io.StringIO(csv_data))

# Clean column names (there's a duplicate "KMM (seconds)" column)
df.columns = ['Matrix Size', 'Memory Usage (KB)', 'MWK (seconds)', 'NKM (seconds)', 
             'NMM (seconds)', 'KMM1 (seconds)', 'KMM2 (seconds)', 'KMM3 (seconds)']

# Create plot
plt.figure(figsize=(12, 8))

# Plot each algorithm's timing
algorithms = ['MWK (seconds)', 'NKM (seconds)', 'NMM (seconds)', 
             'KMM1 (seconds)', 'KMM2 (seconds)', 'KMM3 (seconds)']

for algo in algorithms:
    plt.plot(df['Matrix Size'], df[algo], marker='o', label=algo)

# Add labels and title
plt.xlabel('Matrix Size', fontsize=12)
plt.ylabel('Execution Time (seconds)', fontsize=12)
plt.title('Matrix Multiplication Algorithm Performance Comparison', fontsize=14)
plt.grid(True, which='both', linestyle='--', alpha=0.7)
plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')

# Add secondary axis for memory usage
ax2 = plt.gca().twinx()
ax2.plot(df['Matrix Size'], df['Memory Usage (KB)'], 'k--', label='Memory Usage (KB)')
ax2.set_ylabel('Memory Usage (KB)', fontsize=12)

# Show plot
plt.tight_layout()
plt.show()