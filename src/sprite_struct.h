#ifndef sprite_struct_h
#define sprite_struct_h

#include <stdlib.h>
#include <stdint.h>

#include "platform/dos/bitplane_strip.h"
#include "uint8_vector.h"


typedef struct SpriteStruct {
	uint16_t numColumns;
	uint16_t height;
	uint8_t palette[16];
	BitplaneStripVectorStruct bitPlaneStrips;
	Uint8VectorStruct mask;
} SpriteStruct;

#endif
