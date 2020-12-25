#include "sprite.h"
#include "sprite_struct.h"
#include "image.h"
#include "image_struct.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>


defineVector(SpriteVector, Sprite, spriteVector)


Sprite makeSprite (Image image) {
	Sprite sprite = malloc(sizeof(SpriteStruct));
	assert(sprite);

	sprite->numColumns = image->numColumns;
	sprite->height = image->height;

	memcpy(sprite->palette, image->palette, sizeof(sprite->palette));

	{
		uint16_t numBitplaneStrips = image->numColumns * image->height;
		initializeBitplaneStripVector(&sprite->bitPlaneStrips, numBitplaneStrips);
		initializeUint8Vector(&sprite->mask, numBitplaneStrips);

		{
			uint16_t i;
			for (i=0; i<numBitplaneStrips; ++i) {
				uint16_t column, y;
				column = i % image->numColumns;
				y = i / image->numColumns;
				if (image->upsideDown) {
					y = image->height - 1 - y;
				}
				bitplaneStripVectorPush(&sprite->bitPlaneStrips, makeBitplaneStrip(image->pixels[column + y*image->numColumns]));
				uint8VectorPush(&sprite->mask, image->mask[column + y*image->numColumns]);
			}
		}
	}

	return sprite;
}


void freeSprite (Sprite sprite) {
	destroyBitplaneStripVector(&sprite->bitPlaneStrips);
	destroyUint8Vector(&sprite->mask);
	free(sprite);
}


uint8_t * getSpritePalette(Sprite sprite) {
	return sprite->palette;
}
