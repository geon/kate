#ifndef strip_coord_h
#define strip_coord_h

#include <stdint.h>
#include "vector.h"


typedef struct StripCoord {
	uint16_t column;
	uint16_t y;
} StripCoord;


declareVector(StripCoordVector, StripCoord, stripCoordVector)
defineVectorStruct(StripCoordVector, StripCoord, stripCoordVector)

#endif
