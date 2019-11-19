#include "platform/dos/ega.h"
#include "platform/dos/bitplane_strip.h"
#include "bmp.h"
#include "image.h"
#include "dirty_background_strips.h"

#include <stdio.h>
#include <math.h>


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


typedef struct World {
	int radius, posX, posY;
} World;


void makeWorld (World *world) {
	world->radius = 80;
	world->posX = 100;
	world->posY = 100;
}


void updateWorld (World *world, unsigned int frame) {
	world->radius = sin(frame/4.3435674)*20 + 50;
	world->posX = 100 + sin(frame/10.0) * world->radius;
	world->posY = 100 + cos(frame/10.0) * world->radius;
}


void renderSprites (World world, Image image, DirtyBackgroundStrips dirtyBackgroundStrips) {
	int  y;
	unsigned int  column;
	unsigned int stripIndex;

	drawImage(image, world.posX, world.posY);

	// TODO: This is repeating the loop in drawImage. Move there?
	// Mark the covered background strips as dirty.
	for (y=world.posY; y<world.posY+64; ++y) {
		for (column=world.posX/8; column<world.posX/8+image.numColumns+1; ++column) {
			// The parenthesis around 640/8 is important. Without it, y*640 overflows.
			stripIndex = column + y*(640/8);
			dirtyBackgroundStrips[stripIndex] = true;
		}
	}
}


int main (int argc, char* argv[]) {

	Image image;
	unsigned int frame;
	World world;
	DirtyBackgroundStrips dirtyBackgroundStrips;

	setVideoMode();

	if(!loadBmp(&image, "../images/bunny.bmp", true)) {
		printf("Loading bmp failed.");
		return 1;
	}

	setPalette(image.palette);

	makeWorld(&world);

	dirtyBackgroundStrips = makeDirtyBackgroundStrips();

	for (frame=0; frame < 100; ++frame) {
		updateWorld(&world, frame);
		renderBackground(dirtyBackgroundStrips);
		renderSprites(world, image, dirtyBackgroundStrips);
		waitForFrame();
	}

	freeDirtyBackgroundStrips(dirtyBackgroundStrips);
	freeImage(image);

	printf("\n\n\ndone");
	return 0;
}
