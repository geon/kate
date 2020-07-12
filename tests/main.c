#include "minunit.h"

#include "../src/vector.h"
#include "../src/coords_grouped_by_strip.h"
#include "../src/platform/dos/bitplane_strip.h"


declareVector(IntVector, int, intVector)
defineVectorStruct(IntVector, int, intVector)
defineVector(IntVector, int, intVector)


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
		vectorForeachIndex(int, intVector, element, i, vector) {
			// printf("%i => %i\n", i, *element);
			mu_assert_int_eq(i, *element);
		}
	}

	freeIntVector(vector);
}


MU_TEST(test_coods_grouped_by_strip) {
	int capacity = 10;
	CoordsByStripTable table = makeCoordsByStripTable(capacity);

	coordsByStripTableAdd(table, makeBitplaneStrip(1), (StripCoord) {.column=1, .y=0});
	coordsByStripTableAdd(table, makeBitplaneStrip(1), (StripCoord) {.column=2, .y=0});
	coordsByStripTableAdd(table, makeBitplaneStrip(1), (StripCoord) {.column=3, .y=0});
	coordsByStripTableAdd(table, makeBitplaneStrip(2), (StripCoord) {.column=1, .y=0});
	coordsByStripTableAdd(table, makeBitplaneStrip(2), (StripCoord) {.column=2, .y=0});
	coordsByStripTableAdd(table, makeBitplaneStrip(2), (StripCoord) {.column=3, .y=0});
	coordsByStripTableAdd(table, makeBitplaneStrip(2), (StripCoord) {.column=4, .y=0});
	coordsByStripTableAdd(table, makeBitplaneStrip(2), (StripCoord) {.column=5, .y=0});
	coordsByStripTableAdd(table, makeBitplaneStrip(3), (StripCoord) {.column=1, .y=0});
	coordsByStripTableAdd(table, makeBitplaneStrip(4), (StripCoord) {.column=1, .y=0});
	coordsByStripTableAdd(table, makeBitplaneStrip(5), (StripCoord) {.column=1, .y=0});

	{
		CoordsByStripTableRow *row;
		vectorForeach(BitplaneStrip, coordsByStripTable, row, table) {
			printf("%i => %i\n", bitplaneStripAsInt(row->key), stripCoordVectorSize(row->values));
		}
	}

	freeCoordsByStripTable(table);
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
