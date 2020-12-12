#include "sprite.h"
#include "sprite_struct.h"
#include "image.h"
#include "image_struct.h"
#include <stdlib.h>
#include <string.h>


Sprite makeSprite (Image image) {
	unsigned short int numBitplaneStrips = image->numColumns * image->height;
	unsigned int i;

	Sprite sprite = malloc(sizeof(SpriteStruct));

	sprite->numColumns = image->numColumns;
	sprite->height = image->height;

	memcpy(sprite->palette, image->palette, sizeof(sprite->palette));

	sprite->bitPlaneStrips = malloc(sizeof(BitplaneStrip) * numBitplaneStrips);
	sprite->mask = malloc(sizeof(unsigned char) * numBitplaneStrips);
	for (i=0; i<numBitplaneStrips; ++i) {
		unsigned short int column, y;
		column = i % image->numColumns;
		y = i / image->numColumns;
		if (image->upsideDown) {
			y = image->height - 1 - y;
		}
		sprite->bitPlaneStrips[i] = makeBitplaneStrip(image->pixels[column + y*image->numColumns]);
		sprite->mask[i] = image->mask[column + y*image->numColumns];
	}

	return sprite;
}


void freeSprite (Sprite sprite) {
	free(sprite->bitPlaneStrips);
	free(sprite->mask);
	free(sprite);
}


unsigned char * getSpritePalette(Sprite sprite) {
	return sprite->palette;
}
