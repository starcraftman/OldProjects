/**
 * Template C file.
 */
/********************* Header Files ***********************/
/* C Headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Project Headers */
#include "CUnit/Basic.h"
#include "mpi.h"
#include "shared.h"

/******************* Constants/Macros *********************/
#define TEMP_FILE 		"temp.txt"
#define AR_SIZE			10

/******************* Type Definitions *********************/


/**************** Static Data Definitions *****************/
static FILE *f;

/****************** Static Functions **********************/


/**************** Global Data Definitions *****************/


/****************** Global Functions **********************/
/*
 * Suite initialization function run before each test.
 */
int suite_init(void) {
	if ((f = fopen(TEMP_FILE, "w+")) == NULL)
		return -1;

	return 0;
}

/* The suite cleanup function.
 */
int suite_clean(void) {
	if (fclose(f) != 0)
		return -1;

	remove(TEMP_FILE);

	return 0;
}

/*
 * Compare function, equal case.
 */
void test_compare_equal(void) {
	int a = 9, b = 9;
	CU_ASSERT(lib_compare(&a, &b) == 0);
}

/*
 * Compare function, a less than b case.
 */
void test_compare_less(void) {
	int a = 7, b = 9;
	CU_ASSERT(lib_compare(&a, &b) < 0);
}

/*
 * Compare function, a more than b case.
 */
void test_compare_more(void) {
	int a = 20, b = 9;
	CU_ASSERT(lib_compare(&a, &b) > 0);
}

/*
 * Test the read and write files. Here I've assumed that read and write
 * only work together else they fail.
 */
void test_read_write_file(void) {
	int gen[AR_SIZE], read[AR_SIZE];

	lib_generate_numbers(gen, AR_SIZE);
	lib_write_file(TEMP_FILE, gen, AR_SIZE);
	lib_read_file(TEMP_FILE, read, AR_SIZE);

	for (int i = 0; i < AR_SIZE; ++i) {
		CU_ASSERT(gen[i] == read[i]);
	}
}

/*
 * Test pivot selection, case where numbers are odd.
 */
void test_select_pivot_odd(void) {
	int test_data[] = {5, 9, 22, 99, 33}, expected = 22;
	int selected = lib_select_pivot(test_data, sizeof(test_data)/sizeof(int));

	CU_ASSERT(selected == expected);
}

/*
 * Test pivot selection, case where numbers are even.
 */
void test_select_pivot_even(void) {
	int test_data[] = {5, 9, 22, 99, 33, 2}, expected = 5;
	int selected = lib_select_pivot(test_data, sizeof(test_data)/sizeof(int));

	CU_ASSERT(selected == expected);
}

/*
 * Test simple swap function.
 */
void test_swap_ints(void) {
	int orig_a = 10, orig_b = 20, a = orig_a, b = orig_b;

	lib_swap(&a, &b);
	CU_ASSERT(a == orig_b);
	CU_ASSERT(b == orig_a);
}

/*
 * Test power function when exponent is even.
 */
void test_power_even(void) {
	int base = 2, exp = 10, expected = 1024;
	int actual = lib_power(base, exp);

	CU_ASSERT(actual == expected);
}

/*
 * Test power function when exponent is odd.
 */
void test_power_odd(void) {
	int base = 2, exp = 7, expected = 128;
	int actual = lib_power(base, exp);

	CU_ASSERT(actual == expected);
}

/*
 * Test partitioning of array based on a passed in pivot.
 */
void test_partition_array(void) {
	int pivot = 40, size = 10, orig_data[] = {99, 33, 10, 77, 900, 88, 61, 22, 33, 100};
	int expected_data[] = {33, 33, 10, 22, 900, 88, 61, 77, 99, 100};
	int actual_lt = 0, actual_gt = 0, expected_lt = 4, expected_gt = 6;

	lib_partition_array(pivot, orig_data, size, &actual_lt, &actual_gt);

	CU_ASSERT(actual_lt == expected_lt)
	CU_ASSERT(actual_gt == expected_gt);

	for (int i = 0; i < size; ++i)
		CU_ASSERT(orig_data[i] == expected_data[i]);
}

