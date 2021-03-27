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


void initializeSpriteWithShiftedCopy (Sprite sprite, Sprite originalSprite, uint8_t shift) {
	uint16_t y, column;
	uint16_t numStrips;

	// The sprite needs to be one column wider if it is shifted, to account for the overflowing pixels.
	sprite->numColumns = originalSprite->numColumns + 1;
	sprite->height = originalSprite->height;
	numStrips = sprite->numColumns * sprite->height;

	initializeBitplaneStripVector(&sprite->bitPlaneStrips, numStrips);
	initializeUint8Vector(&sprite->mask, numStrips);
	memcpy(sprite->palette, originalSprite->palette, sizeof(sprite->palette));

	for (y = 0; y < originalSprite->height; ++y) {
		// TODO: Use a uint32_t to shift 3 strips at a time without spilling bits.
		uint16_t lastShiftedStripBuffer[4] = {0, 0, 0, 0};
		uint16_t lastShiftedMaskBuffer;
		uint16_t currentShiftedStripBuffer[4] = {0, 0, 0, 0};
		uint16_t currentShiftedMaskBuffer;
		uint8_t shiftedMask;
		BitplaneStrip shiftedStrip;

		for (column=0; column<originalSprite->numColumns; ++column) {
			uint16_t sourceStripIndex = column + y*originalSprite->numColumns;
			BitplaneStrip strip = originalSprite->bitPlaneStrips.values[sourceStripIndex];
			uint8_t mask = originalSprite->mask.values[sourceStripIndex];

			// Shift the current strip a number of pixels, within a 16 bit variable, so the overflowing bits aren't lost.
			{
				uint8_t plane;
				for (plane=0; plane<4; ++plane) {
					currentShiftedStripBuffer[plane] = strip.planes[plane] << (8 - shift);
				}
			}
			currentShiftedMaskBuffer = mask << (8 - shift);

			// Merge in the overflow from the last strip.
			{
				uint8_t plane;
				for (plane=0; plane<4; ++plane) {
					shiftedStrip.planes[plane] = (lastShiftedStripBuffer[plane] & 0xff) | (currentShiftedStripBuffer[plane] >> 8);
				}
			}
			shiftedMask = (lastShiftedMaskBuffer & 0xff) | (currentShiftedMaskBuffer >> 8);

			// Save the overflow to the next pass.
			{
				uint8_t plane;
				for (plane=0; plane<4; ++plane) {
					lastShiftedStripBuffer[plane] = currentShiftedStripBuffer[plane];
				}
			}
			lastShiftedMaskBuffer = currentShiftedMaskBuffer;

			bitplaneStripVectorPush(&sprite->bitPlaneStrips, shiftedStrip);
			uint8VectorPush(&sprite->mask, shiftedMask);
		}

		// Draw the last overflown bits.
		{
			uint8_t plane;
			for (plane=0; plane<4; ++plane) {
				shiftedStrip.planes[plane] = lastShiftedStripBuffer[plane] & 0xff;
			}
		}
		shiftedMask = lastShiftedMaskBuffer & 0xff;
		bitplaneStripVectorPush(&sprite->bitPlaneStrips, shiftedStrip);
		uint8VectorPush(&sprite->mask, shiftedMask);
	}
}

void freeSprite (Sprite sprite) {
	destroyBitplaneStripVector(&sprite->bitPlaneStrips);
	destroyUint8Vector(&sprite->mask);
	free(sprite);
}


uint8_t * getSpritePalette(Sprite sprite) {
	return sprite->palette;
}


BitplaneStrip mixStripsByMask (BitplaneStrip background, BitplaneStrip foreground, uint8_t mask) {
	BitplaneStrip result;

	uint8_t plane;
	for (plane=0; plane<4; ++plane) {
		result.planes[plane] = (background.planes[plane] & ~mask) | (foreground.planes[plane] & mask);
	}

	return result;
}


void spriteDraw(Sprite sprite, StripCoord pos, Map map, Buffer buffer, PositionAndStripVector stripBatch) {
	BitplaneStrip *strips = mapGetStrips(map);
	uint16_t y, column;
	for (y = 0; y < sprite->height; ++y) {
		uint16_t destinationStripIndex;
		StripCoord worldCoord;

		for (column=0; column<sprite->numColumns; ++column) {
			uint16_t sourceStripIndex = column + y*sprite->numColumns;
			BitplaneStrip strip = sprite->bitPlaneStrips.values[sourceStripIndex];
			uint8_t mask = sprite->mask.values[sourceStripIndex];

			worldCoord.column = pos.column + column;
			worldCoord.y = pos.y + y;
			destinationStripIndex = bufferMapWorldCoordToBufferIndex(
				buffer,
				worldCoord
			);

			if (mask) {
				BitplaneStrip backgroundStrip = strips[getMapStripAtWorldCoord(map, worldCoord)];
				PositionAndStrip posAndStrip;
				posAndStrip.pos=destinationStripIndex;
				posAndStrip.strip=mixStripsByMask(backgroundStrip, strip, mask);
				positionAndStripVectorPush(stripBatch, posAndStrip);
			}
		}
	}

	{
		StripCoord topLeft, bottomRight;
		topLeft.column = pos.column;
		topLeft.y = pos.y;
		bottomRight.column = topLeft.column + sprite->numColumns;
		bottomRight.y = topLeft.y + sprite->height;

		bufferMarkRectangleAsDirty(buffer, topLeft, bottomRight, map);
	}
}
