#include "buffer.h"
#include "platform/dos/ega.h"
#include "strip_coord.h"
#include "coord_conversion.h"

#include <stdlib.h>


typedef struct BufferStruct {
	EgaScrollCoord scroll;
	bool alternateBuffer;
	// TODO: Split into 2; one for each doubled buffer.
	DirtyBackgroundStrips dirtyBackgroundStrips;
} BufferStruct;


Buffer makeBuffer () {
	Buffer buffer = malloc(sizeof(BufferStruct));

	buffer->alternateBuffer = false;
	buffer->scroll.column = 0;
	buffer->scroll.restX = 0;
	buffer->scroll.y = 0;
	buffer->dirtyBackgroundStrips = makeDirtyBackgroundStrips();

	return buffer;
}


void freeBuffer (Buffer buffer) {
	freeDirtyBackgroundStrips(buffer->dirtyBackgroundStrips);
	free(buffer);
}


void updateBuffer (Buffer buffer) {
	// buffer->alternateBuffer = !buffer->alternateBuffer;
}


void setBufferScroll (Buffer buffer, PixelCoord scroll) {
	// TODO: Update dirty strips, using old and new scroll value.
	buffer->scroll = makeEgaScrollCoordFromPixelCoord(scroll);
}


unsigned short int bufferIndexStart (unsigned short int column, unsigned short int y, bool alternateBuffer) {
	return y*EGA_BUFFER_NUM_COLUMNS + column + (alternateBuffer ? EGA_BUFFER_SIZE : 0);
}


StripCoord bufferMapWorldCoordToBufferCoord (Buffer buffer, StripCoord worldCoord) {
	StripCoord bufferCoord;
	bufferCoord.column = worldCoord.column - buffer->scroll.column;
	bufferCoord.y = worldCoord.y - buffer->scroll.y;
	return  bufferCoord;
}


unsigned short int bufferMapBufferCoordToBufferIndex (Buffer buffer, StripCoord bufferCoord) {
	return
		bufferIndexStart(buffer->scroll.column, buffer->scroll.y, buffer->alternateBuffer) +
		bufferCoord.y*EGA_BUFFER_NUM_COLUMNS + bufferCoord.column;
}


StripCoord bufferMapBufferIndexToBufferCoord (Buffer buffer, unsigned short int bufferIndex) {
	unsigned short int bufferStripIndexStart = bufferIndexStart(buffer->scroll.column, buffer->scroll.y, buffer->alternateBuffer);
	StripCoord bufferCoord;
	bufferCoord.column = (bufferIndex - bufferStripIndexStart) % EGA_BUFFER_NUM_COLUMNS;
	bufferCoord.y = (bufferIndex - bufferStripIndexStart) / EGA_BUFFER_NUM_COLUMNS;
	return  bufferCoord;
}


StripCoord bufferMapBufferCoordToWorldCoord (Buffer buffer, StripCoord bufferCoord) {
	StripCoord worldCoord;
	worldCoord.column = buffer->scroll.column + bufferCoord.column;
	worldCoord.y = buffer->scroll.y + bufferCoord.y;
	return  worldCoord;
}


unsigned long int bufferGetDirtyBackgroundStripsNumIndices (Buffer buffer) {
	return getDirtyBackgroundStripsNumIndices(buffer->dirtyBackgroundStrips);
}


unsigned short int *bufferGetDirtyBackgroundStripsIndices (Buffer buffer) {
	return getDirtyBackgroundStripsIndices(buffer->dirtyBackgroundStrips);
}


void bufferClearDirtyBackgroundStrips (Buffer buffer) {
	clearDirtyBackgroundStrips(buffer->dirtyBackgroundStrips);
}


void bufferMarkDirtyBackgroundStrips(Buffer buffer, unsigned short int bufferIndex) {
	markDirtyBackgroundStrips(buffer->dirtyBackgroundStrips, bufferIndex);
}
