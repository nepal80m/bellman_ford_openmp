#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define DYNAMIC_WEIGHT 1
#define INDEX_3D(x, y, z, X_DIM, Y_DIM) ((z) * (X_DIM) * (Y_DIM) + (y) * (X_DIM) + (x))

static const int NEIGH_OFFSETS[6][3] = {
        {+1, 0, 0},
        {-1, 0, 0},
        {0, +1, 0},
        {0, -1, 0},
        {0, 0, +1},
        {0, 0, -1},
    };
    
static inline int inBounds3D(int x, int y, int z, int X, int Y, int Z)
{
    return (x >= 0 && x < X && y >= 0 && y < Y && z >= 0 && z < Z);
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s X_DIM Y_DIM Z_DIM ['random' | <constant edge weight>]\n", argv[0]);
        return 1;
    }

    int X_DIM = atoi(argv[1]);
    int Y_DIM = atoi(argv[2]);
    int Z_DIM = atoi(argv[3]);
    int CONSTANT_WEIGHT = 1;
    int USE_RANDOM_WEIGHTS = 0;
    srand(time(NULL));
    
    if (argc == 5)
    {
        if (strcmp(argv[4], "random") == 0)
        {
            USE_RANDOM_WEIGHTS = 1;
        }
        else
        {
            CONSTANT_WEIGHT = atoi(argv[4]);
        }
    }
    char out_filename[33];
    if (USE_RANDOM_WEIGHTS)
        sprintf(out_filename, "graph3d_%dx%dx%d_random.txt", X_DIM, Y_DIM, Z_DIM);
    else
        sprintf(out_filename, "graph3d_%dx%dx%d_%d.txt", X_DIM, Y_DIM, Z_DIM, CONSTANT_WEIGHT);

    // Open file for writing
    FILE *fp = fopen(out_filename, "w");
    if (!fp)
    {
        perror("fopen");
        return 1;
    }

    // fprintf(fp, "%d %d %d\n", X_DIM, Y_DIM, Z_DIM);
    

    for (int z = 0; z < Z_DIM; z++)
    {
        for (int y = 0; y < Y_DIM; y++)
        {
            for (int x = 0; x < X_DIM; x++)
            {

                int v1 = INDEX_3D(x, y, z, X_DIM, Y_DIM);
                for(int i =0; i<6; i+=2){
                    int nx = x + NEIGH_OFFSETS[i][0];
                    int ny = y + NEIGH_OFFSETS[i][1];
                    int nz = z + NEIGH_OFFSETS[i][2];
                    if (inBounds3D(nx, ny, nz, X_DIM, Y_DIM, Z_DIM))
                    {
                        int v2 = INDEX_3D(nx, ny, nz, X_DIM, Y_DIM);
                        int edge_weight = USE_RANDOM_WEIGHTS ? rand() % 10 + 1 : CONSTANT_WEIGHT;
                        fprintf(fp, "%d %d %d\n", v1, v2, edge_weight);
                        fprintf(fp, "%d %d %d\n", v2, v1, edge_weight);
                    }

                }
            }
        }
    }

    fclose(fp);

    printf("Graph (3D: %dx%dx%d) written to: %s\n", X_DIM, Y_DIM, Z_DIM, out_filename);
    return 0;
}
