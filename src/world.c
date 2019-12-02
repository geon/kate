#include "world.h"
#include "bmp.h"
#include "platform/dos/ega.h"
#include "image_struct.h"

#include <stdlib.h>
#include <math.h>

typedef struct WorldScroll {
	unsigned short int x;
	unsigned short int y;
} WorldScroll;


typedef struct WorldStruct {
	Image image;
	unsigned int frame;
	DirtyBackgroundStrips dirtyBackgroundStrips;

	int radius, posX, posY;
	WorldScroll scroll;
} WorldStruct;


World makeWorld () {
    World world = malloc(sizeof(WorldStruct));
	world->frame = 0;
	world->radius = 80;
	world->posX = 100;
	world->posY = 100;
	world->scroll.x = 0;
	world->scroll.y = 0;

	world->image = loadBmp("../images/bunny.bmp", true, NULL);

	world->dirtyBackgroundStrips = makeDirtyBackgroundStrips();

	return world;
}


void freeWorld (World world) {
	freeDirtyBackgroundStrips(world->dirtyBackgroundStrips);
	freeImage(world->image);
	free(world);
}


unsigned char * getWorldPalette(World world) {
	return getImagePalette(world->image);
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
	drawImage(world, world->image, world->posX, world->posY, alternateBuffer);
}


void renderBackground (World world, bool alternateBuffer) {
	unsigned long int i;
	BitplaneStrip strip;
	bool *dirtyBackgroundStrips;
	unsigned short int bufferStripIndexStart;
	unsigned short int destinationStripIndex;

	strip = makeBitplaneStrip(0xffffffff);

	drawStrip(0, strip, 0xFF);
	copyStrip(0);

	dirtyBackgroundStrips = world->dirtyBackgroundStrips;
	bufferStripIndexStart = stripCoordToIndex(world->scroll.x/8, world->scroll.y, alternateBuffer);
	for (i=0; i<EGA_BUFFER_SIZE; ++i) {
		// Assign to a `unsigned short int`, to force an overflow and make it start over from 0, just like the EGA bit planes.
		destinationStripIndex = bufferStripIndexStart + i;
		if (dirtyBackgroundStrips[destinationStripIndex]) {
			pasteStrip(destinationStripIndex, 0xFF);
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
