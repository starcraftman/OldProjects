/**
 * Reference implementation of floyd's algorithm serial implementation parallel version.
 * This implementation will start at root, distributing all work to the nodes then
 * going through k rounds where we synch after each one the c/p matrices.
 * At the end, we can gather the nodes back at root for final print to file.
 *
 * Use command: bsub -I -q COMP428 -n <tasks> mpirun -srun ./demo/serial <nodes> <mode>
 *
 * Arguments to serial:
 * tasks: Number of tasks to execute against. Due to nature of algorithm, ensure it is a squared even number: 4, 16, 36...
 * nodes: The amount of nodes in a graph.
 * mode: Flag that makes master generate a new input.
 * 		-> Use "gen" to generate new input. OVERWRITES existing input file.
 * 		-> Use anything else to use existing input.txt.
 */
/********************* Header Files ***********************/
/* C Headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Project Headers */
#include "mpi.h"
#include "lib_f.h"

/******************* Constants/Macros *********************/


/******************* Type Definitions *********************/


/**************** Static Data Definitions *****************/


/****************** Static Functions **********************/


/**************** Global Data Definitions *****************/


/****************** Global Functions **********************/
/*
 * Main execution body.
 */
int main(int argc, char **argv) {
    /* Rank is my id in comm_world, world is num procs total, root_world is num of rows/cols required. */
    int rank = 0, world = 0, root_world, nodes = 0, per_node = 0, row_id = 0, col_id = 0;
    int *store_c = NULL, *store_p = NULL, **c = NULL, **p = NULL;
    double start = 0.0;
    FILE *log;
    MPI_Comm comm_row, comm_col; /* Communicators to bcast column or row. */

    /* Standard init for MPI, start timer after init. */
    MPI_Init(&argc, &argv);
    start = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world);

    /* Init rand, open output log and memset buffer for path. */
    srand(time(NULL));
    log = fopen(OUTPUT, "w");

    if (rank == ROOT) {
        if (argc < 3)
            lib_error("MAIN: Error in usage, see notes in c file.");
    }

    /* Get the number of nodes, ints per_node and root_world which is the num of rows and cols needed in mesh. */
    nodes = atoi(*++argv);
    per_node = (nodes*nodes)/world;
    root_world = lib_sqrt(world);

    /* Due to 2D mesh division, check requirements for even operation. */
    if (rank == ROOT) {
        /* Ensure we have even split amongst processors. */
        if ((nodes % world) != 0) {
            lib_error("MAIN: Choose a number of nodes that is a multiple of the processors.");
            return 1;
        }

        /* Ensure we have an even sqrt(p) value. */
        if (root_world == 0) {
            lib_error("The number of processors must have even square root values.\n"
                    "For example, p = 16, root = 4.\n");
            return 1;
        }
    }

    /* Row and col id calculated by div and mod easily. */
    row_id = rank/root_world;
    col_id = rank%root_world;

    /* Create the row and column communicators needed. */
    MPI_Comm_split(MPI_COMM_WORLD, row_id, rank, &comm_row);
    MPI_Comm_split(MPI_COMM_WORLD, col_id, rank, &comm_col);

    /* Allocate cost matrices on heap, they are nodes*nodes large. */
    store_c = (int *)malloc(nodes * nodes * sizeof(int));
    if (store_c == NULL)
        lib_error("MAIN: Can't allocate storage for cost on heap.");

    /* Take contiguous 1D array and make c be a 2d pointer into it. */
    c = (int **)malloc(nodes * sizeof(int*));
    if (c == NULL)
        lib_error("MAIN: Can't allocate cost pointers array on heap.");

    for (int i = 0; i < nodes; ++i)
        c[i] = store_c+(i*nodes);

    /* Allocate path same as cost above. */
    store_p = (int *)malloc(nodes * nodes * sizeof(int));
    if (store_p == NULL)
        lib_error("MAIN: Can't allocate storage for path on heap.");

    /* Take contiguous 1D array and make p be a 2d pointer into it. */
    p = (int **)malloc(nodes * sizeof(int*));
    if (p == NULL)
        lib_error("MAIN: Can't allocate path pointers array on heap.");

    for (int i = 0; i < nodes; ++i)
        p[i] = store_p+(i*nodes);

    /* At this point, c and p are nxn matrices put on heap and freed later. Now root can initialise values. */
    if (rank == ROOT) {
        lib_init_cost(c, nodes);
        lib_init_path(p, nodes);

        /* If requested, generate new input file. */
        if (strcmp(*++argv, GENERATE_FLAG) == 0) {
            lib_generate_graph(c, nodes);
            lib_write_cost_matrix(INPUT, c, nodes);
        }

        /* Read back input from file into array on heap. */
        lib_read_cost_matrix(INPUT, c, nodes);
        fprintf(log, "Initial arrays cost and path.\nCost:\n");
        lib_trace_matrix(log, c, nodes);
        fprintf(log, "Path:\n");
        lib_trace_matrix(log, p, nodes);
    }

    /* Broadcast the values to all and start parallel execution. */
    MPI_Bcast(c, nodes*nodes, MPI_INT, ROOT, MPI_COMM_WORLD);

    int nodes_per_block = nodes/root_world;
    /* Start parallel algorithm. */
    for (int k = 0; k < nodes; ++k) {
        for (int i = 0; i < nodes; ++i) {
            for (int j = 0; j < nodes; ++j) {
                int new_dist = c[i][k] + c[k][j];
                if (new_dist < c[i][j]) {
                    c[i][j] = new_dist;
                    p[i][j] = k;
                }
            }
        }
        int row_rank = rank, col_rank = rank;
        int b_root = k/nodes_per_block;
        MPI_Bcast(&row_rank, 1, MPI_INT, b_root, comm_row);
        MPI_Bcast(&col_rank, 1, MPI_INT, b_root, comm_col);

        printf("I am %d of world %d, at k round %d I have:\nrow = %d, col = %d.\n", rank, world, k, row_rank, col_rank);

        MPI_Barrier(MPI_COMM_WORLD);
    }
    /* End algorithm. */

    if (rank == ROOT) {
        /* Log final. */
        fprintf(log, "After determining the shortest path.\nCost:\n");
        lib_trace_matrix(log, c, nodes);
        fprintf(log, "Path:\n");
        lib_trace_matrix(log, p, nodes);

        /* Dump final cost and path matrix to anaylze later. */
        lib_write_cost_matrix(COST_FILE, c, nodes);
        lib_write_cost_matrix(PATH_FILE, p, nodes);

        fprintf(stdout, "Time elapsed from MPI_Init to MPI_Finalize is %.10f seconds.\n", MPI_Wtime() - start);
    }

    /* Clean up the heap allocation. */
    MPI_Comm_free(&comm_row);
    MPI_Comm_free(&comm_col);
    free(c);
    free(p);
    free(store_c);
    free(store_p);
    fclose(log);
    MPI_Finalize();

    return 0;
}
