import matplotlib.pyplot as plt
import pandas as pd

def plot_execution_time(csv_file):
    save_path = "execution_time_vs_threads/" + csv_file.split(".")[0] + ".png"
    df = pd.read_csv(csv_file)
    
    plt.figure(figsize=(10, 6))
    
    sources = df['Source'].unique()
    
    for source in sources:
        source_df = df[df['Source'] == source]
        threads = source_df['Threads']
        times = source_df['Execution time']
        
        plt.plot(threads, times, marker='o', label=f"Source {source}")
    
    plt.xlabel("Number of Threads")
    plt.ylabel("Execution Time (seconds)")
    plt.title("Execution Time vs. Number of Threads for Different Sources")
    plt.legend()
    plt.grid(True)
    plt.xticks(rotation=45)
     
    plt.savefig(save_path)
    plt.show()

data_files = ['2d_1.csv', '2d_random.csv', '3d_1.csv', '3d_random.csv']
for file in data_files:
    file = "benchmark_results/" + file
    plot_execution_time(file)

