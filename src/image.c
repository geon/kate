#include "image.h"
#include "platform/dos/ega.h"
#include "platform/dos/bitplane_strip.h"
#include <stdlib.h>

void freeImage (Image image) {
	free(image.pixels);
	free(image.mask);
}

void drawImage(Image image, unsigned int posX, unsigned int posY) {
    unsigned int y, column;
    BitplaneStrip strip;
    unsigned int posXColumn, posXRest;
    BitplaneStrip stripShiftedA, stripShiftedB;
    unsigned char shiftMask;
    unsigned int stripIndex;

    for (y = 0; y < image.height; ++y) {
        for (column=0; column<image.numColumns; ++column) {
            stripIndex = column + (image.upsideDown ? (image.height - 1 - y) : y)*image.numColumns;
            strip = makeBitplaneStrip(image.pixels[stripIndex]);

            posXColumn = posX/8;
            posXRest = posX%8;

            stripShiftedA.planes[0] = strip.planes[0] >> posXRest;
            stripShiftedA.planes[1] = strip.planes[1] >> posXRest;
            stripShiftedA.planes[2] = strip.planes[2] >> posXRest;
            stripShiftedA.planes[3] = strip.planes[3] >> posXRest;

            stripShiftedB.planes[0] = strip.planes[0] << (8 - posXRest);
            stripShiftedB.planes[1] = strip.planes[1] << (8 - posXRest);
            stripShiftedB.planes[2] = strip.planes[2] << (8 - posXRest);
            stripShiftedB.planes[3] = strip.planes[3] << (8 - posXRest);

            // TODO: Combine the adjecent strips, to avoid double writes.
            shiftMask = image.mask[stripIndex] >> posXRest;
            drawStrip(column + posXColumn, y + posY, stripShiftedA, shiftMask);
            shiftMask = image.mask[stripIndex] << (8 - posXRest);
            drawStrip(column + posXColumn + 1, y + posY, stripShiftedB, shiftMask);
        }
    }
}
