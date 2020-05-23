#include "world.h"
#include "bmp.h"
#include "platform/dos/ega.h"
#include "sprite.h"
#include "sprite_instance.h"
#include "sprite_struct.h"
#include "map.h"
#include "strip_coord.h"

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

	Map map;
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

	if (!(world->map = makeMap(errorMessage))) {
		return NULL;
	}

	world->dirtyBackgroundStrips = makeDirtyBackgroundStrips();

	return world;
}


void freeWorld (World world) {
	unsigned int i;

	freeMap(world->map);
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


void setWorldScroll (World world, unsigned short x, unsigned short y) {
	world->scroll.x = x;
	world->scroll.y = y;
}


void updateWorld (World world) {
	unsigned int radius;
	unsigned short int i;
	unsigned int posX;
	unsigned int posY;

	world->frame++;

	radius = sin(world->frame/4.3435674)*20 + 50;
	posX = 200 + sin(world->frame/10.0) * radius;
	posY = 200 + cos(world->frame/10.0) * radius;

	setWorldScroll(world, world->frame, world->frame);

	for (i=0; i<world->numSpriteInstances; ++i) {
		world->spriteInstances[i].posX = posX + i*64;
		world->spriteInstances[i].posY = posY;
	}
}


unsigned short int stripWorldCoordToBufferIndex (unsigned short int column, unsigned short int y, bool alternateBuffer) {
	return y*EGA_BUFFER_NUM_COLUMNS + column + (alternateBuffer ? EGA_BUFFER_SIZE : 0);
}


void worldSetScroll (unsigned short int x, unsigned short int y, bool alternateBuffer) {
	unsigned short int column = x/8;
	unsigned char restX = x%8;
	unsigned short int offset = stripWorldCoordToBufferIndex(column, y, alternateBuffer);

	setBufferOffset(offset, restX);
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

			destinationStripIndex = stripWorldCoordToBufferIndex(posXColumn + column, spriteInstance->posY + y, alternateBuffer);

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
			markDirtyBackgroundStrips(world->dirtyBackgroundStrips, destinationStripIndex);
			shiftMask = spriteInstance->sprite->mask[sourceStripIndex] << (8 - posXRest);
			drawStrip(destinationStripIndex + 1, stripShiftedB, shiftMask);
			markDirtyBackgroundStrips(world->dirtyBackgroundStrips, destinationStripIndex+1);
		}
	}
}


void renderSprites (World world, bool alternateBuffer) {
	unsigned int i;

	for (i=0; i<world->numSpriteInstances; ++i) {
		drawSprite(world, &world->spriteInstances[i], alternateBuffer);
	}
}


StripCoord mapBufferIndexToBufferCoord (unsigned short int bufferIndex, WorldScroll worldScroll, bool alternateBuffer) {
	unsigned short int bufferStripIndexStart = stripWorldCoordToBufferIndex(worldScroll.x/8, worldScroll.y, alternateBuffer);
	StripCoord bufferCoord;
	bufferCoord.column = (bufferIndex - bufferStripIndexStart) % EGA_BUFFER_NUM_COLUMNS;
	bufferCoord.y = (bufferIndex - bufferStripIndexStart) / EGA_BUFFER_NUM_COLUMNS;
	return  bufferCoord;
}


StripCoord mapBufferCoordToWorldCoord (StripCoord bufferCoord, WorldScroll worldScroll) {
	StripCoord worldCoord;
	worldCoord.column = bufferCoord.column + worldScroll.x/8;
	worldCoord.y = bufferCoord.y + worldScroll.y;
	return  worldCoord;
}


void renderBackground (World world, bool alternateBuffer) {
	unsigned long int i;
	unsigned long int numIndices = getDirtyBackgroundStripsNumIndices(world->dirtyBackgroundStrips);
	unsigned short int *indices = getDirtyBackgroundStripsIndices(world->dirtyBackgroundStrips);

	for (i=0; i<numIndices; ++i) {
		StripCoord bufferCoord = mapBufferIndexToBufferCoord(indices[i], world->scroll, alternateBuffer);
		StripCoord worldCoord = mapBufferCoordToWorldCoord(bufferCoord, world->scroll);

		drawStrip(
			indices[i],
			getStripAtWorldCoord(world->map, worldCoord),
			0xFF
		);
	}

	clearDirtyBackgroundStrips(world->dirtyBackgroundStrips);
}


void renderWorld(World world) {
	bool alternateBuffer = world->frame%2;
	renderBackground(world, alternateBuffer);
	renderSprites(world, alternateBuffer);
	// Sets the start-address of the next frame.
	// The value won't be latched by the EGA card until the vertical retrace.
	// It is not possible to change the actual used address during a frame.
	worldSetScroll(world->scroll.x, world->scroll.y, alternateBuffer);
	// V-sync.
	waitForFrame();
}
