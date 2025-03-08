import matplotlib.pyplot as plt
import os

def read_path(file_path):
    """Read path coordinates from file."""
    path = []
    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            if line:
                # Remove parentheses and split by comma
                line = line.strip("()")
                parts = line.split(',')
                if len(parts) == 2:
                    try:
                        x = int(parts[0].strip())
                        y = int(parts[1].strip())
                        path.append((x, y))
                    except ValueError:
                        continue
    return path

def main():
    file_path = 'shortest_paths/2d_parallel/path_from_(0,0)_to_(999,500)_random_parallel.txt'
    path = read_path(file_path)
    if not path:
        print("No valid path found in", file_path)
        return

    xs = [coord[0] for coord in path]
    ys = [coord[1] for coord in path]

    fig, ax = plt.subplots(figsize=(10, 10))

    # Set axis limits and grid
    ax.set_xlim(-1, 1000)
    ax.set_ylim(-1, 1000)
    ax.set_xticks(range(0, 1001, 50))
    ax.set_yticks(range(0, 1001, 50))
    ax.grid(True, which='both', linestyle='--', linewidth=0.5)

    # Plot the path
    ax.plot(xs, ys, marker='o', color='red', linewidth=2, markersize=3)

    # Invert y-axis if needed
    ax.invert_yaxis()

    # Set plot labels and title
    ax.set_title("Shortest Path on 1000x1000 Grid")
    ax.set_xlabel("X Coordinate")
    ax.set_ylabel("Y Coordinate")
    
    # Use os.path.splitext for a more robust file name change
    output_file = os.path.splitext(file_path)[0] + ".png"
    
    # Save the figure
    plt.savefig(output_file, dpi=300)
    print(f"Image saved as: {output_file}")
    
    # Display the plot and close the figure
    plt.show()
    plt.close(fig)

if __name__ == '__main__':
    main()
