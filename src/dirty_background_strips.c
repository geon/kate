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

	{
		// The background has not been rendered yet, so it is by definition completely dirty.
		uint32_t column;
		uint32_t y;
		for (column=0; column<EGA_BUFFER_NUM_COLUMNS_DEFAULT; ++column) {
			for (y=0; y<EGA_BUFFER_NUM_ROWS_DEFAULT; ++y) {
				BitplaneStrip strip;
				StripCoord coord;

				coord.column = column;
				coord.y = y;
				// TODO: getMapStripAtWorldCoord(map, worldCoord)
				strip = makeBitplaneStrip(0x18181818);

				dirtyBackgroundStripsMark(dirtyBackgroundStrips, coord, strip);
			}
		}
	}

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


void dirtyBackgroundStripsMark (DirtyBackgroundStrips dirtyBackgroundStrips, StripCoord coord, BitplaneStrip cleanStrip) {
	indicesByStripTableAdd(dirtyBackgroundStrips->coordsByStrip, cleanStrip, coord);
}

void dirtyBackgroundStripsMarkRectangle (DirtyBackgroundStrips dirtyBackgroundStrips, StripCoord topLeftWorldCoord, StripCoord bottomRightWorldCoord, Map map) {
	uint16_t y, column;
	for (y=topLeftWorldCoord.y; y<bottomRightWorldCoord.y; ++y) {
		for (column=topLeftWorldCoord.column; column<bottomRightWorldCoord.column; ++column) {
			StripCoord worldCoord;
			worldCoord.column = column;
			worldCoord.y = y;
			dirtyBackgroundStripsMark(dirtyBackgroundStrips, worldCoord,  getMapStripAtWorldCoord(map, worldCoord));
		}
	}
}
