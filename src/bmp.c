// https://stackoverflow.com/a/47785639/446536

#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>

const int bytesPerPixel = 3; /// red, green, blue
const int fileHeaderSize = 14;
const int infoHeaderSize = 40;

unsigned long int parseUnsignedLongInt (unsigned char *data) {
	return 
		((unsigned long int) data[0]) |
		((unsigned long int) data[1]) << 8 |
		((unsigned long int) data[2]) << 16 |
		((unsigned long int) data[3]) << 24;
}

unsigned long int parseLongInt (unsigned char *data) {
	unsigned long int bits = 
		((unsigned long int) data[0]) |
		((unsigned long int) data[1]) << 8 |
		((unsigned long int) data[2]) << 16 |
		((unsigned long int) data[3]) << 24;

	return *(long int*) &bits;
}

unsigned int parseUnsignedInt (unsigned char *data) {
	return 
		((unsigned long int) data[0]) |
		((unsigned long int) data[1]) << 8;
}

unsigned long int reverseBytes (unsigned long int bits) {
	unsigned char *data = (unsigned char *) &bits;

	return 
		((unsigned long int) data[3]) |
		((unsigned long int) data[2]) << 8 |
		((unsigned long int) data[1]) << 16 |
		((unsigned long int) data[0]) << 24;
}

unsigned char rgbToEga (unsigned char red, unsigned char green, unsigned char blue) {
	unsigned char red2Bit = red/85;
	unsigned char green2Bit = green/85;
	unsigned char blue2Bit = blue/85;

	return
		((red2Bit & 1) << 5) |
		((green2Bit & 1) << 4) |
		((blue2Bit & 1) << 3) |
		(((red2Bit >> 1) & 1) << 2) |
		(((green2Bit >> 1) & 1) << 1) |
		(((blue2Bit >> 1) & 1) << 0);
}

// // Convert to rgb
// red   = 85 * (((ega >> 1) & 2) | (ega >> 5) & 1)
// green = 85 * (( ega       & 2) | (ega >> 4) & 1)
// blue  = 85 * (((ega << 1) & 2) | (ega >> 3) & 1)

#define BYTES_PER_COLOR_ENTRY 4
bool loadBmp(Image *image, char* imageFilePath){

	// TODO: Aremove allocation. Just use an array on the stack.
	unsigned char *fileHeader = malloc(fileHeaderSize);
	unsigned char *infoHeader = malloc(infoHeaderSize);
	unsigned long int width;
	long int height;
	unsigned int bitsPerPixel;
	unsigned long int compression;
	unsigned long int numColors;
	unsigned int i;
	unsigned char paletteData[16 * BYTES_PER_COLOR_ENTRY];
	unsigned long int  dataSize;

	FILE* imageFile = fopen(imageFilePath, "rb");

	// TODO: Replace with fseek?
	fread(fileHeader, 1, fileHeaderSize, imageFile);
	free(fileHeader);

	fread(infoHeader, 1, infoHeaderSize, imageFile);

	// static unsigned char infoHeader[] = {
	//     0,0,0,0, /// header size
	//     0,0,0,0, /// image width
	//     0,0,0,0, /// image height
	//     0,0, /// number of color planes
	//     0,0, /// bits per pixel
	//     0,0,0,0, /// compression
	//     0,0,0,0, /// image size
	//     0,0,0,0, /// horizontal resolution
	//     0,0,0,0, /// vertical resolution
	//     0,0,0,0, /// colors in color table
	//     0,0,0,0, /// important color count
	// };

	width = parseUnsignedLongInt(&infoHeader[4]);
	image->numColumns = width / 8;

	// All images should be a multiple of 8 pixels wide.
	if (width % 8) {
		return false;
	}

	height = parseLongInt(&infoHeader[8]);

	// Some images are stored upside-down. Right-side-up is sigified by a negative height.
	image->upsideDown = height >= 0;
	if(height < 0){
		height *= -1;
	}
	image->height = height;

	bitsPerPixel = parseUnsignedInt(&infoHeader[14]);
	if(bitsPerPixel != 4) {
		return false;
	}

	compression = parseUnsignedLongInt(&infoHeader[16]);
	image->numColumns = width / 8;

	if(compression) {
		return false;
	}

	numColors = parseUnsignedLongInt(&infoHeader[32]);
	if (!numColors) {
		numColors = 1 << bitsPerPixel;
	}

	if (numColors > 16) {
		return false;
	}

	free(infoHeader);
	free(infoHeader);

	fread(paletteData, numColors * BYTES_PER_COLOR_ENTRY, 1, imageFile);

	for (i=0; i<numColors; ++i) {
		image->palette[i] = rgbToEga(
			paletteData[i * BYTES_PER_COLOR_ENTRY + 2],
			paletteData[i * BYTES_PER_COLOR_ENTRY + 1],
			paletteData[i * BYTES_PER_COLOR_ENTRY + 0]
		);
	}
	// Blank out unused entries.
	for (i=numColors; i<16; ++i) {
		image->palette[i] = i;
	}

	dataSize = width * image->height / 2;

	image->pixels = malloc(dataSize);
	fread(image->pixels, dataSize, 1, imageFile);
	fclose(imageFile);

	// Shuffle the bytes to match the pixel strip ordering.
	for (i=0; i<image->numColumns * image->height; ++i) {
		image->pixels[i] = reverseBytes(image->pixels[i]);
	}

	return true;
}
