#include "minunit.h"

#include "../src/vector.h"
#include "../src/grouped_table.h"


declareVector(IntVector, int, intVector)
defineVectorStruct(IntVector, int, intVector)
defineVector(IntVector, int, intVector)


declareGroupedTable(IntByIntTable, int, int, IntVector, intByIntTable)
declareVector(IntByIntTableRowsVector, IntByIntTableRow, intByIntTableRowsVector)
defineVectorStruct(IntByIntTableRowsVector, IntByIntTableRow, intByIntTableRowsVector)
defineVector(IntByIntTableRowsVector, IntByIntTableRow, intByIntTableRowsVector)
#define intsEqual(a, b) a == b
defineGroupedTable(IntByIntTable, int, int, IntVector, intVector, intByIntTable, intsEqual)


void test_setup(void) {
}


void test_teardown(void) {
}


MU_TEST(test_vector) {
	int capacity = 10;
	IntVector vector = makeIntVector(capacity);

	{
		int i;
		for (i=0; i<capacity; ++i) {
			intVectorPush(vector, i);
		}
	}

	{
		int *element;
		int i;
		vectorForeachIndex(intVector, element, i, vector) {
			// printf("%i => %i\n", i, *element);
			mu_assert_int_eq(i, *element);
		}
	}

	freeIntVector(vector);
}


MU_TEST(test_coods_grouped_by_strip) {
	int capacity = 10;
	IntByIntTable table = makeIntByIntTable(capacity);

	intByIntTableAdd(table, 1, 1);
	intByIntTableAdd(table, 1, 2);
	intByIntTableAdd(table, 1, 3);
	intByIntTableAdd(table, 2, 1);
	intByIntTableAdd(table, 2, 2);
	intByIntTableAdd(table, 2, 3);
	intByIntTableAdd(table, 2, 4);
	intByIntTableAdd(table, 2, 5);
	intByIntTableAdd(table, 3, 1);
	intByIntTableAdd(table, 4, 1);
	intByIntTableAdd(table, 5, 1);

	{
		IntByIntTableRow *row;
		int i;
		vectorForeachIndex(intByIntTable, row, i, table) {
			// printf("%i => %i\n", row->key, intVectorSize(row->values));
			if (i == 0) {
				mu_assert_int_eq(5, intVectorSize(row->values));
			} else if (i == 1) {
				mu_assert_int_eq(3, intVectorSize(row->values));
			} else {
				mu_assert_int_eq(1, intVectorSize(row->values));
			}
		}
	}

	freeIntByIntTable(table);
}


MU_TEST_SUITE(test_suite) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(test_vector);
	MU_RUN_TEST(test_coods_grouped_by_strip);
}


int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}