#include "world.h"
#include "bmp.h"
#include "platform/dos/ega.h"
#include "sprite.h"
#include "sprite_instance.h"
#include "sprite_struct.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct WorldScroll {
	unsigned short int x;
	unsigned short int y;
} WorldScroll;


typedef struct WorldStruct {
	unsigned char palette[16];

	unsigned short int numSprites;
	Sprite *sprites;

	unsigned short int numSpriteInstances;
	SpriteInstance *spriteInstances;

	unsigned int frame;
	DirtyBackgroundStrips dirtyBackgroundStrips;

	WorldScroll scroll;
} WorldStruct;


Sprite worldLoadSprite (World world, char *imagePath, char **errorMessage) {
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

	return sprite;
}


World makeWorld (char **errorMessage) {
	unsigned short int i;
	char *spritePaths[] =  {
		"../images/bunny3.bmp",
		"../images/bunny4.bmp",
		"../images/bunny1.bmp",
		"../images/bunny2.bmp"
	};
	unsigned int spritePathArrayLength = sizeof(spritePaths) / sizeof(spritePaths[0]);
	Sprite sprite;
    World world = malloc(sizeof(WorldStruct));
	world->frame = 0;
	world->scroll.x = 0;
	world->scroll.y = 0;

	world->numSprites = 0;
	world->sprites = malloc(sizeof(Sprite) * spritePathArrayLength);
	world->numSpriteInstances = spritePathArrayLength;
	world->spriteInstances = malloc(sizeof(SpriteInstance) * world->numSpriteInstances);
	for (i=0; i<spritePathArrayLength; ++i) {
		if (!(sprite = worldLoadSprite(world, spritePaths[i], errorMessage))) {
			return NULL;
		}
		world->spriteInstances[i] = makeSpriteInstance(sprite, 0, 0);
	}

	if (!worldLoadSprite(world, "../images/backgr.bmp", errorMessage)) {
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
	free(world->spriteInstances);
	free(world);
}


unsigned char * getWorldPalette(World world) {
	return world->palette;
}


void updateWorld (World world) {
	unsigned int radius;
	unsigned short int i;
	unsigned int posX;
	unsigned int posY;

	world->frame++;

	radius = sin(world->frame/4.3435674)*20 + 50;
	posX = 100 + sin(world->frame/10.0) * radius;
	posY = 100 + cos(world->frame/10.0) * radius;

	world->scroll.x = world->frame/3;
	world->scroll.y = world->frame/2;

	for (i=0; i<world->numSpriteInstances; ++i) {
		world->spriteInstances[i].posX = posX + i*64;
		world->spriteInstances[i].posY = posY;
	}
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

	for (i=0; i<world->numSpriteInstances; ++i) {
		drawSprite(world, &world->spriteInstances[i], alternateBuffer);
	}
}


BitplaneStrip getStripAtWorldCoord(Sprite backgroundImage, unsigned short int column, unsigned short int y) {
	unsigned short int sourceStripColumn;
	unsigned short int sourceStripY;
	unsigned short int sourceStripIndex;

	sourceStripColumn = column % backgroundImage->numColumns;
	sourceStripY = y % backgroundImage->height;

	sourceStripIndex = sourceStripColumn + sourceStripY * backgroundImage->numColumns;

	return backgroundImage->bitPlaneStrips[sourceStripIndex];
}


void renderBackground (World world, bool alternateBuffer) {
	unsigned long int column, y;
	bool *dirtyBackgroundStrips;
	unsigned short int bufferStripIndexStart;
	unsigned short int destinationStripIndex;
	unsigned short int bufferStart;
	Sprite backgroundImage = world->sprites[world->numSpriteInstances];

	unsigned short int worldStartColumn =world->scroll.x/8;
	unsigned short int worldStartY = world->scroll.y;

	dirtyBackgroundStrips = world->dirtyBackgroundStrips;
	bufferStripIndexStart = stripCoordToIndex(worldStartColumn, worldStartY, alternateBuffer);
	bufferStart = stripCoordToIndex(0, 0, alternateBuffer);

	destinationStripIndex = bufferStripIndexStart;
	for (y=0; y<350; ++y) {
		for (column=0; column<EGA_BUFFER_NUM_COLUMNS; ++column) {

			//Using a `unsigned short int`, to force an overflow and make it start over from 0, just like the EGA bit planes.
			if (dirtyBackgroundStrips[destinationStripIndex]) {
				drawStrip(
					destinationStripIndex,
					getStripAtWorldCoord(backgroundImage, worldStartColumn + column, worldStartY + y),
					0xFF
				);
				dirtyBackgroundStrips[destinationStripIndex] = false;
			}

			++destinationStripIndex;
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
