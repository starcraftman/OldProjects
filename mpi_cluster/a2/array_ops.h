#ifndef _SHARED_H_
#define _SHARED_H_

/********************* Header Files ***********************/
/* C Headers */

/* Project Headers */

/******************* Constants/Macros *********************/
/* Flag to pass in to generate new input.txt */
#define GENERATE_FLAG       "gen"
/* Max value of randomly generated numbers */
#define MAX_VAL             10000
/* Root ID */
#define ROOT                0

/******************* Type Declarations ********************/
/* Struct contains information on a subgroup in a hypercube. */
typedef struct subgroup_info_s {
    int group_size; /* Size of subgroup. */
    int group_num; /* Number of the group, starts at 0. */
    int member_num; /* Position in the relevant subgroup, starts at 0. */
    int partner; /* Partner to exchange data with during the hypercube round. */
    int world_id; /* ID of this process in the MPI_COMM_WORLD group. */
} subgroup_info_t;

/********************** Prototypes ************************/
/*
 * Generic error function, prints out the error and terminates execution.
 */
void lib_error(const char * const mesg);

/*
 * Standard increasing comparator for qsort.
 */
int lib_compare(const void *a, const void *b);

/*
 * Function generates size random numbers between 0 and MAX_VAL. All values put in vals array.
 */
void lib_generate_numbers(int vals[], int size);

/*
 * Returns the pivot based on passed in array.
 * Currently: Median of three algorithm, select median of first, last and middle elements.
 */
int lib_select_pivot(int vals[], const int size);

/*
 * Simple swap function.
 */
void lib_swap(int *a, int *b);

/*
 * Partition the passed in array in place. All elements at the front will be less than or equal to pivot.
 * All elements at back will be strictly greater than pivot.
 */
void lib_partition_by_pivot_val(int pivot, int vals[], const int vals_size, int *lt_size, int *gt_size);

/*
 * Partition the passed in array in place. All elements at the front will be less than or equal to pivot.
 * All elements at back will be strictly greater than pivot. Pivot will be in the middle and index will be returned.
 */
int lib_partition_by_pivot_index(int pivot_index, int *left, int *right);

/*
 * Select the kth largest element from the range of values in the array.
 * Starts at k = 1..N.
 */
int lib_select_kth(int kth, int *left, int *right);

/* This function groups values into blocks of five and then selects a median.
 * All such medians are collected at the front and the median of this group is selected as the true median.
 */
int lib_median_of_medians(int *vals, int left, int right);

/*
 * Integer power function, takes log(n) steps to compute.
 */
int lib_power(const int base, const unsigned int exp);

/*
 * Returns true only if the id passed in is the root of the given subgroup domain.
 * The root of any given subgroup has d 0's starting from the right.
 * That basically means it is some modulo, I'll also return the subgroup.
 */
void lib_subgroup_info(const int dimension, subgroup_info_t *info);

/*
 * Function takes two arrays of passed size and merges them into array a.
 * Assumes that a is in fact a malloced array that can be freed.
 */
void lib_array_union(int *a[], int *a_size, const int b[], const int b_size);

/*
 * Once gathered, root array contains a lot of empty values as -1.
 * Compress down array to be contiguous.
 */
void lib_compress_array(int world, int root[], int root_size);

/*
 * Select the required number of medians and order them at the front of the array.
 */
int lib_select_medians(int *vals, const int left, const int right);

/*
 * Function isn't the nicest, could be refactored to be more elegant. For now,
 * it will calculate and select the right number of pivots and put them in pivots array for any dimension from 1-3.
 */
void lib_select_pivots_from_medians(const int dimension, int *pivots, const int pivots_size, int *vals, const int vals_size);

#endif /* _SHARED_H_ */
