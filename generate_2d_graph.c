#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INDEX_2D(x, y, X_DIM) ((y) * X_DIM + (x))

static const int NEIGH_OFFSETS[4][2] = {
    {+1, 0},
    {-1, 0},
    {0, +1},
    {0, -1},
};

static inline int inBounds2D(int x, int y, int X, int Y)
{
    return (x >= 0 && x < X && y >= 0 && y < Y);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s X_DIM Y_DIM\n", argv[0]);
        return 1;
    }

    int X_DIM = atoi(argv[1]);
    int Y_DIM = atoi(argv[2]);
    int CONSTANT_WEIGHT = 1;
    int USE_RANDOM_WEIGHTS = 0;
    srand(time(NULL));

    if (argc == 4)
    {
        if (strcmp(argv[3], "random") == 0)
        {
            USE_RANDOM_WEIGHTS = 1;
        }
        else
        {
            CONSTANT_WEIGHT = atoi(argv[3]);
        }
    }

    char out_filename[28];
    if (USE_RANDOM_WEIGHTS)
        sprintf(out_filename, "graphs/graph2d_%dx%d_random.txt", X_DIM, Y_DIM);
    else
        sprintf(out_filename, "graphs/graph2d_%dx%d_%d.txt", X_DIM, Y_DIM, CONSTANT_WEIGHT);

    FILE *fp = fopen(out_filename, "w");
    if (!fp)
    {
        perror("fopen");
        return 1;
    }

    // fprintf(fp, "%d %d\n", X_DIM, Y_DIM);

    for (int y = 0; y < Y_DIM; y++)
    {
        for (int x = 0; x < X_DIM; x++)
        {

            int v1 = INDEX_2D(x, y, X_DIM);

            for (int i = 0; i < 4; i += 2)
            {
                int nx = x + NEIGH_OFFSETS[i][0];
                int ny = y + NEIGH_OFFSETS[i][1];
                if (inBounds2D(nx, ny, X_DIM, Y_DIM))
                {
                    int v2 = INDEX_2D(nx, ny, X_DIM);
                    int edge_weight = USE_RANDOM_WEIGHTS ? rand() % 10 + 1 : CONSTANT_WEIGHT;
                    fprintf(fp, "%d %d %d\n", v1, v2, edge_weight);
                    fprintf(fp, "%d %d %d\n", v2, v1, edge_weight);
                }
            }
        }
    }

    fclose(fp);

    printf("Graph (2D: %dx%d) written to: %s\n", X_DIM, Y_DIM, out_filename);
    return 0;
}
