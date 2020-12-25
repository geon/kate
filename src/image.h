#ifndef image_h
#define image_h

#include <stdbool.h>
#include <stdint.h>

typedef struct ImageStruct *Image;

Image makeImage ();
void freeImage (Image image);
uint8_t * getImagePalette(Image image);

#endif
