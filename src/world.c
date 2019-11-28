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


void drawImage(Image image, unsigned int posX, unsigned int posY) {
	unsigned int y, column;
	BitplaneStrip strip;
	unsigned int posXColumn, posXRest;
	BitplaneStrip stripShiftedA, stripShiftedB;
	unsigned char shiftMask;
	unsigned int stripIndex;

	for (y = 0; y < image->height; ++y) {
		for (column=0; column<image->numColumns; ++column) {
			stripIndex = column + (image->upsideDown ? (image->height - 1 - y) : y)*image->numColumns;
			strip = makeBitplaneStrip(image->pixels[stripIndex]);

			posXColumn = posX/8;
			posXRest = posX%8;

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
			drawStrip(column + posXColumn, y + posY, stripShiftedA, shiftMask);
			shiftMask = image->mask[stripIndex] << (8 - posXRest);
			drawStrip(column + posXColumn + 1, y + posY, stripShiftedB, shiftMask);
		}
	}
}


void renderSprites (World world) {
	int  y;
	unsigned int  column;
	unsigned int stripIndex;
    unsigned int numColumns = getImageNumColumns(world->image);

	drawImage(world->image, world->posX, world->posY);

	// TODO: This is repeating the loop in drawImage. Move there?
	// Mark the covered background strips as dirty.
	for (y=world->posY; y<world->posY+64; ++y) {
		for (column=world->posX/8; column<world->posX/8+numColumns+1; ++column) {
			// The parenthesis around 640/8 is important. Without it, y*640 overflows.
			stripIndex = column + y*(640/8);
			world->dirtyBackgroundStrips[stripIndex] = true;
		}
	}
}


void renderBackground (World world) {
	int  y;
	unsigned int  column;
	BitplaneStrip strip;
	unsigned int stripIndex;

	strip = makeBitplaneStrip(0xffffffff);

	drawStrip(0, 0, strip, 0xFF);
	copyStrip(0, 0);

	for (y=0; y<350; ++y) {
		for (column=0; column<640/8; ++column) {
			stripIndex = column + y*(640/8);
			if (world->dirtyBackgroundStrips[stripIndex]) {
				pasteStrip(column, y, 0xFF);
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
