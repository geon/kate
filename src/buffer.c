#include "buffer.h"
#include "platform/dos/ega.h"
#include "strip_coord.h"
#include "coord_conversion.h"
#include "dirty_background_strips.h"
#include "uint16_vector.h"

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
	// Mark the new edge strips as dirty.

	{
		StripCoord topLeft, bottomRight;
		// Mark the top/bottom edge.
		topLeft.column = scroll.x/8;
		bottomRight.column = topLeft.column + EGA_SCREEN_NUM_COLUMNS;
		if (scroll.y > buffer->scroll.y) {
			// Bottom
			topLeft.y = buffer->scroll.y + EGA_SCREEN_HEIGHT;
			bottomRight.y = scroll.y + EGA_SCREEN_HEIGHT;
		} else {
			// Top
			topLeft.y = scroll.y;
			bottomRight.y = buffer->scroll.y;
		}

		// Mark in both buffers.
		// TODO: store the old scroll for both buffers.
		dirtyBackgroundStripsMarkRectangle(
			buffer->dirtyBackgroundStrips[0],
			topLeft,
			bottomRight,
			map,
			buffer,
			false
		);
		dirtyBackgroundStripsMarkRectangle(
			buffer->dirtyBackgroundStrips[1],
			topLeft,
			bottomRight,
			map,
			buffer,
			true
		);
	}

	// TODO: Remove overlap.
	{
		uint16_t scrollColumn = (scroll.x+7)/8;
		StripCoord topLeft, bottomRight;
		// Mark the left/right edge, where it does not overlap the top/bottom.
		topLeft.y = scroll.y;
		bottomRight.y = topLeft.y + EGA_SCREEN_HEIGHT;
		if (scrollColumn > buffer->scroll.column) {
			// Right
			topLeft.column = buffer->scroll.column + EGA_SCREEN_NUM_COLUMNS;
			bottomRight.column = scrollColumn + EGA_SCREEN_NUM_COLUMNS;
		} else {
			// Left
			topLeft.column = buffer->scroll.column;
			bottomRight.column = scrollColumn;
		}

		dirtyBackgroundStripsMarkRectangle(
			buffer->dirtyBackgroundStrips[0],
			topLeft,
			bottomRight,
			map,
			buffer,
			false
		);
		dirtyBackgroundStripsMarkRectangle(
			buffer->dirtyBackgroundStrips[1],
			topLeft,
			bottomRight,
			map,
			buffer,
			true
		);
	}
}


void switchBuffer (Buffer buffer, PixelCoord scrollOfNextFrame, Map map) {
	// Calculate the current buffer offset and fractional strip panning.
	StripCoord stripScroll = makeStripCoordFromEgaScrollCoord(buffer->scroll);
	uint16_t bufferIndex = bufferMapWorldCoordToBufferIndexInAlternateBuffer(
		stripScroll,
		buffer->scroll,
		buffer->alternateBuffer
	);
	const uint8_t  restX =  buffer->scroll.restX;

	markBordersAsDirty(buffer, scrollOfNextFrame, map);

	// Update internally.
	buffer->alternateBuffer = !buffer->alternateBuffer;
	buffer->scroll = makeEgaScrollCoordFromPixelCoord(scrollOfNextFrame);

	// Send the EGA command.
	setBufferOffset(bufferIndex, restX);
}


uint16_t bufferIndexStart (uint16_t column, uint16_t y, bool alternateBuffer) {
	return y*EGA_BUFFER_NUM_COLUMNS + column + (alternateBuffer ? EGA_BUFFER_SIZE : 0);
}


uint16_t bufferMapWorldCoordToBufferIndexInAlternateBuffer (StripCoord worldCoord, EgaScrollCoord bufferScroll, bool bufferAlternateBuffer) {
	return  bufferStaticMapBufferCoordToBufferIndex(
		bufferScroll,
		bufferAlternateBuffer,
		bufferStaticMapWorldCoordToBufferCoord(
			bufferScroll,
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


uint16_t * bufferDirtyBackgroundStripsBegin (Buffer buffer) {
    return  dirtyBackgroundStripsBegin(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0]);
}


uint16_t * bufferDirtyBackgroundStripsEnd (Buffer buffer) {
    return  dirtyBackgroundStripsEnd(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0]);
}


Uint16Vector bufferGetDirtyBufferIndicesByStripIndex (Buffer buffer) {
	return dirtyBackgroundStripsGetDirtyBufferIndicesByStripIndex(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0]);
}


void bufferClearDirtyBackgroundStrips (Buffer buffer) {
	dirtyBackgroundStripsClear(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0]);
}


// void bufferMarkDirtyBackgroundStrips(Buffer buffer, StripCoord coord, uint16_t cleanStripIndex) {
// 	dirtyBackgroundStripsMark(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0], coord, cleanStripIndex);
// }

void bufferMarkRectangleAsDirty (Buffer buffer, StripCoord topLeftWorldCoord, StripCoord bottomRightWorldCoord, Map map) {
	dirtyBackgroundStripsMarkRectangle(buffer->dirtyBackgroundStrips[buffer->alternateBuffer ? 1 : 0], topLeftWorldCoord, bottomRightWorldCoord, map, buffer, buffer->alternateBuffer);
}
