/**
 * Library functions that are common to both my serial and parallel
 * quicksort implementations. Also in general the functions that can be tested.
 */
/********************* Header Files ***********************/
/* C Headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Project Headers */
#include "shared.h"

/******************* Constants/Macros *********************/


/******************* Type Definitions *********************/


/**************** Static Data Definitions *****************/


/****************** Static Functions **********************/


/**************** Global Data Definitions *****************/


/****************** Global Functions **********************/
/*
 * Generic error function, prints out the error and terminates execution.
 */
void lib_error(const char * const mesg) {
	printf("ERROR: %s.\n", mesg);
	exit(1);
}

/*
 * Standard increasing comparator for qsort.
 */
int lib_compare (const void *a, const void *b) {
	return *((int *)a) - *((int *)b);
}

/*
 * Function generates size random numbers between 0 and MAX_VAL. All values put in vals array.
 */
void lib_generate_numbers(int vals[], int size) {
	srand(time(NULL));

	for (int i = 0; i < size; ++i)
		vals[i] = rand() % MAX_VAL;
}

/*
 * Opens the filename passed in and reads size numbers into the vals array.
 */
void lib_read_file(const char *filename, int *vals, const int size) {
	// Note on size, I start it at -1 to compensate for the below while loop going one extra time.
	int i;
	FILE *f;

	if ((f = fopen(filename, "r")) == NULL)
		lib_error("READ: Failed to open file.");

	for (i = 0; i < size; ++i)
		fscanf(f, "%d", vals+i);

	if (fclose(f) != 0)
		lib_error("READ: Failed to close properly.");
}

/*
 * Function takes an array of values and prints them to a file.
 * Each integer on a separate line.
 */
void lib_write_file(const char *filename, const int *vals, const int size) {
	FILE *f;

	if ((f = fopen(filename, "w")) == NULL)
		lib_error("WRITE: Failed to open file.");

	for (int i = 0; i < size; ++i)
		fprintf(f, "%d\n", vals[i]);

	if (fclose(f) != 0)
		lib_error("WRITE: Failed to close properly.");
}


/*
 * Returns the pivot based on passed in array.
 * Currently: Median of three algorithm, select median of first, last and middle elements.
 */
int lib_select_pivot(int vals[], const int size) {
//	const int start = 0, mid = size/2, end = size-1;
//	int large, small, pivot;

	qsort(vals, size, sizeof(int), lib_compare);
	return vals[size/2];

//	/* Find the largest and smallest of first two. */
//	if (vals[start] > vals[mid]) {
//		large = start;
//		small = mid;
//	} else {
//		large = mid;
//		small = start;
//	}
//
//	/* If end greater than large it is median. Else determine middle of small and end. */
//	if (vals[end] > vals[large]) {
//		pivot = vals[large];
//	} else {
//		if (vals[end] > vals[small]) {
//			pivot = vals[end];
//		} else {
//			pivot = vals[small];
//		}
//	}
}

/*
 * Simple swap function.
 */
void lib_swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

/*
 * Partition the passed in array in place. All elements at the front will be less than or equal to pivot.
 * All elements at back will be strictly greater than pivot.
 */
void lib_partition_array(int pivot, int vals[], const int vals_size, int *lt_size, int *gt_size) {
	int *left = vals, *right = vals + vals_size-1;
	*lt_size = 0;

	/* Done when left and right cross makes difference negative. */
	while ((right-left) > 0) {
		/* Scan until left points to larger than pivot and right points to less than pivot.  */
		while ((right-left) > 0 && *left <= pivot) {
			++left;
			++*lt_size;
		}
		while ((right-left) > 0 && *right > pivot) {
			--right;
		}
		if ((right-left) > 0)
			lib_swap(left, right);
	}

	*gt_size = vals_size - *lt_size;
}

/*
 * Integer power function, takes log(n) steps to compute.
 */
int lib_power(const int base, const unsigned int exp) {
	int temp;

	if (exp == 0)
		return 1;

	// Odd power.
	if (exp % 2) {
		temp = lib_power(base, (exp-1)/2);
		return temp * temp * base;
	} else {
		temp = lib_power(base, exp/2);
		return temp * temp;
	}
}

/*
 * Returns true only if the id passed in is the root of the given subgroup domain.
 * The root of any given subgroup has d 0's starting from the right.
 * That basically means it is some modulo, I'll also return the subgroup and it's partner.
 */
int lib_subgroup_info(const int dimension, const int id, int *group, int *member, int *partner) {
	int val = lib_power(2, dimension);

	*member = id % val;
	*group = id / val;
	*partner = id ^ (1<<(dimension-1));

	return id == 0;
}

/*
 * Function takes two arrays of passed size and merges them into array a.
 * Assumes that a is in fact a malloced array that can be freed.
 */
void lib_array_union(int *a[], int *a_size, const int b[], const int b_size) {
	/* New array of combined size, then copy in the contents of a and b. */
	int *temp = malloc((*a_size + b_size) * sizeof(int));
	if (temp == NULL)
		lib_error("LIB_ARRAY_UNION: Unable to allocate new array.");

	memcpy(temp, *a, (*a_size)*sizeof(int));
	memcpy(temp+(*a_size), b, b_size*sizeof(int));

	/* Free old a array. Then update pointer and size. */
	free(*a);
	*a = temp;
	*a_size += b_size;
}

/*
 * Tracing function, takes a buffer of adequate size and traces the values in array.
 */
void lib_trace_array(char *buf, int buf_size, char *tag, int array[], int size, int id) {
	int count;

	/* Using snprintf to avoid overflowing the buffer if too small. */
	count = snprintf(buf, buf_size, "%s: I am %d. I have numbers: ", tag, id);
	if ((buf_size - count) < 1)
		lib_error("LIB_TRACE: Insufficient buffer for print.");

	for (int i = 0; i < size; ++i) {
		count += snprintf(buf+count, buf_size-count, "%d ", array[i]);
		if ((buf_size-count) < 1)
			lib_error("LIB_TRACE: Insufficient buffer for print.");
	}

	count += snprintf(buf+count, buf_size-count, "\n");
	if ((buf_size-count) < 1)
		lib_error("LIB_TRACE: Insufficient buffer for print.");

	/* Ensure always null terminated, only catches if buffer too small. */
	buf[buf_size-1] = '\0';
}

/*
 * In place merge of multiple arrays.
 */
void lib_merge_arrays() {

}
