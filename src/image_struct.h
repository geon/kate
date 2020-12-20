#ifndef image_struct_h
#define image_struct_h

#include <stdint.h>


typedef struct ImageStruct {
	uint16_t numColumns;
	uint16_t height;
	bool upsideDown;
	unsigned char palette[16];
	uint32_t *pixels;
	unsigned char *mask;
} ImageStruct;

#endif
