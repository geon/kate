#include "platform/dos/ega.h"

#include <stdio.h>


int main (int argc, char* argv[]) {

	unsigned int  column, y;
	unsigned char colors[8];
	unsigned long int strip;
	unsigned char palette[16];
	
	setVideoMode();

	palette[0] = 4;
	palette[1] = 1;
	palette[2] = 8;
	palette[3] = 63;
	palette[4] = 36;
	palette[5] = 5;
	palette[6] = 52;
	palette[7] = 18;
	palette[8] = 9;
	palette[9] = 53;
	palette[10] = 42;
	palette[11] = 36;
	palette[12] = 12;
	palette[13] = 41;
	palette[14] = 23;
	palette[15] = 11;
	setPalette(palette);

	drawPoint(100, 100, 0x04);

	colors[0] = 0;
	colors[1] = 1;
	colors[2] = 2;
	colors[3] = 3;
	colors[4] = 4;
	colors[5] = 5;
	colors[6] = 6;
	colors[7] = 7;
	strip = makeStrip(colors);

	drawStrip(0, 0, strip, 0xFF);
	copyStrip(0, 0);

	for (y=0; y<350; ++y) {
		for (column=0; column<640/8/2; ++column) {
			pasteStrip (column*2, y, 0xFF);
		}
	}

	colors[0] = 0+8;
	colors[1] = 1+8;
	colors[2] = 2+8;
	colors[3] = 3+8;
	colors[4] = 4+8;
	colors[5] = 5+8;
	colors[6] = 6+8;
	colors[7] = 7+8;
	strip = makeStrip(colors);

	drawStrip(1, 0, strip, 0xFF);
	copyStrip(1, 0);

	for (y=0; y<350; ++y) {
		for (column=0; column<640/8/2; ++column) {
			pasteStrip (column*2+1, y, 0xFF);
		}
	}

	printf("\n\n\ndone");
	return 0;
}
