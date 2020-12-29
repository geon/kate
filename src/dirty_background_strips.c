#include "dirty_background_strips.h"

#include "world.h"
#include "buffer.h"

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>


typedef struct DirtyBackgroundStripsStruct {
	IndicesByStripTable coordsByStrip;
} DirtyBackgroundStripsStruct;


DirtyBackgroundStrips makeDirtyBackgroundStrips (bool alternateBuffer) {
	DirtyBackgroundStrips dirtyBackgroundStrips;
	dirtyBackgroundStrips = malloc(sizeof(DirtyBackgroundStripsStruct));
	assert(dirtyBackgroundStrips);
	dirtyBackgroundStrips->coordsByStrip = makeIndicesByStripTable(64);

	return dirtyBackgroundStrips;
}


void freeDirtyBackgroundStrips (DirtyBackgroundStrips dirtyBackgroundStrips) {
	free(dirtyBackgroundStrips);
}


IndicesByStripTableRow* dirtyBackgroundStripsBegin (DirtyBackgroundStrips dirtyBackgroundStrips) {
	return indicesByStripTableBegin(dirtyBackgroundStrips->coordsByStrip);
}


IndicesByStripTableRow* dirtyBackgroundStripsEnd (DirtyBackgroundStrips dirtyBackgroundStrips) {
	return indicesByStripTableEnd(dirtyBackgroundStrips->coordsByStrip);
}


void dirtyBackgroundStripsClear (DirtyBackgroundStrips dirtyBackgroundStrips) {
	indicesByStripTableClear(dirtyBackgroundStrips->coordsByStrip);
}


void dirtyBackgroundStripsMark (DirtyBackgroundStrips dirtyBackgroundStrips, StripCoord coord, uint16_t cleanStripIndex) {
	indicesByStripTableAdd(dirtyBackgroundStrips->coordsByStrip, cleanStripIndex, coord);
}

void dirtyBackgroundStripsMarkRectangle (DirtyBackgroundStrips dirtyBackgroundStrips, StripCoord topLeftWorldCoord, StripCoord bottomRightWorldCoord, Map map) {
	uint16_t y, column;
	for (y=topLeftWorldCoord.y; y<bottomRightWorldCoord.y; ++y) {
		for (column=topLeftWorldCoord.column; column<bottomRightWorldCoord.column; ++column) {
			StripCoord worldCoord;
			worldCoord.column = column;
			worldCoord.y = y;

			dirtyBackgroundStripsMark(
				dirtyBackgroundStrips,
				worldCoord,
				getMapStripAtWorldCoord(map, worldCoord)
			);
		}
	}
}
