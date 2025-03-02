import matplotlib.pyplot as plt

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
    file_path = 'path.txt'
    path = read_path(file_path)
    if not path:
        print("No valid path found in", file_path)
        return

    # Separate x and y coordinates from the path list
    xs = [coord[0] for coord in path]
    ys = [coord[1] for coord in path]

    # Create a figure with a fixed size
    fig, ax = plt.subplots(figsize=(10, 10))

    # Set the limits for a 1000 x 1000 grid
    ax.set_xlim(-1, 1000)
    ax.set_ylim(-1, 1000)

    # Optionally set grid lines every 50 cells for clarity
    ax.set_xticks(range(0, 1001, 50))
    ax.set_yticks(range(0, 1001, 50))
    ax.grid(True, which='both', linestyle='--', linewidth=0.5)

    # Plot the shortest path
    ax.plot(xs, ys, marker='o', color='red', linewidth=2, markersize=3)

    # Invert y-axis if you want the origin (0,0) to appear at the top left like a typical grid
    ax.invert_yaxis()

    # Set titles and labels
    ax.set_title("Shortest Path on 1000x1000 Grid")
    ax.set_xlabel("X Coordinate")
    ax.set_ylabel("Y Coordinate")

    # Save the figure as an image file
    plt.savefig("grid_path.png", dpi=300)
    plt.show()

if __name__ == '__main__':
    main()
