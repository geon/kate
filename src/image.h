#ifndef image_h
#define image_h

#include <stdbool.h>

typedef struct ImageStruct *Image;

Image makeImage ();
void freeImage (Image image);
unsigned char * getImagePalette(Image image);

#endif
