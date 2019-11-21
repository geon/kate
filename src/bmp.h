#ifndef bmp_h
#define bmp_h

#include "image.h"
#include <stdbool.h>

Image loadBmp(char* imageFilePath, bool firstColorIsTransparency, char **errorMessage);

#endif
