#ifndef sprite_struct_h
#define sprite_struct_h

#include <stdlib.h>
#include <stdint.h>

#include "platform/dos/bitplane_strip.h"
#include "vector.h"


typedef unsigned char StripMask;
declareVector(StripMaskVector, StripMask, stripMaskVector)
defineVectorStruct(StripMaskVector, StripMask, stripMaskVector)
defineVector(StripMaskVector, StripMask, stripMaskVector)

typedef struct SpriteStruct {
	uint16_t numColumns;
	uint16_t height;
	unsigned char palette[16];
	BitplaneStripVectorStruct bitPlaneStrips;
	StripMaskVectorStruct mask;
} SpriteStruct;

#endif
