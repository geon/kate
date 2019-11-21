#ifndef image_h
#define image_h

#include <stdbool.h>

typedef struct ImageStruct *Image;

Image makeImage ();
void freeImage (Image image);
void drawImage(Image image, unsigned int posX, unsigned int posY);
unsigned char * getImagePalette(Image image);
unsigned int getImageNumColumns(Image image);

#endif
