#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <omp.h>

#define INDEX(x, y, z, X_DIM, Y_DIM) ((z) * (X_DIM * Y_DIM) + (y) * X_DIM + (x))

#define GET_X_COORD(index, X_DIM) ((index) % (X_DIM))
#define GET_Y_COORD(index, X_DIM, Y_DIM) (((index) / (X_DIM)) % (Y_DIM))
#define GET_Z_COORD(index, X_DIM, Y_DIM) ((index) / (X_DIM * Y_DIM))

static const int NEIGH_OFFSETS[6][3] = {
    {1, 0, 0},
    {-1, 0, 0},
    {0, 1, 0},
    {0, -1, 0},
    {0, 0, 1},
    {0, 0, -1},
};

void relax_using_neighbors(int index, double *D, int *P, double *updated_D, int *updated_P, int *was_updated, double (*W)[6], int X_DIM, int Y_DIM, int Z_DIM)
{
    updated_D[index] = D[index];
    updated_P[index] = P[index];

    for (int i = 0; i < 6; i++)
    {
        int x = GET_X_COORD(index, X_DIM);
        int y = GET_Y_COORD(index, X_DIM, Y_DIM);
        int z = GET_Z_COORD(index, X_DIM, Y_DIM);
        int neigh_x = x + NEIGH_OFFSETS[i][0];
        int neigh_y = y + NEIGH_OFFSETS[i][1];
        int neigh_z = z + NEIGH_OFFSETS[i][2];

        if (neigh_x >= 0 && neigh_x < X_DIM && neigh_y >= 0 && neigh_y < Y_DIM && neigh_z >= 0 && neigh_z < Z_DIM)
        {
            int neigh_index = INDEX(neigh_x, neigh_y, neigh_z, X_DIM, Y_DIM);
            double edge_weight = W[index][i];

            if (D[neigh_index] == INFINITY)
                continue;

            if (D[neigh_index] + edge_weight < D[index])
            {
                updated_D[index] = D[neigh_index] + edge_weight;
                updated_P[index] = neigh_index;
                *was_updated |= 1;
            }
        }
    }
}

int run_bellman_ford(double *D, int *P, double (*W)[6], int V, int X_DIM, int Y_DIM, int Z_DIM)
{

    int iter;
    double *updated_D = (double *)malloc(V * sizeof(double));
    int *updated_P = (int *)malloc(V * sizeof(int));
    for (iter = 0; iter < V - 1; iter++)
    {

        int was_updated = 0;
#pragma omp parallel for reduction(| : was_updated)
        for (int index = 0; index < V; index++)
        {
            relax_using_neighbors(index, D, P, updated_D, updated_P, &was_updated, W, X_DIM, Y_DIM, Z_DIM);
        }
        if (!was_updated)
            break;

        for (int index = 0; index < V; index++)
        {
            D[index] = updated_D[index];
            P[index] = updated_P[index];
        }

        // double *tempD = D;
        // D = updated_D;
        // updated_D = tempD;
        // int *tempP = P;
        // P = updated_P;
        // updated_P = tempP;
    }

    // Negative cycle check
    int was_updated = 0;
#pragma omp parallel for reduction(| : was_updated)
    for (int index = 0; index < V; index++)
    {
        relax_using_neighbors(index, D, P, updated_D, updated_P, &was_updated, W, X_DIM, Y_DIM, Z_DIM);
    }
    if (was_updated)
    {
        printf("Graph contains a negative-weight cycle\n");
        free(updated_D);
        free(updated_P);
        return -1;
    }

    free(updated_D);
    free(updated_P);
    printf("Converged after %d iterations\n", iter + 1);
    return iter + 1;
}
void initialize_graph(double *D, int *P, double (*W)[6], int V, int X_DIM, int Y_DIM, int Z_DIM, char *filename)
{

    for (int i = 0; i < V; i++)
    {
        D[i] = INFINITY;
        P[i] = -1;
        for (int j = 0; j < 6; j++)
        {
            W[i][j] = INFINITY;
        }
    }

    // read weight from file
    FILE *fp = fopen(filename, "r");

    int v1, v2;
    double weight;
    while (fscanf(fp, "%d %d %lf", &v1, &v2, &weight) == 3)
    {
        if (v1 < 0 || v1 >= V || v2 < 0 || v2 >= V)
        {
            printf("Error: invalid vertex (%d, %d)\n", v1, v2);
            continue;
        }
        int x = GET_X_COORD(v1, X_DIM);
        int y = GET_Y_COORD(v1, X_DIM, Y_DIM);
        int z = GET_Z_COORD(v1, X_DIM, Y_DIM);
        int neigh_x = GET_X_COORD(v2, X_DIM);
        int neigh_y = GET_Y_COORD(v2, X_DIM, Y_DIM);
        int neigh_z = GET_Z_COORD(v2, X_DIM, Y_DIM);
        if (neigh_x == x + 1 && neigh_y == y && neigh_z == z)
            W[v1][0] = weight;
        else if (neigh_x == x - 1 && neigh_y == y && neigh_z == z)
            W[v1][1] = weight;
        else if (neigh_x == x && neigh_y == y + 1 && neigh_z == z)
            W[v1][2] = weight;
        else if (neigh_x == x && neigh_y == y - 1 && neigh_z == z)
            W[v1][3] = weight;
        else if (neigh_x == x && neigh_y == y && neigh_z == z + 1)
            W[v1][4] = weight;
        else if (neigh_x == x && neigh_y == y && neigh_z == z - 1)
            W[v1][5] = weight;
        else
            printf("Error: invalid edge (%d, %d)\n", v1, v2);
    }
    fclose(fp);
}
int write_path_to_file(const int DESTINATIONS[10][3], int SOURCE_X, int SOURCE_Y, int SOURCE_Z, int *P, int USE_RANDOM_WEIGHTS, int X_DIM, int Y_DIM)
{
    for (int i = 0; i < 10; i++)
    {
        int dest_x = DESTINATIONS[i][0];
        int dest_y = DESTINATIONS[i][1];
        int dest_z = DESTINATIONS[i][2];
        char filename[100];

        sprintf(filename, "shortest_paths/3d_parallel/path_from_(%d,%d,%d)_to_(%d,%d,%d)_1.txt", SOURCE_X, SOURCE_Y, SOURCE_Z, dest_x, dest_y, dest_z);
        if (USE_RANDOM_WEIGHTS)
            sprintf(filename, "shortest_paths/3d_parallel/path_from_(%d,%d,%d)_to_(%d,%d,%d)_random.txt", SOURCE_X, SOURCE_Y, SOURCE_Z, dest_x, dest_y, dest_z);

        FILE *fp = fopen(filename, "w");
        if (!fp)
        {
            perror("fopen");
            return 1;
        }
        int index = INDEX(dest_x, dest_y, dest_z, X_DIM, Y_DIM);
        while (index != -1)
        {
            fprintf(fp, "(%d, %d, %d)\n", GET_X_COORD(index, X_DIM), GET_Y_COORD(index, X_DIM, Y_DIM), GET_Z_COORD(index, X_DIM, Y_DIM));
            index = P[index];
        }
        fclose(fp);
    }
}

