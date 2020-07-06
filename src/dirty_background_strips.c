#include "dirty_background_strips.h"

#include "world.h"
#include "buffer.h"

#include <stdbool.h>
#include <stdlib.h>


typedef struct DirtyBackgroundStripsStruct {
	unsigned long int numCoords;
	StripCoord coords[EGA_PLANE_SIZE];
} DirtyBackgroundStripsStruct;


DirtyBackgroundStrips makeDirtyBackgroundStrips (bool alternateBuffer) {
	unsigned long int column;
	unsigned long int y;

	DirtyBackgroundStrips dirtyBackgroundStrips;
	dirtyBackgroundStrips = malloc(sizeof(DirtyBackgroundStripsStruct));
	dirtyBackgroundStrips->numCoords = 0;

	// The background has not been rendered yet, so it is by definition completely dirty.
	for (column=0; column<EGA_BUFFER_NUM_COLUMNS_DEFAULT; ++column) {
		for (y=0; y<EGA_BUFFER_NUM_ROWS_DEFAULT; ++y) {
			StripCoord coord;
			coord.column = column;
			coord.y = y;
			dirtyBackgroundStripsMark(dirtyBackgroundStrips, coord);
		}
	}

	return dirtyBackgroundStrips;
}


void freeDirtyBackgroundStrips (DirtyBackgroundStrips dirtyBackgroundStrips) {
	free(dirtyBackgroundStrips);
}

unsigned long int getDirtyBackgroundStripsNumCoords (DirtyBackgroundStrips dirtyBackgroundStrips) {
	return dirtyBackgroundStrips->numCoords;
}

StripCoord *getDirtyBackgroundStripsCoords (DirtyBackgroundStrips dirtyBackgroundStrips) {
	return dirtyBackgroundStrips->coords;
}

void dirtyBackgroundStripsClear (DirtyBackgroundStrips dirtyBackgroundStrips) {
	dirtyBackgroundStrips->numCoords = 0;
}

void dirtyBackgroundStripsMark (DirtyBackgroundStrips dirtyBackgroundStrips, StripCoord coord) {
	dirtyBackgroundStrips->coords[dirtyBackgroundStrips->numCoords++] = coord;
}
