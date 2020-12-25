#ifndef sprite_struct_h
#define sprite_struct_h

#include <stdlib.h>
#include <stdint.h>

#include "platform/dos/bitplane_strip.h"
#include "vector.h"


declareVector(Uint8Vector, uint8_t, uint8Vector)
defineVectorStruct(Uint8Vector, uint8_t, uint8Vector)
defineVector(Uint8Vector, uint8_t, uint8Vector)

typedef struct SpriteStruct {
	uint16_t numColumns;
	uint16_t height;
	uint8_t palette[16];
	BitplaneStripVectorStruct bitPlaneStrips;
	Uint8VectorStruct mask;
} SpriteStruct;

#endif
