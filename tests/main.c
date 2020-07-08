#include "minunit.h"

#include "../src/vector.h"
declareVector(IntVector, int, intVector)
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


MU_TEST_SUITE(test_suite) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(test_vector);
}


int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
