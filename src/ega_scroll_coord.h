#ifndef ega_scroll_coord_h
#define ega_scroll_coord_h

#include <stdint.h>


typedef struct EgaScrollCoord {
	uint16_t column;
	unsigned char restX;
	uint16_t y;
} EgaScrollCoord;

#endif
