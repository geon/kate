#include <stdbool.h>

typedef struct Image {
    unsigned int numColumns;
    unsigned int height;
	bool upsideDown;
    unsigned char palette[16];
    unsigned long int *pixels;
} Image;

void freeImage (Image image);
