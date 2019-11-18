#include "platform/dos/ega.h"
#include "platform/dos/bitplane_strip.h"
#include "bmp.h"
#include "image.h"

#include <stdio.h>
#include <math.h>


int main (int argc, char* argv[]) {

	int  y;
	unsigned int  column;
	BitplaneStrip strip;
	Image image;

	int radius, posX, posY;
	unsigned int frame;

	setVideoMode();

	if(!loadBmp(&image, "../images/bunny.bmp", true)) {
		printf("Loading bmp failed.");
		return 1;
	}

	setPalette(image.palette);

	strip = makeBitplaneStrip(0xffffffff);

	drawStrip(0, 0, strip, 0xFF);
	copyStrip(0, 0);

	for (y=0; y<350; ++y) {
		for (column=0; column<640/8; ++column) {
			pasteStrip(column, y, 0xFF);
		}
	}

	for (frame=0; frame < 100; ++frame) {
		radius = sin(frame/4.3435674)*20 + 50;
		posX = 100 + sin(frame/10.0) * radius;
		posY = 100 + cos(frame/10.0) * radius;
		drawImage(image, posX, posY);
		waitForFrame();
	}

	freeImage(image);

	printf("\n\n\ndone");
	return 0;
}
