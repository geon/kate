#ifndef vector_h
#define vector_h

#include <assert.h>

#define vectorForeach(begin, end, iterator) for (iterator=begin; iterator!=end; ++iterator)
#define vectorForeachIndex(begin, end, iterator, index) for (iterator=begin,index=0; iterator!=end; ++iterator,++index)


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
	assert(vector); \
	initialize##VectorTypeName(vector, capacity); \
	return vector; \
} \
void initialize##VectorTypeName(VectorTypeName vector, const long int capacity) { \
	vector->values = malloc(sizeof(TValue) * capacity); \
	assert(vector->values); \
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
	if (vector->capacity <= vector->size) { \
		vector->capacity *= 2; \
		vector->values = realloc(vector->values, sizeof(TValue) * vector->capacity); \
	} \
	vector->values[vector->size] = value; \
	++vector->size; \
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
