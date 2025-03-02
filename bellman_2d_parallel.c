#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#define INDEX(x, y, X_DIM) ((y) * X_DIM + (x))

#define GET_X_COORD(index, X_DIM) ((index) % (X_DIM))
#define GET_Y_COORD(index, X_DIM) ((index) / (X_DIM))

static const int NEIGH_OFFSETS[4][2] = {
    {+1, 0},
    {-1, 0},
    {0, +1},
    {0, -1},
};

void relax_using_neighbors(int index, double *D, int *P, double *updated_D, int *updated_P, int *was_updated, double (*W)[4], int X_DIM, int Y_DIM)
{
    updated_D[index] = D[index];
    updated_P[index] = P[index];

    for (int i = 0; i < 4; i++)
    {
        int x = GET_X_COORD(index, X_DIM);
        int y = GET_Y_COORD(index, X_DIM);
        int neigh_x = x + NEIGH_OFFSETS[i][0];
        int neigh_y = y + NEIGH_OFFSETS[i][1];

        if (neigh_x >= 0 && neigh_x < X_DIM && neigh_y >= 0 && neigh_y < Y_DIM)
        {
            int neigh_index = INDEX(neigh_x, neigh_y, X_DIM);
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

int run_bellman_ford(double *D, int *P, double (*W)[4], int V, int X_DIM, int Y_DIM)
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
            relax_using_neighbors(index, D, P, updated_D, updated_P, &was_updated, W, X_DIM, Y_DIM);
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
        relax_using_neighbors(index, D, P, updated_D, updated_P, &was_updated, W, X_DIM, Y_DIM);
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

void initialize_graph(double *D, int *P, double (*W)[4], int V, int X_DIM, int Y_DIM, char *filename)
{

    for (int i = 0; i < V; i++)
    {
        D[i] = INFINITY;
        P[i] = -1;
        for (int j = 0; j < 4; j++)
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
        int x = GET_X_COORD(v1, X_DIM);
        int y = GET_Y_COORD(v1, X_DIM);
        int neigh_x = GET_X_COORD(v2, X_DIM);
        int neigh_y = GET_Y_COORD(v2, X_DIM);
        if (neigh_x == x + 1 && neigh_y == y)
            W[v1][0] = weight;
        else if (neigh_x == x - 1 && neigh_y == y)
            W[v1][1] = weight;
        else if (neigh_x == x && neigh_y == y + 1)
            W[v1][2] = weight;
        else if (neigh_x == x && neigh_y == y - 1)
            W[v1][3] = weight;
        else
            printf("Error: invalid edge (%d, %d)\n", v1, v2);
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{
    const int X_DIM = 1000;
    const int Y_DIM = 1000;
    int V = X_DIM * Y_DIM;
    const int USE_RANDOM_WEIGHTS = 1;

    char *filename = "graph2d_1000x1000_1.txt";
    if (USE_RANDOM_WEIGHTS)
        filename = "graph2d_1000x1000_random.txt";

    const int DESTINATIONS[10][2] = {
        {10, 10},
        {100, 200},
        {250, 250},
        {250, 750},
        {500, 250},
        {500, 750},
        {500, 999},
        {750, 500},
        {800, 800},
        {999, 500},
    };

    int SOURCE_X;
    int SOURCE_Y;

    if (argc == 1)
    {
        SOURCE_X = 0;
        SOURCE_Y = 0;
    }
    else if (argc == 3)
    {
        SOURCE_X = atoi(argv[1]);
        SOURCE_Y = atoi(argv[2]);
    }
    else
    {
        printf("Usage: %s <SOURCE_X> <SOURCE_Y>\n", argv[0]);
        return 1;
    }

    double *D = (double *)malloc(V * sizeof(double));
    int *P = (int *)malloc(V * sizeof(int));
    double(*W)[4] = malloc(V * sizeof(*W));

    initialize_graph(D, P, W, V, X_DIM, Y_DIM, filename);

    D[INDEX(SOURCE_X, SOURCE_Y, X_DIM)] = 0;

    struct timespec start, end;
    double elapsed;

    printf("Starting Bellman-Ford algorithm...\n");

    clock_gettime(CLOCK_MONOTONIC, &start);
    int n_iteration = run_bellman_ford(D, P, W, V, X_DIM, Y_DIM);
    clock_gettime(CLOCK_MONOTONIC, &end);

    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Execution time: %f seconds\n", elapsed);

    if (n_iteration != -1)
    {
        // Draw table to show the shortest path distance to above destinations
        printf("Shortest path distance from (%d, %d) to the following destinations:\n", SOURCE_X, SOURCE_Y);
        for (int i = 0; i < 10; i++)
        {
            int dest_x = DESTINATIONS[i][0];
            int dest_y = DESTINATIONS[i][1];
            printf("(%d, %d): %.1f\n", dest_x, dest_y, D[INDEX(dest_x, dest_y, X_DIM)]);
        }

        // Write path from source to each destination to separate file in reverse order from source to destination

        for (int i = 0; i < 10; i++)
        {
            int dest_x = DESTINATIONS[i][0];
            int dest_y = DESTINATIONS[i][1];
            char filename[50];

            sprintf(filename, "path_from_(%d,%d)_to_(%d,%d)_1_parallel.txt", SOURCE_X, SOURCE_Y, dest_x, dest_y);
            if (USE_RANDOM_WEIGHTS)
                sprintf(filename, "path_from_(%d,%d)_to_(%d,%d)_random_parallel.txt", SOURCE_X, SOURCE_Y, dest_x, dest_y);

            FILE *fp = fopen(filename, "w");
            if (!fp)
            {
                perror("fopen");
                return 1;
            }
            int index = INDEX(dest_x, dest_y, X_DIM);
            while (index != -1)
            {
                fprintf(fp, "(%d, %d)\n", GET_X_COORD(index, X_DIM), GET_Y_COORD(index, X_DIM));
                index = P[index];
            }
            fclose(fp);
        }
    }

    free(D);
    free(P);
    free(W);
    return 0;
}