#include "platform/dos/ega.h"
#include "bmp.h"

#include <stdio.h>


int main (int argc, char* argv[]) {

	int  y;
	unsigned int  column;
	unsigned long int strip;
	Image image;

	setVideoMode();

	if(!loadBmp(&image, "../images/bunny.bmp")) {
		printf("Loading bmp failed.");
		return 1;
	}

	setPalette(image.palette);

	strip = makeBitplaneStrip(0x01234567);

	drawStrip(0, 0, strip, 0xFF);
	copyStrip(0, 0);

	for (y=0; y<350; ++y) {
		for (column=0; column<640/8; ++column) {
			pasteStrip(column, y, 0xFF);
		}
	}

	for (y = 0; y < image.height; ++y) {
		for (column=0; column<image.numColumns; ++column) {
			strip = makeBitplaneStrip(image.pixels[column + (image.upsideDown ? (image.height - 1 - y) : y)*image.numColumns]);
			drawStrip(column + 10, y + 100, strip, 0xFF);
		}
	}

	freeImage(image);

	printf("\n\n\ndone");
	return 0;
}
