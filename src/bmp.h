#include <stdbool.h>

typedef struct Image {
    unsigned int numColumns;
    unsigned int height;
	bool upsideDown;
    unsigned char palette[16];
    unsigned long int *pixels;
} Image;

bool loadBmp (Image *image, char* imageFilePath);
void freeImage (Image image);
