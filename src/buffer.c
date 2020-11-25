#include "buffer.h"
#include "platform/dos/ega.h"
#include "strip_coord.h"
#include "coord_conversion.h"

#include <stdlib.h>


typedef struct BufferStruct {
	EgaScrollCoord scroll;
	bool alternateBuffer;
	DirtyBackgroundStrips dirtyBackgroundStrips[2];
} BufferStruct;


Buffer makeBuffer () {
	Buffer buffer = malloc(sizeof(BufferStruct));

	buffer->alternateBuffer = false;
	buffer->scroll.column = 0;
	buffer->scroll.restX = 0;
	buffer->scroll.y = 0;
	buffer->dirtyBackgroundStrips[0] = makeDirtyBackgroundStrips(0);
	buffer->dirtyBackgroundStrips[1] = makeDirtyBackgroundStrips(1);

	return buffer;
}


void freeBuffer (Buffer buffer) {
	freeDirtyBackgroundStrips(buffer->dirtyBackgroundStrips[1]);
	freeDirtyBackgroundStrips(buffer->dirtyBackgroundStrips[0]);
	free(buffer);
}


// TODO: Move code from `rendererSetBufferOffset` here?
void switchBuffer (Buffer buffer) {
	buffer->alternateBuffer = !buffer->alternateBuffer;
}


#define EGA_SCREEN_NUM_COLUMNS 80
#define EGA_SCREEN_HEIGHT 350
void setBufferScroll (Buffer buffer, PixelCoord scroll) {
	int xChange = scroll.x/8 - buffer->scroll.column;
	int yChange = scroll.y - buffer->scroll.y;

	// Actually scroll.
	buffer->scroll = makeEgaScrollCoordFromPixelCoord(scroll);

	// Mark the new edge strips as dirty.

	// Mark the bottom/top edge.
	{
		unsigned short int beginY = yChange < 0
			? 0
			: max(0, EGA_SCREEN_HEIGHT - 1 - yChange);
		unsigned short int endY = yChange < 0
			? -yChange
			: EGA_SCREEN_HEIGHT;
		unsigned short int dirtyColumn, dirtyY;
		for (dirtyY=beginY; dirtyY<endY; ++dirtyY) {
			for (dirtyColumn=0; dirtyColumn<=EGA_SCREEN_NUM_COLUMNS; ++dirtyColumn) {
				StripCoord dirtyCoord;
				dirtyCoord.column = dirtyColumn + scroll.x/8;
				dirtyCoord.y = dirtyY + scroll.y;
				dirtyBackgroundStripsMark(
					buffer->dirtyBackgroundStrips[0],
					dirtyCoord
				);
				dirtyBackgroundStripsMark(
					buffer->dirtyBackgroundStrips[1],
					dirtyCoord
				);
			}
		}
	}

	// Mark the left/right edge, where it does not overlap the top/bottom.
	{
		// The range in Y not covered above.
		unsigned short int beginY = yChange < 0
			? -yChange
			: 0;
		unsigned short int endY = yChange < 0
			? EGA_SCREEN_HEIGHT
			: max(0, EGA_SCREEN_HEIGHT - 1 - yChange);

		unsigned short int beginColumn = xChange < 0
			? 0
			: max(0, EGA_SCREEN_NUM_COLUMNS - 1 - xChange);
		unsigned short int endColumn = xChange < 0
			? -xChange
			: EGA_SCREEN_NUM_COLUMNS;
		unsigned short int dirtyColumn, dirtyY;
		for (dirtyY=beginY; dirtyY<endY; ++dirtyY) {
			for (dirtyColumn=beginColumn; dirtyColumn<=endColumn; ++dirtyColumn) {
				StripCoord dirtyCoord;
				dirtyCoord.column = dirtyColumn + scroll.x/8;
				dirtyCoord.y = dirtyY + scroll.y;
				dirtyBackgroundStripsMark(
					buffer->dirtyBackgroundStrips[0],
					dirtyCoord
				);
				dirtyBackgroundStripsMark(
					buffer->dirtyBackgroundStrips[1],
					dirtyCoord
				);
			}
		}
	}
}


unsigned short int bufferIndexStart (unsigned short int column, unsigned short int y, bool alternateBuffer) {
	return y*EGA_BUFFER_NUM_COLUMNS + column + (alternateBuffer ? EGA_BUFFER_SIZE : 0);
}


unsigned short int bufferMapWorldCoordToBufferIndex (Buffer buffer, StripCoord worldCoord) {
	return  bufferStaticMapBufferCoordToBufferIndex(
		buffer->scroll,
		buffer->alternateBuffer,
		bufferStaticMapWorldCoordToBufferCoord(
			buffer->scroll,
			worldCoord
		)
	);
}


StripCoord bufferStaticMapWorldCoordToBufferCoord (EgaScrollCoord bufferScroll, StripCoord worldCoord) {
	StripCoord bufferCoord;
	bufferCoord.column = worldCoord.column - bufferScroll.column;
	bufferCoord.y = worldCoord.y - bufferScroll.y;
	return  bufferCoord;
}


unsigned short int bufferStaticMapBufferCoordToBufferIndex (EgaScrollCoord bufferScroll, bool alternateBuffer, StripCoord bufferCoord) {
	return
		bufferIndexStart(bufferScroll.column, bufferScroll.y, alternateBuffer) +
		bufferCoord.y*EGA_BUFFER_NUM_COLUMNS + bufferCoord.column;
}


StripCoord bufferMapBufferIndexToWorldCoord (Buffer buffer, unsigned short int bufferIndex) {
	return bufferStaticMapBufferCoordToWorldCoord(
		buffer->scroll,
		bufferStaticMapBufferIndexToBufferCoord(
			buffer->scroll,
			buffer->alternateBuffer,
			bufferIndex
		)
	);
}


StripCoord bufferStaticMapBufferIndexToBufferCoord (EgaScrollCoord bufferScroll, bool alternateBuffer, unsigned short int bufferIndex) {
	unsigned short int bufferStripIndexStart = bufferIndexStart(bufferScroll.column, bufferScroll.y, alternateBuffer);
	StripCoord bufferCoord;
	bufferCoord.column = (bufferIndex - bufferStripIndexStart) % EGA_BUFFER_NUM_COLUMNS;
	bufferCoord.y = (bufferIndex - bufferStripIndexStart) / EGA_BUFFER_NUM_COLUMNS;
	return  bufferCoord;
}


StripCoord bufferStaticMapBufferCoordToWorldCoord (EgaScrollCoord bufferScroll, StripCoord bufferCoord) {
	StripCoord worldCoord;
	worldCoord.column = bufferScroll.column + bufferCoord.column;
	worldCoord.y = bufferScroll.y + bufferCoord.y;
	return  worldCoord;
}


unsigned long int bufferGetDirtyBackgroundStripsNumCoords (Buffer buffer) {
	return getDirtyBackgroundStripsNumCoords(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0]);
}


StripCoord *bufferGetDirtyBackgroundStripsCoords (Buffer buffer) {
	return getDirtyBackgroundStripsCoords(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0]);
}


void bufferClearDirtyBackgroundStrips (Buffer buffer) {
	dirtyBackgroundStripsClear(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0]);
}


void bufferMarkDirtyBackgroundStrips(Buffer buffer, StripCoord coord) {
	dirtyBackgroundStripsMark(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0], coord);
}
