#include "image.h"
#include <stdlib.h>

void freeImage (Image image) {
	free(image.pixels);
}
