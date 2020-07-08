#ifndef vector_h
#define vector_h

#include <assert.h>

#define vectorForeach(ValueTypeName, methodPrefix, iterator, vector) for (iterator=methodPrefix##Beginvector(); iterator!=methodPrefix##End(vector); ++iterator)
#define vectorForeachIndex(ValueTypeName, methodPrefix, iterator, index, vector) for (iterator=methodPrefix##Begin(vector),index=0; iterator!=methodPrefix##End(vector); ++iterator,++index)


#define declareVector(VectorTypeName, ValueTypeName, methodPrefix) \
typedef struct VectorTypeName##Struct *VectorTypeName; \
VectorTypeName make##VectorTypeName(const int capacity); \
void initialize##VectorTypeName(VectorTypeName vector, const int capacity); \
void destroy##VectorTypeName(VectorTypeName vector); \
void free##VectorTypeName(VectorTypeName vector); \
void methodPrefix##Push(VectorTypeName vector, const ValueTypeName value); \
void methodPrefix##Clear(VectorTypeName vector); \
ValueTypeName* methodPrefix##Begin(VectorTypeName vector); \
ValueTypeName* methodPrefix##End(VectorTypeName vector); \


#define defineVector(VectorTypeName, ValueTypeName, methodPrefix) \
typedef struct VectorTypeName##Struct { \
	ValueTypeName *values; \
	int size; \
	int	capacity; \
} VectorTypeName##Struct; \
VectorTypeName make##VectorTypeName(const int capacity) { \
    VectorTypeName vector = malloc(sizeof(VectorTypeName)); \
    initialize##VectorTypeName(vector, capacity); \
	return vector; \
} \
void initialize##VectorTypeName(VectorTypeName vector, const int capacity) { \
	vector->values = malloc(sizeof(ValueTypeName) * capacity); \
    vector->size = 0; \
    vector->capacity = capacity; \
} \
void destroy##VectorTypeName(VectorTypeName vector) { \
	free(vector->values); \
	vector->size = 0; \
	vector->capacity = 0; \
} \
void free##VectorTypeName(VectorTypeName vector) { \
    destroy##VectorTypeName(vector); \
	free(vector); \
} \
void methodPrefix##Push(VectorTypeName vector, const ValueTypeName value) { \
	if (vector->capacity > vector->size) { \
		vector->values[vector->size] = value; \
		++vector->size; \
	} else { \
		/* TODO: Change this to grow the vector dynamically. */ \
		assert(0); \
	} \
} \
void methodPrefix##Clear(VectorTypeName vector) { \
	vector->size = 0; \
} \
ValueTypeName* methodPrefix##Begin(VectorTypeName vector) { \
    return vector->values; \
} \
ValueTypeName* methodPrefix##End(VectorTypeName vector) { \
    return vector->values + vector->size; \
} \


#endif
