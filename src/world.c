#include "world.h"
#include "bmp.h"
#include "platform/dos/ega.h"
#include "image_struct.h"

#include <stdlib.h>
#include <math.h>

typedef struct WorldStruct {
	Image image;
	unsigned int frame;
	DirtyBackgroundStrips dirtyBackgroundStrips;

	int radius, posX, posY;
} WorldStruct;



World makeWorld () {
    World world = malloc(sizeof(WorldStruct));
	world->frame = 0;
	world->radius = 80;
	world->posX = 100;
	world->posY = 100;

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
}


void drawImage(World world, Image image, unsigned int posX, unsigned int posY) {
	unsigned int y, column;
	BitplaneStrip strip;
	unsigned int posXColumn, posXRest;
	BitplaneStrip stripShiftedA, stripShiftedB;
	unsigned char shiftMask;
	unsigned int stripIndex;
	unsigned int backgroundStripIndex;

	for (y = 0; y < image->height; ++y) {
		for (column=0; column<image->numColumns; ++column) {
			stripIndex = column + (image->upsideDown ? (image->height - 1 - y) : y)*image->numColumns;
			strip = makeBitplaneStrip(image->pixels[stripIndex]);

			posXColumn = posX/8;
			posXRest = posX%8;

			backgroundStripIndex = stripCoordToIndex(posXColumn + column, posY + y);

			stripShiftedA.planes[0] = strip.planes[0] >> posXRest;
			stripShiftedA.planes[1] = strip.planes[1] >> posXRest;
			stripShiftedA.planes[2] = strip.planes[2] >> posXRest;
			stripShiftedA.planes[3] = strip.planes[3] >> posXRest;

			stripShiftedB.planes[0] = strip.planes[0] << (8 - posXRest);
			stripShiftedB.planes[1] = strip.planes[1] << (8 - posXRest);
			stripShiftedB.planes[2] = strip.planes[2] << (8 - posXRest);
			stripShiftedB.planes[3] = strip.planes[3] << (8 - posXRest);

			// TODO: Combine the adjecent strips, to avoid double writes.
			shiftMask = image->mask[stripIndex] >> posXRest;
			drawStrip(stripCoordToIndex(column + posXColumn, y + posY), stripShiftedA, shiftMask);
			world->dirtyBackgroundStrips[backgroundStripIndex] = true;
			shiftMask = image->mask[stripIndex] << (8 - posXRest);
			drawStrip(stripCoordToIndex(column + posXColumn + 1, y + posY), stripShiftedB, shiftMask);
			world->dirtyBackgroundStrips[backgroundStripIndex+1] = true;
		}
	}
}


void renderSprites (World world) {
	drawImage(world, world->image, world->posX, world->posY);
}


void renderBackground (World world) {
	int  y;
	unsigned int  column;
	BitplaneStrip strip;
	unsigned int stripIndex;

	strip = makeBitplaneStrip(0xffffffff);

	drawStrip(0, strip, 0xFF);
	copyStrip(0);

	for (y=0; y<350; ++y) {
		for (column=0; column<640/8; ++column) {
			stripIndex = stripCoordToIndex(column, y);
			if (world->dirtyBackgroundStrips[stripIndex]) {
				pasteStrip(stripIndex, 0xFF);
				world->dirtyBackgroundStrips[stripIndex] = false;
			}
		}
	}
}


void renderWorld(World world) {
	setScroll(world->frame/3, world->frame/2);
	renderBackground(world);
	renderSprites(world);
}
