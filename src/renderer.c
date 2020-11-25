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

#include <stdlib.h>
#include <math.h>
#include <string.h>

#define NUM_BUNNY_IMAGES 4


typedef struct RendererStruct {
	unsigned char palette[16];
	Buffer buffer;

	unsigned short int numSprites;
	Sprite *sprites;
} RendererStruct;


Renderer makeRenderer (char **errorMessage) {
	Renderer renderer = malloc(sizeof(RendererStruct));
	renderer->buffer = makeBuffer();

	renderer->numSprites = 0;
	// TODO: Make it dynamic.
	renderer->sprites = malloc(sizeof(Sprite) * 100);

	return renderer;
}


void freeRenderer (Renderer renderer) {
	unsigned int i;

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


void drawSprite(Renderer renderer, SpriteInstance *spriteInstance) {
	unsigned int y, column;
	for (y = 0; y < spriteInstance->sprite->height; ++y) {
		for (column=0; column<spriteInstance->sprite->numColumns; ++column) {
			unsigned int sourceStripIndex = column + y*spriteInstance->sprite->numColumns;
			BitplaneStrip strip = spriteInstance->sprite->bitPlaneStrips[sourceStripIndex];
			unsigned int posXColumn = spriteInstance->posX/8;
			unsigned int posXRest = spriteInstance->posX%8;
			unsigned char shiftMask;
			BitplaneStrip stripShiftedA, stripShiftedB;
			unsigned int destinationStripIndex;
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
			shiftMask = spriteInstance->sprite->mask[sourceStripIndex] >> posXRest;
			drawStrip(destinationStripIndex, stripShiftedA, shiftMask);
			bufferMarkDirtyBackgroundStrips(renderer->buffer, worldCoord);
			shiftMask = spriteInstance->sprite->mask[sourceStripIndex] << (8 - posXRest);
			++worldCoord.column;
			drawStrip(destinationStripIndex + 1, stripShiftedB, shiftMask);
			bufferMarkDirtyBackgroundStrips(renderer->buffer, worldCoord);
		}
	}
}


void renderSprites (Renderer renderer, unsigned int numSpriteInstances, SpriteInstance *spriteInstances) {
	unsigned int i;

	for (i=0; i<numSpriteInstances; ++i) {
		drawSprite(renderer, &spriteInstances[i]);
	}
}


void renderBackground (Renderer renderer, Map map) {
	unsigned long int i;
	unsigned long int numCoords = bufferGetDirtyBackgroundStripsNumCoords(renderer->buffer);
	StripCoord *worldCoords = bufferGetDirtyBackgroundStripsCoords(renderer->buffer);

	for (i=0; i<numCoords; ++i) {
		StripCoord worldCoord = worldCoords[i];
		unsigned short index = bufferMapWorldCoordToBufferIndex(renderer->buffer, worldCoord);

		drawStrip(
			index,
			getMapStripAtWorldCoord(map, worldCoord),
			0xFF
		);
	}

	bufferClearDirtyBackgroundStrips(renderer->buffer);
}


void rendererSetBufferOffset (Renderer renderer, PixelCoord scroll) {
	EgaScrollCoord bufferScroll = makeEgaScrollCoordFromPixelCoord(scroll);
	StripCoord stripScroll = makeStripCoordFromEgaScrollCoord(bufferScroll);
	unsigned short bufferIndex = bufferMapWorldCoordToBufferIndex(
		renderer->buffer,
		stripScroll
	);
	setBufferOffset(bufferIndex, bufferScroll.restX);
}


void rendererRender(Renderer renderer, unsigned int numSpriteInstances, SpriteInstance *spriteInstances, Map map, PixelCoord scroll) {
	switchBuffer(renderer->buffer);
	setBufferScroll(renderer->buffer, scroll);

	renderBackground(renderer, map);
	renderSprites(renderer, numSpriteInstances, spriteInstances);

	// Sets the start-address of the next frame.
	// The value won't be latched by the EGA card until the vertical retrace.
	// It is not possible to change the actual used address during a frame.
	rendererSetBufferOffset (renderer, scroll);

	// V-sync.
	waitForFrame();
}
