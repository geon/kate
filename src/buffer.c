#include "buffer.h"
#include "platform/dos/ega.h"
#include "strip_coord.h"
#include "coord_conversion.h"

#include <stdlib.h>
#include <assert.h>


typedef struct BufferStruct {
	EgaScrollCoord scroll;
	bool alternateBuffer;
	DirtyBackgroundStrips dirtyBackgroundStrips[2];
} BufferStruct;


Buffer makeBuffer () {
	Buffer buffer = malloc(sizeof(BufferStruct));
	assert(buffer);

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


#define EGA_SCREEN_NUM_COLUMNS 80
#define EGA_SCREEN_HEIGHT 350
void markBordersAsDirty (Buffer buffer, PixelCoord scroll, Map map) {
	int16_t xChange = scroll.x/8 - buffer->scroll.column;
	int16_t yChange = scroll.y - buffer->scroll.y;

	// Mark the new edge strips as dirty.

	// Mark the bottom/top edge.
	{
		uint16_t beginY = yChange < 0
			? 0
			: max(0, EGA_SCREEN_HEIGHT - 1 - yChange);
		uint16_t endY = yChange < 0
			? -yChange
			: EGA_SCREEN_HEIGHT;
		uint16_t dirtyColumn, dirtyY;
		for (dirtyY=beginY; dirtyY<endY; ++dirtyY) {
			for (dirtyColumn=0; dirtyColumn<=EGA_SCREEN_NUM_COLUMNS; ++dirtyColumn) {
				StripCoord dirtyCoord;
				dirtyCoord.column = dirtyColumn + scroll.x/8;
				dirtyCoord.y = dirtyY + scroll.y;
				dirtyBackgroundStripsMark(
					buffer->dirtyBackgroundStrips[0],
					dirtyCoord,
					getMapStripAtWorldCoord(map, dirtyCoord)
				);
				dirtyBackgroundStripsMark(
					buffer->dirtyBackgroundStrips[1],
					dirtyCoord,
					getMapStripAtWorldCoord(map, dirtyCoord)
				);
			}
		}
	}

	// Mark the left/right edge, where it does not overlap the top/bottom.
	{
		// The range in Y not covered above.
		uint16_t beginY = yChange < 0
			? -yChange
			: 0;
		uint16_t endY = yChange < 0
			? EGA_SCREEN_HEIGHT
			: max(0, EGA_SCREEN_HEIGHT - 1 - yChange);

		uint16_t beginColumn = xChange < 0
			? 0
			: max(0, EGA_SCREEN_NUM_COLUMNS - 1 - xChange);
		uint16_t endColumn = xChange < 0
			? -xChange
			: EGA_SCREEN_NUM_COLUMNS;
		uint16_t dirtyColumn, dirtyY;
		for (dirtyY=beginY; dirtyY<endY; ++dirtyY) {
			for (dirtyColumn=beginColumn; dirtyColumn<=endColumn; ++dirtyColumn) {
				StripCoord dirtyCoord;
				dirtyCoord.column = dirtyColumn + scroll.x/8;
				dirtyCoord.y = dirtyY + scroll.y;
				dirtyBackgroundStripsMark(
					buffer->dirtyBackgroundStrips[0],
					dirtyCoord,
					getMapStripAtWorldCoord(map, dirtyCoord)
				);
				dirtyBackgroundStripsMark(
					buffer->dirtyBackgroundStrips[1],
					dirtyCoord,
					getMapStripAtWorldCoord(map, dirtyCoord)
				);
			}
		}
	}
}


void switchBuffer (Buffer buffer, PixelCoord scroll, Map map) {
	// Calculate the current buffer offset and fractional strip panning.
	StripCoord stripScroll = makeStripCoordFromEgaScrollCoord(buffer->scroll);
	uint16_t bufferIndex = bufferMapWorldCoordToBufferIndex(
		buffer,
		stripScroll
	);

	// Send the EGA command.
	setBufferOffset(bufferIndex, buffer->scroll.restX);

	// Update internally.
	buffer->alternateBuffer = !buffer->alternateBuffer;
	buffer->scroll = makeEgaScrollCoordFromPixelCoord(scroll);

	markBordersAsDirty(buffer, scroll, map);
}


uint16_t bufferIndexStart (uint16_t column, uint16_t y, bool alternateBuffer) {
	return y*EGA_BUFFER_NUM_COLUMNS + column + (alternateBuffer ? EGA_BUFFER_SIZE : 0);
}


uint16_t bufferMapWorldCoordToBufferIndex (Buffer buffer, StripCoord worldCoord) {
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


uint16_t bufferStaticMapBufferCoordToBufferIndex (EgaScrollCoord bufferScroll, bool alternateBuffer, StripCoord bufferCoord) {
	return
		bufferIndexStart(bufferScroll.column, bufferScroll.y, alternateBuffer) +
		bufferCoord.y*EGA_BUFFER_NUM_COLUMNS + bufferCoord.column;
}


StripCoord bufferMapBufferIndexToWorldCoord (Buffer buffer, uint16_t bufferIndex) {
	return bufferStaticMapBufferCoordToWorldCoord(
		buffer->scroll,
		bufferStaticMapBufferIndexToBufferCoord(
			buffer->scroll,
			buffer->alternateBuffer,
			bufferIndex
		)
	);
}


StripCoord bufferStaticMapBufferIndexToBufferCoord (EgaScrollCoord bufferScroll, bool alternateBuffer, uint16_t bufferIndex) {
	uint16_t bufferStripIndexStart = bufferIndexStart(bufferScroll.column, bufferScroll.y, alternateBuffer);
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


IndicesByStripTableRow * bufferDirtyBackgroundStripsBegin(Buffer buffer) {
    return  dirtyBackgroundStripsBegin(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0]);
}


IndicesByStripTableRow * bufferDirtyBackgroundStripsEnd(Buffer buffer) {
    return  dirtyBackgroundStripsEnd(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0]);
}


void bufferClearDirtyBackgroundStrips (Buffer buffer) {
	dirtyBackgroundStripsClear(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0]);
}


void bufferMarkDirtyBackgroundStrips(Buffer buffer, StripCoord coord, BitplaneStrip cleanStrip) {
	dirtyBackgroundStripsMark(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0], coord, cleanStrip);
}
