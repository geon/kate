#include "world.h"
#include "bmp.h"
#include "platform/dos/ega.h"

#include <math.h>


void makeWorld (World *world) {
	world->radius = 80;
	world->posX = 100;
	world->posY = 100;

	loadBmp(&world->image, "../images/bunny.bmp", true);

	world->dirtyBackgroundStrips = makeDirtyBackgroundStrips();
}


void freeWorld (World world) {
	freeDirtyBackgroundStrips(world.dirtyBackgroundStrips);
	freeImage(world.image);
}


void updateWorld (World *world) {
	world->frame++;
	world->radius = sin(world->frame/4.3435674)*20 + 50;
	world->posX = 100 + sin(world->frame/10.0) * world->radius;
	world->posY = 100 + cos(world->frame/10.0) * world->radius;
}


void renderSprites (World world) {
	int  y;
	unsigned int  column;
	unsigned int stripIndex;

	drawImage(world.image, world.posX, world.posY);

	// TODO: This is repeating the loop in drawImage. Move there?
	// Mark the covered background strips as dirty.
	for (y=world.posY; y<world.posY+64; ++y) {
		for (column=world.posX/8; column<world.posX/8+world.image.numColumns+1; ++column) {
			// The parenthesis around 640/8 is important. Without it, y*640 overflows.
			stripIndex = column + y*(640/8);
			world.dirtyBackgroundStrips[stripIndex] = true;
		}
	}
}


void renderBackground (DirtyBackgroundStrips dirtyBackgroundStrips) {
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
			if (dirtyBackgroundStrips[stripIndex]) {
				pasteStrip(column, y, 0xFF);
				dirtyBackgroundStrips[stripIndex] = false;
			}
		}
	}
}


void renderWorld(World world) {
	renderBackground(world.dirtyBackgroundStrips);
	renderSprites(world);
}
