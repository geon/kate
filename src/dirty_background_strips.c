#include "dirty_background_strips.h";

#include "platform/dos/ega.h"

#include <stdbool.h>
#include <stdlib.h>


typedef struct DirtyBackgroundStripsStruct {
	unsigned long int numIndices;
	unsigned short int indices[EGA_PLANE_SIZE];
} DirtyBackgroundStripsStruct;


DirtyBackgroundStrips makeDirtyBackgroundStrips () {
	DirtyBackgroundStrips dirtyBackgroundStrips;
	unsigned long int i;

	dirtyBackgroundStrips = malloc(sizeof(DirtyBackgroundStripsStruct));

	// The background has not been rendered yet, so it is by definition completely dirty.
	dirtyBackgroundStrips->numIndices = EGA_PLANE_SIZE;
	for (i=0; i<EGA_PLANE_SIZE; ++i) {
		dirtyBackgroundStrips->indices[i] = i;
	}

	return dirtyBackgroundStrips;
}


void freeDirtyBackgroundStrips (DirtyBackgroundStrips dirtyBackgroundStrips) {
	free(dirtyBackgroundStrips);
}

unsigned long int getDirtyBackgroundStripsNumIndices (DirtyBackgroundStrips dirtyBackgroundStrips) {
	return dirtyBackgroundStrips->numIndices;
}

unsigned short int *getDirtyBackgroundStripsIndices (DirtyBackgroundStrips dirtyBackgroundStrips) {
	return dirtyBackgroundStrips->indices;
}

void clearDirtyBackgroundStrips (DirtyBackgroundStrips dirtyBackgroundStrips) {
	dirtyBackgroundStrips->numIndices = 0;
}

void markDirtyBackgroundStrips (DirtyBackgroundStrips dirtyBackgroundStrips, unsigned short int index) {
	dirtyBackgroundStrips->indices[dirtyBackgroundStrips->numIndices++] = index;
}
