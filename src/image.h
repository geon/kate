#ifndef image_h
#define image_h

#include <stdbool.h>

typedef struct Image {
    unsigned int numColumns;
    unsigned int height;
	bool upsideDown;
    unsigned char palette[16];
    unsigned long int *pixels;
    unsigned char *mask;
} Image;

void freeImage (Image image);
void drawImage(Image image, unsigned int posX, unsigned int posY);

#endif