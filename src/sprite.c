#include "sprite.h"
#include "sprite_struct.h"
#include "image.h"
#include "image_struct.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>


Sprite makeSprite (Image image) {
	Sprite sprite = malloc(sizeof(SpriteStruct));
	assert(sprite);

	sprite->numColumns = image->numColumns;
	sprite->height = image->height;

	memcpy(sprite->palette, image->palette, sizeof(sprite->palette));

	{
		unsigned short int numBitplaneStrips = image->numColumns * image->height;
		initializeBitplaneStripVector(&sprite->bitPlaneStrips, numBitplaneStrips);
		initializeStripMaskVector(&sprite->mask, numBitplaneStrips);

		{
			unsigned int i;
			for (i=0; i<numBitplaneStrips; ++i) {
				unsigned short int column, y;
				column = i % image->numColumns;
				y = i / image->numColumns;
				if (image->upsideDown) {
					y = image->height - 1 - y;
				}
				bitplaneStripVectorPush(&sprite->bitPlaneStrips, makeBitplaneStrip(image->pixels[column + y*image->numColumns]));
				stripMaskVectorPush(&sprite->mask, image->mask[column + y*image->numColumns]);
			}
		}
	}

	return sprite;
}


void freeSprite (Sprite sprite) {
	destroyBitplaneStripVector(&sprite->bitPlaneStrips);
	destroyStripMaskVector(&sprite->mask);
	free(sprite);
}


unsigned char * getSpritePalette(Sprite sprite) {
	return sprite->palette;
}
