#ifndef image_struct_h
#define image_struct_h

typedef struct ImageStruct {
	unsigned int numColumns;
	unsigned int height;
	bool upsideDown;
	unsigned char palette[16];
	unsigned long int *pixels;
	unsigned char *mask;
} ImageStruct;

#endif
