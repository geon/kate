#include "bitplane_strip.h"

#define BIT_0 1
#define BIT_1 2
#define BIT_2 4
#define BIT_3 8

/**
	Turn an array of 8 4-bit colors into 4 bit-planes, each with an 8-bit wide strip of pixels.
*/
BitplaneStrip makeBitplaneStrip (unsigned long int nibbleStrip) {

	unsigned long int
		pixel0,
		pixel1,
		pixel2,
		pixel3,
		pixel4,
		pixel5,
		pixel6,
		pixel7;

	BitplaneStrip bitplaneStrip;

	pixel0 = (nibbleStrip >> 28) & 0xf;
	pixel1 = (nibbleStrip >> 24) & 0xf;
	pixel2 = (nibbleStrip >> 20) & 0xf;
	pixel3 = (nibbleStrip >> 16) & 0xf;
	pixel4 = (nibbleStrip >> 12) & 0xf;
	pixel5 = (nibbleStrip >> 8) & 0xf;
	pixel6 = (nibbleStrip >> 4) & 0xf;
	pixel7 = (nibbleStrip >> 0) & 0xf;

	bitplaneStrip.planes[0] = (pixel0 & BIT_0) << 7 | (pixel1 & BIT_0) << 6 | (pixel2 & BIT_0) << 5 | (pixel3 & BIT_0) << 4 | (pixel4 & BIT_0) << 3 | (pixel5 & BIT_0) << 2 | (pixel6 & BIT_0) << 1 | (pixel7 & BIT_0) << 0;
	bitplaneStrip.planes[1] = (pixel0 & BIT_1) << 6 | (pixel1 & BIT_1) << 5 | (pixel2 & BIT_1) << 4 | (pixel3 & BIT_1) << 3 | (pixel4 & BIT_1) << 2 | (pixel5 & BIT_1) << 1 | (pixel6 & BIT_1) << 0 | (pixel7 & BIT_1) >> 1;
	bitplaneStrip.planes[2] = (pixel0 & BIT_2) << 5 | (pixel1 & BIT_2) << 4 | (pixel2 & BIT_2) << 3 | (pixel3 & BIT_2) << 2 | (pixel4 & BIT_2) << 1 | (pixel5 & BIT_2) << 0 | (pixel6 & BIT_2) >> 1 | (pixel7 & BIT_2) >> 2;
	bitplaneStrip.planes[3] = (pixel0 & BIT_3) << 4 | (pixel1 & BIT_3) << 3 | (pixel2 & BIT_3) << 2 | (pixel3 & BIT_3) << 1 | (pixel4 & BIT_3) << 0 | (pixel5 & BIT_3) >> 1 | (pixel6 & BIT_3) >> 2 | (pixel7 & BIT_3) >> 3;

	return bitplaneStrip;
}

