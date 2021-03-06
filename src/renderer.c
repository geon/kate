#include "renderer.h"
#include "bmp.h"
#include "platform/dos/ega.h"
#include "platform/dos/bitplane_strip.h"
#include "buffer.h"
#include "sprite.h"
#include "sprite_instance.h"
#include "map.h"
#include "strip_coord.h"
#include "pixel_coord.h"
#include "ega_scroll_coord.h"
#include "coord_conversion.h"
#include "vector.h"
#include "uint16_vector.h"
#include "position_and_strip.h"

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
	memcpy(renderer->palette, getSpritePalette(sprite), 16);

	return sprite;
}


void rendererRender(Renderer renderer, SpriteInstanceVector spriteInstances, Map map, PixelCoord scrollOfNextFrame) {
	// V-sync.
	waitForFrame();

	bufferRenderBackground(renderer->buffer, map);
	spriteInstanceRenderSprites(spriteInstances, map, renderer->buffer);

	// Sets the start-address of the buffer.
	// The value won't be latched by the EGA card until the vertical retrace.
	// It is not possible to change the actual used address during a frame.
	switchBuffer(renderer->buffer, scrollOfNextFrame, map);
}
