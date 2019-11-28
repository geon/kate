#include "dirty_background_strips.h";

#include "platform/dos/ega.h"

#include <stdbool.h>
#include <stdlib.h>


DirtyBackgroundStrips makeDirtyBackgroundStrips () {
    DirtyBackgroundStrips dirtyBackgroundStrips;
    unsigned long int i;

    dirtyBackgroundStrips = malloc(EGA_PLANE_SIZE * sizeof(bool));

    // The background has not been rendered yet, so it is by definition completely dirty.
    for (i=0; i<EGA_PLANE_SIZE; ++i) {
        dirtyBackgroundStrips[i] = true;
    }

    return dirtyBackgroundStrips;
}


void freeDirtyBackgroundStrips (DirtyBackgroundStrips dirtyBackgroundStrips) {
    free(dirtyBackgroundStrips);
}
