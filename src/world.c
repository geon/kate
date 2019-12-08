#include "world.h"
#include "bmp.h"
#include "platform/dos/ega.h"
#include "sprite.h"
#include "sprite_instance.h"
#include "sprite_struct.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#define NUM_BUNNY_IMAGES 4

typedef struct WorldScroll {
	unsigned short int x;
	unsigned short int y;
} WorldScroll;


typedef struct WorldStruct {
	unsigned char palette[16];

	unsigned short int numSprites;
	Sprite *sprites;

	unsigned int frame;
	DirtyBackgroundStrips dirtyBackgroundStrips;

	int posX, posY;
	WorldScroll scroll;
} WorldStruct;


bool worldLoadSprite (World world, char *imagePath, char **errorMessage) {
	Image image;
	Sprite sprite;

	image = loadBmp(imagePath, true, errorMessage);
	if (!image) {
		return false;
	}

	sprite = makeSprite(image);
	freeImage(image);

	// TODO: Make sure it is the same palette, or remap colors.
	world->sprites[world->numSprites++] = sprite;
	memcpy(world->palette, sprite->palette, 16);

	return true;
}


World makeWorld (char **errorMessage) {
    World world = malloc(sizeof(WorldStruct));
	world->frame = 0;
	world->posX = 100;
	world->posY = 100;
	world->scroll.x = 0;
	world->scroll.y = 0;

	world->numSprites = 0;
	world->sprites = malloc(sizeof(Sprite) * (NUM_BUNNY_IMAGES + 1));
	if (!(
		worldLoadSprite(world, "../images/bunny3.bmp", errorMessage) &&
		worldLoadSprite(world, "../images/bunny4.bmp", errorMessage) &&
		worldLoadSprite(world, "../images/bunny1.bmp", errorMessage) &&
		worldLoadSprite(world, "../images/bunny2.bmp", errorMessage) &&
		worldLoadSprite(world, "../images/backgr.bmp", errorMessage)
	)) {
		return NULL;
	}

	world->dirtyBackgroundStrips = makeDirtyBackgroundStrips();

	return world;
}


void freeWorld (World world) {
	unsigned int i;

	freeDirtyBackgroundStrips(world->dirtyBackgroundStrips);
	for (i=0; i<world->numSprites; ++i) {
		freeSprite(world->sprites[i]);
	}
	free(world);
}


unsigned char * getWorldPalette(World world) {
	return world->palette;
}


void updateWorld (World world) {
	unsigned int radius;

	world->frame++;
	radius = sin(world->frame/4.3435674)*20 + 50;
	world->posX = 100 + sin(world->frame/10.0) * radius;
	world->posY = 100 + cos(world->frame/10.0) * radius;
	world->scroll.x = world->frame/3;
	world->scroll.y = world->frame/2;
}


void drawSprite(World world, SpriteInstance *spriteInstance, bool alternateBuffer) {
	unsigned int y, column;
	BitplaneStrip strip;
	unsigned int posXColumn, posXRest;
	BitplaneStrip stripShiftedA, stripShiftedB;
	unsigned char shiftMask;
	unsigned int sourceStripIndex;
	unsigned int destinationStripIndex;

	for (y = 0; y < spriteInstance->sprite->height; ++y) {
		for (column=0; column<spriteInstance->sprite->numColumns; ++column) {
			sourceStripIndex = column + y*spriteInstance->sprite->numColumns;
			strip = spriteInstance->sprite->bitPlaneStrips[sourceStripIndex];

			posXColumn = spriteInstance->posX/8;
			posXRest = spriteInstance->posX%8;

			destinationStripIndex = stripCoordToIndex(posXColumn + column, spriteInstance->posY + y, alternateBuffer);

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
			world->dirtyBackgroundStrips[destinationStripIndex] = true;
			shiftMask = spriteInstance->sprite->mask[sourceStripIndex] << (8 - posXRest);
			drawStrip(destinationStripIndex + 1, stripShiftedB, shiftMask);
			world->dirtyBackgroundStrips[destinationStripIndex+1] = true;
		}
	}
}


void renderSprites (World world, bool alternateBuffer) {
	unsigned int i;
	SpriteInstance instance;

	for (i=0; i<NUM_BUNNY_IMAGES; ++i) {
		instance = makeSpriteInstance(world->sprites[i], world->posX+i*64, world->posY);
		drawSprite(world, &instance, alternateBuffer);
	}
}


void renderBackground (World world, bool alternateBuffer) {
	unsigned long int i;
	bool *dirtyBackgroundStrips;
	unsigned short int bufferStripIndexStart;
	unsigned short int destinationStripColumn;
	unsigned short int destinationStripY;
	unsigned short int destinationStripIndex;
	unsigned short int sourceStripColumn;
	unsigned short int sourceStripY;
	unsigned short int sourceStripIndex;
	unsigned short int bufferStart;
	Sprite backgroundImage = world->sprites[NUM_BUNNY_IMAGES];

	dirtyBackgroundStrips = world->dirtyBackgroundStrips;
	bufferStripIndexStart = stripCoordToIndex(world->scroll.x/8, world->scroll.y, alternateBuffer);
	bufferStart = stripCoordToIndex(0, 0, alternateBuffer);
	for (i=0; i<EGA_BUFFER_SIZE; ++i) {
		// Assign to a `unsigned short int`, to force an overflow and make it start over from 0, just like the EGA bit planes.
		destinationStripIndex = bufferStripIndexStart + i;
		if (dirtyBackgroundStrips[destinationStripIndex]) {
			destinationStripColumn = (destinationStripIndex - bufferStart) % EGA_BUFFER_NUM_COLUMNS;
			destinationStripY = (destinationStripIndex - bufferStart) / EGA_BUFFER_NUM_COLUMNS;

			sourceStripColumn = destinationStripColumn % backgroundImage->numColumns;
			sourceStripY = destinationStripY % backgroundImage->height;

			sourceStripIndex = sourceStripColumn + sourceStripY * backgroundImage->numColumns;
			drawStrip(
				destinationStripIndex,
				backgroundImage->bitPlaneStrips[sourceStripIndex],
				0xFF
			);
			dirtyBackgroundStrips[destinationStripIndex] = false;
		}
	}
}


void renderWorld(World world) {
	bool alternateBuffer = world->frame%2;
	renderBackground(world, alternateBuffer);
	renderSprites(world, alternateBuffer);
	// Sets the start-address of the next frame.
	// The value won't be latched by the EGA card until the vertical retrace.
	// It is not possible to change the actual used address during a frame.
	setScroll(world->scroll.x, world->scroll.y, alternateBuffer);
	// V-sync.
	waitForFrame();
}
