#include "platform/dos/ega.h"
#include "platform/dos/bitplane_strip.h"
#include "bmp.h"
#include "image.h"

#include <stdio.h>
#include <math.h>


void renderBackground () {
	int  y;
	unsigned int  column;
	BitplaneStrip strip;

	strip = makeBitplaneStrip(0xffffffff);

	drawStrip(0, 0, strip, 0xFF);
	copyStrip(0, 0);

	for (y=0; y<350; ++y) {
		for (column=0; column<640/8; ++column) {
			pasteStrip(column, y, 0xFF);
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


void renderSprites (World world, Image image) {
	drawImage(image, world.posX, world.posY);
}


int main (int argc, char* argv[]) {

	Image image;
	unsigned int frame;
	World world;

	setVideoMode();

	if(!loadBmp(&image, "../images/bunny.bmp", true)) {
		printf("Loading bmp failed.");
		return 1;
	}

	setPalette(image.palette);

	makeWorld(&world);

	for (frame=0; frame < 100; ++frame) {
		updateWorld(&world, frame);
		renderBackground();
		renderSprites(world, image);
		waitForFrame();
	}

	freeImage(image);

	printf("\n\n\ndone");
	return 0;
}