int main(int argc, char *argv[])
{
    const int X_DIM = 50;
    const int Y_DIM = 50;
    const int Z_DIM = 50;
    int V = X_DIM * Y_DIM * Z_DIM;
    const int USE_RANDOM_WEIGHTS = 1;

    char *filename = "graphs/graph3d_50x50x50_1.txt";
    if (USE_RANDOM_WEIGHTS)
        filename = "graphs/graph3d_50x50x50_random.txt";

    int DESTINATIONS[10][3] = {
        {1, 1, 1},
        {10, 10, 10},
        {25, 25, 0},
        {25, 25, 49},
        {49, 25, 25},
        {25, 49, 25},
        {0, 25, 25},
        {40, 10, 10},
        {49, 49, 0},
        {30, 20, 40},
    };

    int SOURCE_X;
    int SOURCE_Y;
    int SOURCE_Z;

    if (argc == 1)
    {
        SOURCE_X = 0;
        SOURCE_Y = 0;
        SOURCE_Z = 0;
    }
    else if (argc == 4)
    {
        SOURCE_X = atoi(argv[1]);
        SOURCE_Y = atoi(argv[2]);
        SOURCE_Z = atoi(argv[3]);
    }
    else
    {
        printf("Usage: %s <SOURCE_X> <SOURCE_Y> <SOURCE_Z>\n", argv[0]);
        return 1;
    }

    double *D = (double *)malloc(V * sizeof(double));
    int *P = (int *)malloc(V * sizeof(int));
    double(*W)[6] = malloc(V * sizeof(*W));

    initialize_graph(D, P, W, V, X_DIM, Y_DIM, Z_DIM, filename);

    D[INDEX(SOURCE_X, SOURCE_Y, SOURCE_Z, X_DIM, Y_DIM)] = 0;

    struct timespec start, end;
    double elapsed;

    printf("\n\nStarting Bellman-Ford algorithm...\n");

    clock_gettime(CLOCK_MONOTONIC, &start);
    int n_iteration = run_bellman_ford(D, P, W, V, X_DIM, Y_DIM, Z_DIM);
    clock_gettime(CLOCK_MONOTONIC, &end);

    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    if (n_iteration != -1)
    {
        printf("Number of threads: %d\n", omp_get_max_threads());
        printf("Execution time: %f seconds\n", elapsed);
        printf("Shortest path distance from (%d, %d, %d) to the following destinations:\n", SOURCE_X, SOURCE_Y, SOURCE_Z);
        for (int i = 0; i < 10; i++)
        {
            int dest_x = DESTINATIONS[i][0];
            int dest_y = DESTINATIONS[i][1];
            int dest_z = DESTINATIONS[i][2];
            printf("(%d, %d, %d): %.1f\n", dest_x, dest_y, dest_z, D[INDEX(dest_x, dest_y, dest_z, X_DIM, Y_DIM)]);
        }
        write_path_to_file(DESTINATIONS, SOURCE_X, SOURCE_Y, SOURCE_Z, P, USE_RANDOM_WEIGHTS, X_DIM, Y_DIM);
    }

    free(D);
    free(P);
    free(W);
    return 0;
}