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
#include "indices_grouped_by_strip.h"

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


// TODO: This can't be here.
typedef struct BufferStruct {
	EgaScrollCoord scroll;
	bool alternateBuffer;
} BufferStruct;

void drawSprite(Renderer renderer, SpriteInstance *spriteInstance, Map map) {
	uint16_t y, column;
	for (y = 0; y < spriteInstance->sprite->height; ++y) {
		for (column=0; column<spriteInstance->sprite->numColumns; ++column) {
			uint16_t sourceStripIndex = column + y*spriteInstance->sprite->numColumns;
			BitplaneStrip strip = spriteInstance->sprite->bitPlaneStrips.values[sourceStripIndex];
			uint16_t posXColumn = spriteInstance->posX/8;
			uint16_t posXRest = spriteInstance->posX%8;
			uint8_t shiftMask;
			BitplaneStrip stripShiftedA, stripShiftedB;
			uint16_t destinationStripIndex;
			StripCoord worldCoord;

			worldCoord.column = posXColumn + column;
			worldCoord.y = spriteInstance->posY + y;
			destinationStripIndex = bufferMapWorldCoordToBufferIndex(
				worldCoord,
				renderer->buffer->scroll,
				renderer->buffer->alternateBuffer
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
			}
			shiftMask = spriteInstance->sprite->mask.values[sourceStripIndex] << (8 - posXRest);
			++worldCoord.column;
			if (shiftMask) {
				drawStrip(destinationStripIndex + 1, stripShiftedB, shiftMask);
			}
		}
	}

	{
		StripCoord topLeft, bottomRight;
		topLeft.column = spriteInstance->posX/8;
		topLeft.y = spriteInstance->posY;
		bottomRight.column = topLeft.column + spriteInstance->sprite->numColumns;
		bottomRight.y = topLeft.y + spriteInstance->sprite->height;

		bufferMarkRectangleAsDirty(renderer->buffer, topLeft, bottomRight, map);
	}
}


void renderSprites (Renderer renderer, SpriteInstanceVector spriteInstances, Map map) {
	SpriteInstance *spriteInstance;

	vectorForeach (spriteInstanceVectorBegin(spriteInstances), spriteInstanceVectorEnd(spriteInstances), spriteInstance) {
		drawSprite(renderer, spriteInstance, map);
	}
}


void renderBackground (Renderer renderer, Map map) {
	BitplaneStrip *strips = mapGetStrips(map);
	IndicesByStripTableRow *row;
	vectorForeach (bufferDirtyBackgroundStripsBegin(renderer->buffer), bufferDirtyBackgroundStripsEnd(renderer->buffer), row) {
		drawStrips(
			uint16VectorBegin(row->values),
			uint16VectorEnd(row->values),
			strips[row->key]
		);
	}

	bufferClearDirtyBackgroundStrips(renderer->buffer);
}



void rendererRender(Renderer renderer, SpriteInstanceVector spriteInstances, Map map, PixelCoord scroll) {
	// Sets the start-address of the buffer.
	// The value won't be latched by the EGA card until the vertical retrace.
	// It is not possible to change the actual used address during a frame.
	switchBuffer(renderer->buffer, scroll, map);

	renderBackground(renderer, map);
	renderSprites(renderer, spriteInstances, map);

	// V-sync.
	waitForFrame();
}
