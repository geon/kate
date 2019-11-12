#include "platform/dos/ega.h"

#include <stdio.h>


int main (int argc, char* argv[]) {

	unsigned int  column, y;
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

	strip = makeBitplaneStrip(0x01234567);

	drawStrip(0, 0, strip, 0xFF);
	copyStrip(0, 0);

	for (y=0; y<350; ++y) {
		for (column=0; column<640/8/2; ++column) {
			pasteStrip (column*2, y, 0xFF);
		}
	}

	strip = makeBitplaneStrip(0x89abcdef);

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
