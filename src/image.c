#include "image.h"
#include "platform/dos/ega.h"
#include <stdlib.h>

void freeImage (Image image) {
	free(image.pixels);
}

void drawImage(Image image, unsigned int posX, unsigned int posY) {
    unsigned int y, column;
    long unsigned int strip;

    for (y = 0; y < image.height; ++y) {
        for (column=0; column<image.numColumns; ++column) {
            strip = makeBitplaneStrip(image.pixels[column + (image.upsideDown ? (image.height - 1 - y) : y)*image.numColumns]);
            drawStrip(column + posX/8, y + posY, strip, 0xFF);
        }
    }
}
