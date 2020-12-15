#ifndef sprite_struct_h
#define sprite_struct_h

#include <stdlib.h>

#include "platform/dos/bitplane_strip.h"
#include "vector.h"


declareVector(BitplaneStripVector, BitplaneStrip, bitplaneStripVector)
defineVectorStruct(BitplaneStripVector, BitplaneStrip, bitplaneStripVector)
defineVector(BitplaneStripVector, BitplaneStrip, bitplaneStripVector)

typedef unsigned char StripMask;
declareVector(StripMaskVector, StripMask, stripMaskVector)
defineVectorStruct(StripMaskVector, StripMask, stripMaskVector)
defineVector(StripMaskVector, StripMask, stripMaskVector)

typedef struct SpriteStruct {
	unsigned int numColumns;
	unsigned int height;
	unsigned char palette[16];
	BitplaneStripVector bitPlaneStrips;
	StripMaskVector mask;
} SpriteStruct;

#endif
