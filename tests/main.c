#include "minunit.h"

#include "../src/vector.h"


declareVector(IntVector, int16_t, intVector)
defineVectorStruct(IntVector, int16_t, intVector)
defineVector(IntVector, int16_t, intVector)


declareGroupedTable(IntByIntTable, int16_t, int16_t, IntVector, intByIntTable)
declareVector(IntByIntTableRowsVector, IntByIntTableRow, intByIntTableRowsVector)
defineVectorStruct(IntByIntTableRowsVector, IntByIntTableRow, intByIntTableRowsVector)
defineVector(IntByIntTableRowsVector, IntByIntTableRow, intByIntTableRowsVector)
#define intsEqual(a, b) a == b
defineGroupedTable(IntByIntTable, int16_t, int16_t, IntVector, intVector, intByIntTable, intsEqual)


void test_setup(void) {
}


void test_teardown(void) {
}


MU_TEST(test_vector) {
	int16_t capacity = 10;
	IntVector vector = makeIntVector(capacity);

	{
		int16_t i;
		for (i=0; i<capacity; ++i) {
			intVectorPush(vector, i);
		}
	}

	{
		int16_t *element;
		int16_t i;
		vectorForeachIndex(intVectorBegin(vector), intVectorEnd(vector), element, i) {
			// printf("%i => %i\n", i, *element);
			mu_assert_int_eq(i, *element);
		}
	}

	freeIntVector(vector);
}


MU_TEST_SUITE(test_suite) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(test_vector);
	MU_RUN_TEST(test_coods_grouped_by_strip);
}


int16_t main(int16_t argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
