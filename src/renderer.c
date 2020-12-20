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

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#define NUM_BUNNY_IMAGES 4


typedef struct RendererStruct {
	unsigned char palette[16];
	Buffer buffer;

	uint16_t numSprites;
	Sprite *sprites;
} RendererStruct;


Renderer makeRenderer (char **errorMessage) {
	Renderer renderer = malloc(sizeof(RendererStruct));
	assert(renderer);
	renderer->buffer = makeBuffer();

	renderer->numSprites = 0;
	// TODO: Make it dynamic.
	renderer->sprites = malloc(sizeof(Sprite) * 100);
	assert(renderer->sprites);

	return renderer;
}


void freeRenderer (Renderer renderer) {
	uint16_t i;

	for (i=0; i<renderer->numSprites; ++i) {
		freeSprite(renderer->sprites[i]);
	}
	freeBuffer(renderer->buffer);
	free(renderer);
}


unsigned char * getRendererPalette(Renderer renderer) {
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

	// TODO: Make sure it is the same palette, or remap colors.
	renderer->sprites[renderer->numSprites++] = sprite;
	memcpy(renderer->palette, sprite->palette, 16);

	return sprite;
}


void drawSprite(Renderer renderer, SpriteInstance *spriteInstance, Map map) {
	uint16_t y, column;
	for (y = 0; y < spriteInstance->sprite->height; ++y) {
		for (column=0; column<spriteInstance->sprite->numColumns; ++column) {
			uint16_t sourceStripIndex = column + y*spriteInstance->sprite->numColumns;
			BitplaneStrip strip = spriteInstance->sprite->bitPlaneStrips.values[sourceStripIndex];
			uint16_t posXColumn = spriteInstance->posX/8;
			uint16_t posXRest = spriteInstance->posX%8;
			unsigned char shiftMask;
			BitplaneStrip stripShiftedA, stripShiftedB;
			uint16_t destinationStripIndex;
			StripCoord worldCoord;

			worldCoord.column = posXColumn + column;
			worldCoord.y = spriteInstance->posY + y;
			destinationStripIndex = bufferMapWorldCoordToBufferIndex(
				renderer->buffer,
				worldCoord
			);

			stripShiftedA.planes[0] = strip.planes[0] >> posXRest;
			stripShiftedA.planes[1] = strip.planes[1] >> posXRest;
			stripShiftedA.planes[2] = strip.planes[2] >> posXRest;
			stripShiftedA.planes[3] = strip.planes[3] >> posXRest;
			stripShiftedB.planes[0] = strip.planes[0] << (8 - posXRest);
			stripShiftedB.planes[1] = strip.planes[1] << (8 - posXRest);
			stripShiftedB.planes[2] = strip.planes[2] << (8 - posXRest);
			stripShiftedB.planes[3] = strip.planes[3] << (8 - posXRest);
			// TODO: Combine the adjecent strips, to avoid double writes.
			shiftMask = spriteInstance->sprite->mask.values[sourceStripIndex] >> posXRest;
			if (shiftMask) {
				drawStrip(destinationStripIndex, stripShiftedA, shiftMask);
				bufferMarkDirtyBackgroundStrips(renderer->buffer, worldCoord, getMapStripAtWorldCoord(map, worldCoord));
			}
			shiftMask = spriteInstance->sprite->mask.values[sourceStripIndex] << (8 - posXRest);
			++worldCoord.column;
			if (shiftMask) {
				drawStrip(destinationStripIndex + 1, stripShiftedB, shiftMask);
				bufferMarkDirtyBackgroundStrips(renderer->buffer, worldCoord,  getMapStripAtWorldCoord(map, worldCoord));
			}
		}
	}
}


void renderSprites (Renderer renderer, uint16_t numSpriteInstances, SpriteInstance *spriteInstances, Map map) {
	uint16_t i;

	for (i=0; i<numSpriteInstances; ++i) {
		drawSprite(renderer, &spriteInstances[i], map);
	}
}


void renderBackground (Renderer renderer) {
	IndicesByStripTableRow *row;
	vectorForeach (bufferDirtyBackgroundStripsBegin(renderer->buffer), bufferDirtyBackgroundStripsEnd(renderer->buffer), row) {
		// TODO: Replace with single draw call.
		StripCoord *stripCoord;
		vectorForeach (stripCoordVectorBegin(row->values), stripCoordVectorEnd(row->values), stripCoord) {
			unsigned short bufferIndex = bufferMapWorldCoordToBufferIndex(
				renderer->buffer,
				*stripCoord
			);
			drawStrip(
				bufferIndex,
				row->key,
				0xFF
			);
		}
	}

	bufferClearDirtyBackgroundStrips(renderer->buffer);
}



void rendererRender(Renderer renderer, uint16_t numSpriteInstances, SpriteInstance *spriteInstances, Map map, PixelCoord scroll) {
	// Sets the start-address of the buffer.
	// The value won't be latched by the EGA card until the vertical retrace.
	// It is not possible to change the actual used address during a frame.
	switchBuffer(renderer->buffer, scroll, map);

	renderBackground(renderer);
	renderSprites(renderer, numSpriteInstances, spriteInstances, map);

	// V-sync.
	waitForFrame();
}
