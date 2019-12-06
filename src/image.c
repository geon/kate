#include "image.h"
#include "image_struct.h"
#include "platform/dos/ega.h"
#include "platform/dos/bitplane_strip.h"
#include <stdlib.h>


Image makeImage () {
	Image image = malloc(sizeof(ImageStruct));

	// Empty image.
	image->numColumns = 0;
	image->height = 0;
	image->upsideDown = false;
	image->pixels = NULL;
	image->mask = NULL;

	return image;
}


void freeImage (Image image) {
	free(image->pixels);
	free(image->mask);
	free(image);
}


unsigned char * getImagePalette(Image image) {
	return image->palette;
}