/*
 * Test array union, takes two arrays and put them into one larger merged array.
 */
void test_array_union(void) {
	int *left, *right, left_size = 10, right_size = 10;
	int left_data[] = {40, 48, 21, 76, 4, 50, 53, 87, 12, 94};
	int right_data[] = {52, 83, 88, 95, 59, 78, 9, 68, 61, 96};
	int expected_size = left_size+right_size;
	int expected[] = {40, 48, 21, 76, 4, 50, 53, 87, 12, 94,
			52, 83, 88, 95, 59, 78, 9, 68, 61, 96};

	/* Allocate arrays then fill with test data. */
	left = malloc(left_size * sizeof(int));
	CU_ASSERT_PTR_NOT_EQUAL_FATAL(left, NULL);

	right = malloc(right_size * sizeof(int));
	CU_ASSERT_PTR_NOT_EQUAL_FATAL(right, NULL);

	/* Put values in malloced arrays then call union. */
	memcpy(left, left_data, left_size * sizeof(int));
	memcpy(right, right_data, right_size * sizeof(int));
	lib_array_union(&left, &left_size, right, right_size);

	CU_ASSERT(left_size == expected_size);
	for (int i = 0; i < expected_size; ++i)
		CU_ASSERT(left[i] == expected[i]);

	/* Fatal assert above prevents calling if the malloc failed. */
	free(left);
	free(right);
}

/*
 * Test to exercise the array tracing function.
 */
void test_trace_array(void) {
	int buf_size = 200, id = 2, ar_size = 5, ar[] = {20, 44, 1, 99, 7};
	char buf[buf_size], tag[] = "TEST", expected[] = "TEST: I am 2. I have numbers: 20 44 1 99 7 \n";

	lib_trace_array(buf, buf_size, tag, ar, ar_size, id);
	CU_ASSERT_STRING_EQUAL(buf, expected);
}

/*
 * Test to exercise the array tracing function.
 */
void test_subgroup_info(void) {
	int dimension = 2, id = 6, group = 0, member = 0, partner = 0;
	int expected_group = 1, expected_member = 2, expected_partner = 4;

	lib_subgroup_info(dimension, id, &group, &member, &partner);

	CU_ASSERT(group == expected_group);
	CU_ASSERT(member == expected_member);
	CU_ASSERT(partner == expected_partner);
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main() {
   CU_pSuite sharedSuite = NULL;

   /* Initialize the CUnit test registry. */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* Add a suite to the registry. */
   sharedSuite = CU_add_suite("Shared Suite", suite_init, suite_clean);
   if (NULL == sharedSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Add the tests to the suite. */
   if (
	   (NULL == CU_add_test(sharedSuite, "Compare: Equal", test_compare_equal)) ||
	   (NULL == CU_add_test(sharedSuite, "Compare: Less", test_compare_less)) ||
	   (NULL == CU_add_test(sharedSuite, "Compare: Greater", test_compare_more)) ||
	   (NULL == CU_add_test(sharedSuite, "Write/Read", test_read_write_file)) ||
	   (NULL == CU_add_test(sharedSuite, "Select Pivot, Odd", test_select_pivot_odd)) ||
	   (NULL == CU_add_test(sharedSuite, "Select Pivot, Even", test_select_pivot_even)) ||
	   (NULL == CU_add_test(sharedSuite, "Swap Ints", test_swap_ints)) ||
	   (NULL == CU_add_test(sharedSuite, "Power Function, Even", test_power_even)) ||
	   (NULL == CU_add_test(sharedSuite, "Power Function, Odd", test_power_odd)) ||
	   (NULL == CU_add_test(sharedSuite, "Partition Array", test_partition_array)) ||
	   (NULL == CU_add_test(sharedSuite, "Array Union", test_array_union)) ||
	   (NULL == CU_add_test(sharedSuite, "Array Trace", test_trace_array)) ||
	   (NULL == CU_add_test(sharedSuite, "Subgroup Info", test_subgroup_info))

      )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
