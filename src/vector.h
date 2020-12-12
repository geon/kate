#ifndef vector_h
#define vector_h

#include <assert.h>

#define vectorForeach(methodPrefix, iterator, vector) for (iterator=methodPrefix##Begin(vector); iterator!=methodPrefix##End(vector); ++iterator)
#define vectorForeachIndex(methodPrefix, iterator, index, vector) for (iterator=methodPrefix##Begin(vector),index=0; iterator!=methodPrefix##End(vector); ++iterator,++index)


#define declareVector(VectorTypeName, TValue, methodPrefix) \
typedef struct VectorTypeName##Struct *VectorTypeName; \
VectorTypeName make##VectorTypeName(const long int capacity); \
void initialize##VectorTypeName(VectorTypeName vector, const long int capacity); \
void destroy##VectorTypeName(VectorTypeName vector); \
void free##VectorTypeName(VectorTypeName vector); \
void methodPrefix##Push(VectorTypeName vector, const TValue value); \
void methodPrefix##Clear(VectorTypeName vector); \
TValue* methodPrefix##Begin(VectorTypeName vector); \
TValue* methodPrefix##End(VectorTypeName vector); \
long int methodPrefix##Size(VectorTypeName vector); \


#define defineVectorStruct(VectorTypeName, TValue, methodPrefix) \
typedef struct VectorTypeName##Struct { \
	TValue *values; \
	long int size; \
	long int capacity; \
} VectorTypeName##Struct; \


#define defineVector(VectorTypeName, TValue, methodPrefix) \
VectorTypeName make##VectorTypeName(const long int capacity) { \
	VectorTypeName vector = malloc(sizeof(VectorTypeName)); \
	initialize##VectorTypeName(vector, capacity); \
	return vector; \
} \
void initialize##VectorTypeName(VectorTypeName vector, const long int capacity) { \
	vector->values = malloc(sizeof(TValue) * capacity); \
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
void methodPrefix##Push(VectorTypeName vector, const TValue value) { \
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
TValue* methodPrefix##Begin(VectorTypeName vector) { \
    return vector->values; \
} \
TValue* methodPrefix##End(VectorTypeName vector) { \
    return vector->values + vector->size; \
} \
long int methodPrefix##Size(VectorTypeName vector) { \
	return vector->size; \
} \


#endif
