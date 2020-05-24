#include "renderer.h"
#include "bmp.h"
#include "platform/dos/ega.h"
#include "platform/dos/bitplane_strip.h"
#include "buffer.h"
#include "dirty_background_strips.h"
#include "sprite.h"
#include "sprite_instance.h"
#include "sprite_struct.h"
#include "map.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#define NUM_BUNNY_IMAGES 4


typedef struct RendererStruct {
	unsigned char palette[16];
	Buffer buffer;

	unsigned short int numSprites;
	Sprite *sprites;

	DirtyBackgroundStrips dirtyBackgroundStrips;
} RendererStruct;


Renderer makeRenderer (char **errorMessage) {
	Renderer renderer = malloc(sizeof(RendererStruct));
	renderer->buffer = makeBuffer();

	renderer->numSprites = 0;
	// TODO: Make it dynamic.
	renderer->sprites = malloc(sizeof(Sprite) * 100);

	renderer->dirtyBackgroundStrips = makeDirtyBackgroundStrips();

	return renderer;
}


void freeRenderer (Renderer renderer) {
	unsigned int i;

	freeDirtyBackgroundStrips(renderer->dirtyBackgroundStrips);
	for (i=0; i<renderer->numSprites; ++i) {
		freeSprite(renderer->sprites[i]);
	}
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


void drawSprite(Renderer renderer, SpriteInstance *spriteInstance, bool alternateBuffer) {
	unsigned int y, column;
	for (y = 0; y < spriteInstance->sprite->height; ++y) {
		for (column=0; column<spriteInstance->sprite->numColumns; ++column) {
			unsigned int sourceStripIndex = column + y*spriteInstance->sprite->numColumns;
			BitplaneStrip strip = spriteInstance->sprite->bitPlaneStrips[sourceStripIndex];
			unsigned int posXColumn = spriteInstance->posX/8;
			unsigned int posXRest = spriteInstance->posX%8;
			unsigned int destinationStripIndex = bufferIndexStart(posXColumn + column, spriteInstance->posY + y, alternateBuffer);
			unsigned char shiftMask;
			BitplaneStrip stripShiftedA, stripShiftedB;

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
			markDirtyBackgroundStrips(renderer->dirtyBackgroundStrips, destinationStripIndex);
			shiftMask = spriteInstance->sprite->mask[sourceStripIndex] << (8 - posXRest);
			drawStrip(destinationStripIndex + 1, stripShiftedB, shiftMask);
			markDirtyBackgroundStrips(renderer->dirtyBackgroundStrips, destinationStripIndex+1);
		}
	}
}


void renderSprites (Renderer renderer, unsigned int numSpriteInstances, SpriteInstance *spriteInstances, bool alternateBuffer) {
	unsigned int i;

	for (i=0; i<numSpriteInstances; ++i) {
		drawSprite(renderer, &spriteInstances[i], alternateBuffer);
	}
}


void renderBackground (Renderer renderer, Map map) {
	unsigned long int i;
	unsigned long int numIndices = getDirtyBackgroundStripsNumIndices(renderer->dirtyBackgroundStrips);
	unsigned short int *indices = getDirtyBackgroundStripsIndices(renderer->dirtyBackgroundStrips);

	for (i=0; i<numIndices; ++i) {
		StripCoord bufferCoord;
		StripCoord worldCoord;
		bufferCoord = bufferMapBufferIndexToBufferCoord(&renderer->buffer, indices[i]);
		worldCoord = bufferMapBufferCoordToWorldCoord(&renderer->buffer, bufferCoord);

		drawStrip(
			indices[i],
			getMapStripAtWorldCoord(map, worldCoord),
			0xFF
		);
	}

	clearDirtyBackgroundStrips(renderer->dirtyBackgroundStrips);
}


void rendererRender(Renderer renderer, unsigned int numSpriteInstances, SpriteInstance *spriteInstances, Map map) {
	updateBuffer(&renderer->buffer);

	renderBackground(renderer, map);
	renderSprites(renderer, numSpriteInstances, spriteInstances, renderer->buffer.alternateBuffer);
	// Sets the start-address of the next frame.
	// The value won't be latched by the EGA card until the vertical retrace.
	// It is not possible to change the actual used address during a frame.
	// setScroll(renderer->buffer, scrollX, scrollY);
	// V-sync.
	waitForFrame();
}
