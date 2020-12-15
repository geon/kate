#ifndef grouped_table_h
#define grouped_table_h

#include <stdlib.h>
#include <assert.h>


#define declareGroupedTable(TableTypeName, TKey, TValue, TValueVector, methodPrefix) \
typedef struct TableTypeName##Row { \
	TKey key; \
	TValueVector values; \
} TableTypeName##Row; \
typedef struct TableTypeName##Struct *TableTypeName; \
TableTypeName make##TableTypeName(const long int capacity); \
void initialize##TableTypeName(TableTypeName table, const long int capacity); \
void destroy##TableTypeName(TableTypeName table); \
void free##TableTypeName(TableTypeName table); \
void methodPrefix##Add(TableTypeName table, const TKey key, const TValue value); \
void methodPrefix##Clear(TableTypeName table); \
TableTypeName##Row* methodPrefix##Begin(TableTypeName table); \
TableTypeName##Row* methodPrefix##End(TableTypeName table); \


#define defineGroupedTable(TableTypeName, TKey, TValue, TValueVector, valueVectorPrefix, methodPrefix, keysEqual) \
typedef struct TableTypeName##Struct { \
	TableTypeName##RowsVector rows; \
} TableTypeName##Struct; \
 \
 \
TableTypeName make##TableTypeName(const long int capacity) { \
	##TableTypeName table = malloc(sizeof(TableTypeName)); \
	assert(table); \
	initialize##TableTypeName(table, capacity); \
	return table; \
} \
 \
 \
void initialize##TableTypeName(TableTypeName table, const long int capacity) { \
	table->rows = make##TableTypeName##RowsVector(capacity); \
} \
 \
 \
void destroy##TableTypeName(TableTypeName table) { \
	free##TableTypeName##RowsVector(table->rows); \
} \
 \
 \
void free##TableTypeName(TableTypeName table) { \
	destroy##TableTypeName(table); \
	free(table); \
} \
 \
 \
void methodPrefix##Add(TableTypeName table, const TKey key, const TValue value) { \
	long int rowIndex; \
 \
	/* Scan for an existing key. */ \
	for (rowIndex = 0; rowIndex < methodPrefix##RowsVectorSize(table->rows); ++rowIndex) { \
		if (keysEqual(key, table->rows->values[rowIndex].key)) { \
			break; \
		} \
	} \
 \
	/* Check if the key exists. */ \
	if (rowIndex < methodPrefix##RowsVectorSize(table->rows)) { \
		/* The key exists. */ \
 \
		/* Use the row of the existing key. */ \
		valueVectorPrefix##Push(table->rows->values[rowIndex].values, value); \
 \
		/* Bubble the row up, so commonly used rows are quicker to find. */ \
		if (rowIndex > 0) { \
			##TableTypeName##Row tempRow = table->rows->values[rowIndex - 1]; \
			table->rows->values[rowIndex - 1] = table->rows->values[rowIndex]; \
			table->rows->values[rowIndex] = tempRow; \
		} \
	} else { \
		/* The key is missing. */ \
 \
		/* Add a row for the missing key. */ \
		##TableTypeName##Row row; \
		row.key = key; \
		row.values = make##TValueVector(64); \
		valueVectorPrefix##Push(row.values, value); \
		methodPrefix##RowsVectorPush(table->rows, row); \
	} \
 \
} \
 \
 \
void methodPrefix##Clear(TableTypeName table) { \
	long int rowIndex; \
	for (rowIndex = 0; rowIndex < methodPrefix##RowsVectorSize(table->rows); ++rowIndex) { \
		valueVectorPrefix##Clear(table->rows->values[rowIndex].values); \
	} \
} \
 \
 \
TableTypeName##Row* methodPrefix##Begin(TableTypeName table) { \
	return methodPrefix##RowsVectorBegin(table->rows); \
} \
 \
 \
TableTypeName##Row* methodPrefix##End(TableTypeName table) { \
	return methodPrefix##RowsVectorEnd(table->rows); \
} \


#endif
