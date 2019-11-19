#include "dirty_background_strips.h";

#include <stdbool.h>
#include <stdlib.h>

#define NUM_BACKGROUND_STRIPS 640/8*350


DirtyBackgroundStrips makeDirtyBackgroundStrips () {
    DirtyBackgroundStrips dirtyBackgroundStrips;
    unsigned int i;

    dirtyBackgroundStrips = malloc(NUM_BACKGROUND_STRIPS * sizeof(bool));

    // The background has not been rendered yet, so it is by definition completely dirty.
    for (i=0; i<NUM_BACKGROUND_STRIPS; ++i) {
        dirtyBackgroundStrips[i] = true;
    }

    return dirtyBackgroundStrips;
}


void freeDirtyBackgroundStrips (DirtyBackgroundStrips dirtyBackgroundStrips) {
    free(dirtyBackgroundStrips);
}
