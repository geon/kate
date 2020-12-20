// https://stackoverflow.com/a/47785639/446536

#include "bmp.h"
#include "image_struct.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

const int bytesPerPixel = 3; /// red, green, blue
#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

char *errorfileOpen = "Could not open the image file.";
char *errorMessagePartialStrip = "All images should be a multiple of 8 pixels wide.";
char *errorMessageNumBits = "Only 4-bits-per-pixel images are supported.";
char *errorMessageCompression = "Compressed images are not supported.";
char *errorMessagePalette = "Max supported palette length is 16 colors.";

uint32_t parseUnsignedLongInt (unsigned char *data) {
	return
		((uint32_t) data[0]) |
		((uint32_t) data[1]) << 8 |
		((uint32_t) data[2]) << 16 |
		((uint32_t) data[3]) << 24;
}

uint32_t parseLongInt (unsigned char *data) {
	uint32_t bits =
		((uint32_t) data[0]) |
		((uint32_t) data[1]) << 8 |
		((uint32_t) data[2]) << 16 |
		((uint32_t) data[3]) << 24;

	return *(int32_t*) &bits;
}

uint16_t parseUnsignedInt (unsigned char *data) {
	return 
		((uint32_t) data[0]) |
		((uint32_t) data[1]) << 8;
}

uint32_t reverseBytes (uint32_t bits) {
	unsigned char *data = (unsigned char *) &bits;

	return 
		((uint32_t) data[3]) |
		((uint32_t) data[2]) << 8 |
		((uint32_t) data[1]) << 16 |
		((uint32_t) data[0]) << 24;
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

unsigned char makeMask (uint32_t nibbleStrip) {
	unsigned char i;
	unsigned char mask;

	mask = 0;
	for (i=0; i<8; ++i) {
		mask |= (nibbleStrip & (((uint32_t) 0xf) << (i*4))) ? (1 << i) : 0;
	}

	return mask;
}

// // Convert to rgb
// red   = 85 * (((ega >> 1) & 2) | (ega >> 5) & 1)
// green = 85 * (( ega       & 2) | (ega >> 4) & 1)
// blue  = 85 * (((ega << 1) & 2) | (ega >> 3) & 1)

#define BYTES_PER_COLOR_ENTRY 4
Image loadBmp(char* imageFilePath, bool firstColorIsTransparency, char** errorMessage) {
	Image image = makeImage();
	unsigned char fileHeader[FILE_HEADER_SIZE];
	unsigned char infoHeader[INFO_HEADER_SIZE];
	uint32_t width;
	int32_t height;
	uint16_t bitsPerPixel;
	uint32_t compression;
	uint32_t numColors;
	uint16_t i;
	unsigned char paletteData[16 * BYTES_PER_COLOR_ENTRY];
	uint32_t  dataSize;
	uint32_t  maskSize;

	FILE* imageFile = fopen(imageFilePath, "rb");
	if (!imageFile) {
		if (errorMessage) {
			*errorMessage = errorfileOpen;
		}
		return NULL;
	}

	// TODO: Replace with fseek?
	fread(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

	fread(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

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
		if (errorMessage) {
			*errorMessage = errorMessagePartialStrip;
		}
		return NULL;
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
		if (errorMessage) {
			*errorMessage = errorMessageNumBits;
		}
		return NULL;
	}

	compression = parseUnsignedLongInt(&infoHeader[16]);
	image->numColumns = width / 8;

	if(compression) {
		if (errorMessage) {
			*errorMessage = errorMessageCompression;
		}
		return NULL;
	}

	numColors = parseUnsignedLongInt(&infoHeader[32]);
	if (!numColors) {
		numColors = 1 << bitsPerPixel;
	}

	if (numColors > 16) {
		if (errorMessage) {
			*errorMessage = errorMessagePalette;
		}
		return NULL;
	}

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
	assert(image->pixels);
	fread(image->pixels, dataSize, 1, imageFile);
	fclose(imageFile);

	// Shuffle the bytes to match the pixel strip ordering.
	for (i=0; i<image->numColumns * image->height; ++i) {
		image->pixels[i] = reverseBytes(image->pixels[i]);
	}

	maskSize = image->numColumns * image->height;
	image->mask = malloc(maskSize);
	assert(image->mask);

	for (i=0; i<maskSize; ++i) {
		image->mask[i] = firstColorIsTransparency
			? makeMask(image->pixels[i])
			: 0xff;
	}

	return image;
}
