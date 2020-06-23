#include "dirty_background_strips.h";

#include "world.h"
#include "buffer.h"

#include <stdbool.h>
#include <stdlib.h>


typedef struct DirtyBackgroundStripsStruct {
	unsigned long int numIndices;
	unsigned short int indices[EGA_PLANE_SIZE];
} DirtyBackgroundStripsStruct;


DirtyBackgroundStrips makeDirtyBackgroundStrips (bool alternateBuffer) {
	DirtyBackgroundStrips dirtyBackgroundStrips;
	unsigned short startIndex;
	unsigned long int i;

	dirtyBackgroundStrips = malloc(sizeof(DirtyBackgroundStripsStruct));

	// The background has not been rendered yet, so it is by definition completely dirty.
	startIndex = alternateBuffer ? EGA_BUFFER_SIZE : 0;
	dirtyBackgroundStrips->numIndices = EGA_BUFFER_SIZE;
	for (i=0; i<dirtyBackgroundStrips->numIndices; ++i) {
		dirtyBackgroundStrips->indices[i] = startIndex + i;
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
