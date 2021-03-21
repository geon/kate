#include "renderer.h"
#include "bmp.h"
#include "platform/dos/ega.h"
#include "platform/dos/bitplane_strip.h"
#include "buffer.h"
#include "sprite.h"
#include "sprite_instance.h"
#include "sprite_struct.h"
#include "map.h"
#include "strip_coord.h"
#include "pixel_coord.h"
#include "ega_scroll_coord.h"
#include "coord_conversion.h"
#include "vector.h"
#include "uint16_vector.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#define NUM_BUNNY_IMAGES 4


typedef struct RendererStruct {
	uint8_t palette[16];
	Buffer buffer;

	SpriteVectorStruct sprites;
} RendererStruct;


Renderer makeRenderer (char **errorMessage) {
	Renderer renderer = malloc(sizeof(RendererStruct));
	assert(renderer);
	renderer->buffer = makeBuffer();

	initializeSpriteVector(&renderer->sprites, 4);

	return renderer;
}


void freeRenderer (Renderer renderer) {
	Sprite *sprite;

	vectorForeach (spriteVectorBegin(&renderer->sprites), spriteVectorEnd(&renderer->sprites), sprite) {
		freeSprite(*sprite);
	}
	destroySpriteVector(&renderer->sprites);
	freeBuffer(renderer->buffer);
	free(renderer);
}


uint8_t * getRendererPalette(Renderer renderer) {
	return renderer->palette;
}


Sprite rendererLoadSprite (Renderer renderer, char *imagePath, char **errorMessage) {
	Image image;
	Sprite sprite;

	image = loadBmp(imagePath, true, errorMessage);
	if (!image) {
		return NULL;
	}

	sprite = makeSprite(image);
	if (!sprite) {
		return NULL;
	}

	freeImage(image);

	spriteVectorPush(&renderer->sprites, sprite);

	// TODO: Make sure it is the same palette, or remap colors.
	memcpy(renderer->palette, sprite->palette, 16);

	return sprite;
}


void drawSprite(Renderer renderer, Sprite sprite, PixelCoord pos, Map map) {
	uint16_t y, column;
	for (y = 0; y < sprite->height; ++y) {
		// TODO: Use a uint32_t to shift 3 strips at a time without spilling bits.
		uint16_t lastShiftedStripBuffer[4] = {0, 0, 0, 0};
		uint16_t lastShiftedMaskBuffer;
		uint16_t currentShiftedStripBuffer[4] = {0, 0, 0, 0};
		uint16_t currentShiftedMaskBuffer;
		uint8_t shiftedMask;
		BitplaneStrip shiftedStrip;
		uint16_t destinationStripIndex;

		for (column=0; column<sprite->numColumns; ++column) {
			uint16_t sourceStripIndex = column + y*sprite->numColumns;
			BitplaneStrip strip = sprite->bitPlaneStrips.values[sourceStripIndex];
			uint8_t mask = sprite->mask.values[sourceStripIndex];
			uint16_t posXColumn = pos.x/8;
			uint16_t posXRest = pos.x%8;
			StripCoord worldCoord;

			worldCoord.column = posXColumn + column;
			worldCoord.y = pos.y + y;
			destinationStripIndex = bufferMapWorldCoordToBufferIndex(
				renderer->buffer,
				worldCoord
			);




			// Shift the current strip a number of pixels, within a 16 bit variable, so the overflowing bits aren't lost.
			{
				uint8_t plane;
				for (plane=0; plane<4; ++plane) {
					currentShiftedStripBuffer[plane] = strip.planes[plane] << (8 - posXRest);
				}
			}
			currentShiftedMaskBuffer = mask << (8 - posXRest);

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

			if (shiftedMask) {
				drawStrip(destinationStripIndex, shiftedStrip, shiftedMask);
			}
		}

		// Draw the last overflown bits.
		{
			uint8_t plane;
			for (plane=0; plane<4; ++plane) {
				shiftedStrip.planes[plane] = lastShiftedStripBuffer[plane] & 0xff;
			}
		}
		shiftedMask = lastShiftedMaskBuffer & 0xff;
		if (shiftedMask) {
			drawStrip(destinationStripIndex+1, shiftedStrip, shiftedMask);
		}
	}

	{
		StripCoord topLeft, bottomRight;
		topLeft.column = pos.x/8;
		topLeft.y = pos.y;
		bottomRight.column = topLeft.column + sprite->numColumns;
		bottomRight.y = topLeft.y + sprite->height;

		bufferMarkRectangleAsDirty(renderer->buffer, topLeft, bottomRight, map);
	}
}


void renderSprites (Renderer renderer, SpriteInstanceVector spriteInstances, Map map) {
	SpriteInstance *spriteInstance;

	vectorForeach (spriteInstanceVectorBegin(spriteInstances), spriteInstanceVectorEnd(spriteInstances), spriteInstance) {
		drawSprite(renderer, spriteInstance->sprite, spriteInstance->pos, map);
	}
}


void renderBackground (Renderer renderer, Map map) {
	BitplaneStrip *strips = mapGetStrips(map);
	Uint16Vector dirtyBufferIndicesByStripIndex = bufferGetDirtyBufferIndicesByStripIndex(renderer->buffer);

	uint16_t *stripIndex;
	vectorForeach (bufferDirtyBackgroundStripsBegin(renderer->buffer), bufferDirtyBackgroundStripsEnd(renderer->buffer), stripIndex) {
		Uint16Vector dirtyBufferIndices = &dirtyBufferIndicesByStripIndex[*stripIndex];

		drawStrips(
			uint16VectorBegin(dirtyBufferIndices),
			uint16VectorEnd(dirtyBufferIndices),
			strips[*stripIndex]
		);
	}

	bufferClearDirtyBackgroundStrips(renderer->buffer);
}



void rendererRender(Renderer renderer, SpriteInstanceVector spriteInstances, Map map, PixelCoord scrollOfNextFrame) {
	// V-sync.
	waitForFrame();

	renderBackground(renderer, map);
	renderSprites(renderer, spriteInstances, map);

	// Sets the start-address of the buffer.
	// The value won't be latched by the EGA card until the vertical retrace.
	// It is not possible to change the actual used address during a frame.
	switchBuffer(renderer->buffer, scrollOfNextFrame, map);
}
