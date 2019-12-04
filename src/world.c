#include "world.h"
#include "bmp.h"
#include "platform/dos/ega.h"
#include "image_struct.h"

#include <stdlib.h>
#include <math.h>

#define NUM_BUNNY_IMAGES 4

typedef struct WorldScroll {
	unsigned short int x;
	unsigned short int y;
} WorldScroll;


typedef struct WorldStruct {
	Image bunnyImages[NUM_BUNNY_IMAGES];
	Image backgroundImage;
	unsigned int frame;
	DirtyBackgroundStrips dirtyBackgroundStrips;

	int radius, posX, posY;
	WorldScroll scroll;
} WorldStruct;


World makeWorld (char **errorMessage) {
    World world = malloc(sizeof(WorldStruct));
	world->frame = 0;
	world->radius = 80;
	world->posX = 100;
	world->posY = 100;
	world->scroll.x = 0;
	world->scroll.y = 0;

	world->bunnyImages[2] = loadBmp("../images/bunny1.bmp", true, errorMessage);
	world->bunnyImages[3] = loadBmp("../images/bunny2.bmp", true, errorMessage);
	world->bunnyImages[0] = loadBmp("../images/bunny3.bmp", true, errorMessage);
	world->bunnyImages[1] = loadBmp("../images/bunny4.bmp", true, errorMessage);

	world->backgroundImage =  loadBmp("../images/backgr.bmp", false, errorMessage);
	if(!world->backgroundImage) {
		return NULL;
	}

	world->dirtyBackgroundStrips = makeDirtyBackgroundStrips();

	return world;
}


void freeWorld (World world) {
	unsigned int i;

	freeDirtyBackgroundStrips(world->dirtyBackgroundStrips);
	freeImage(world->backgroundImage);
	for (i=0; i<NUM_BUNNY_IMAGES; ++i) {
		freeImage(world->bunnyImages[i]);
	}
	free(world);
}


unsigned char * getWorldPalette(World world) {
	return getImagePalette(world->backgroundImage);
}


void updateWorld (World world) {
	world->frame++;
	world->radius = sin(world->frame/4.3435674)*20 + 50;
	world->posX = 100 + sin(world->frame/10.0) * world->radius;
	world->posY = 100 + cos(world->frame/10.0) * world->radius;
	world->scroll.x = world->frame/3;
	world->scroll.y = world->frame/2;
}


void drawImage(World world, Image image, unsigned int posX, unsigned int posY, bool alternateBuffer) {
	unsigned int y, column;
	BitplaneStrip strip;
	unsigned int posXColumn, posXRest;
	BitplaneStrip stripShiftedA, stripShiftedB;
	unsigned char shiftMask;
	unsigned int sourceStripIndex;
	unsigned int destinationStripIndex;

	for (y = 0; y < image->height; ++y) {
		for (column=0; column<image->numColumns; ++column) {
			sourceStripIndex = column + (image->upsideDown ? (image->height - 1 - y) : y)*image->numColumns;
			strip = makeBitplaneStrip(image->pixels[sourceStripIndex]);

			posXColumn = posX/8;
			posXRest = posX%8;

			destinationStripIndex = stripCoordToIndex(posXColumn + column, posY + y, alternateBuffer);

			stripShiftedA.planes[0] = strip.planes[0] >> posXRest;
			stripShiftedA.planes[1] = strip.planes[1] >> posXRest;
			stripShiftedA.planes[2] = strip.planes[2] >> posXRest;
			stripShiftedA.planes[3] = strip.planes[3] >> posXRest;

			stripShiftedB.planes[0] = strip.planes[0] << (8 - posXRest);
			stripShiftedB.planes[1] = strip.planes[1] << (8 - posXRest);
			stripShiftedB.planes[2] = strip.planes[2] << (8 - posXRest);
			stripShiftedB.planes[3] = strip.planes[3] << (8 - posXRest);

			// TODO: Combine the adjecent strips, to avoid double writes.
			shiftMask = image->mask[sourceStripIndex] >> posXRest;
			drawStrip(destinationStripIndex, stripShiftedA, shiftMask);
			world->dirtyBackgroundStrips[destinationStripIndex] = true;
			shiftMask = image->mask[sourceStripIndex] << (8 - posXRest);
			drawStrip(destinationStripIndex + 1, stripShiftedB, shiftMask);
			world->dirtyBackgroundStrips[destinationStripIndex+1] = true;
		}
	}
}


void renderSprites (World world, bool alternateBuffer) {
	unsigned int i;

	for (i=0; i<NUM_BUNNY_IMAGES; ++i) {
		drawImage(world, world->bunnyImages[i], world->posX + i*64, world->posY, alternateBuffer);
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
	Image backgroundImage = world->backgroundImage;

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

			sourceStripIndex = sourceStripColumn + (backgroundImage->upsideDown ? (backgroundImage->height - 1 - sourceStripY) : sourceStripY) * backgroundImage->numColumns;
			drawStrip(
				destinationStripIndex,
				makeBitplaneStrip(backgroundImage->pixels[sourceStripIndex]),
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
