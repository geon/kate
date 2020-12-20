#ifndef image_struct_h
#define image_struct_h

#include <stdint.h>


typedef struct ImageStruct {
	unsigned int numColumns;
	unsigned int height;
	bool upsideDown;
	unsigned char palette[16];
	uint32_t *pixels;
	unsigned char *mask;
} ImageStruct;

#endif
