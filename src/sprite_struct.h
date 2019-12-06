#ifndef sprite_struct_h
#define sprite_struct_h

#include "platform/dos/bitplane_strip.h"

typedef struct SpriteStruct {
	unsigned int numColumns;
	unsigned int height;
	unsigned char palette[16];
	BitplaneStrip *bitPlaneStrips;
	unsigned char *mask;
} SpriteStruct;

#endif
