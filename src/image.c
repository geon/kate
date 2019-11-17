#include "image.h"
#include "platform/dos/ega.h"
#include <stdlib.h>

void freeImage (Image image) {
	free(image.pixels);
}

void drawImage(Image image, unsigned int posX, unsigned int posY) {
    unsigned int y, column;
    long unsigned int strip;
    unsigned int posXColumn, posXRest;
    unsigned long int stripShiftedA, stripShiftedB;
    unsigned long int shiftMask;

    for (y = 0; y < image.height; ++y) {
        for (column=0; column<image.numColumns; ++column) {
            strip = makeBitplaneStrip(image.pixels[column + (image.upsideDown ? (image.height - 1 - y) : y)*image.numColumns]);

            posXColumn = posX/8;
            posXRest = posX%8;

            shiftMask =
                (((unsigned long int) 0xff >> posXRest) << 24) |
                (((unsigned long int) 0xff >> posXRest) << 16) |
                (((unsigned long int) 0xff >> posXRest) << 8) |
                (((unsigned long int) 0xff >> posXRest) << 0);

            stripShiftedA = (strip >> posXRest) & shiftMask;
            stripShiftedB = (strip << (8 - posXRest)) & (~shiftMask);

            // TODO: Combine the adjecent strips, to avoid double writes.
            drawStrip(column + posXColumn, y + posY, stripShiftedA, (shiftMask) & 0xff);
            drawStrip(column + posXColumn + 1, y + posY, stripShiftedB, (~shiftMask) & 0xff);
        }
    }
}
