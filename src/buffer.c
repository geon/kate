#include "buffer.h"
#include "platform/dos/ega.h"
#include "strip_coord.h"

#include <stdlib.h>


Buffer makeBuffer () {
	Buffer buffer;

	buffer.alternateBuffer = false;
	buffer.scroll.column = 0;
	buffer.scroll.y = 0;

	return buffer;
}


void updateBuffer (Buffer *buffer) {
	// buffer->alternateBuffer = !buffer->alternateBuffer;
}


void setBufferScroll (Buffer *buffer, unsigned short int column, unsigned short int y) {
	buffer->scroll.column = column;
	buffer->scroll.y = y;
}


unsigned short int stripWorldCoordToBufferIndex (unsigned short int column, unsigned short int y, bool alternateBuffer) {
	return y*EGA_BUFFER_NUM_COLUMNS + column + (alternateBuffer ? EGA_BUFFER_SIZE : 0);
}


StripCoord mapBufferIndexToBufferCoord (unsigned short int bufferIndex, StripCoord worldScroll, bool alternateBuffer) {
	unsigned short int bufferStripIndexStart = stripWorldCoordToBufferIndex(worldScroll.column, worldScroll.y, alternateBuffer);
	StripCoord bufferCoord;
	bufferCoord.column = (bufferIndex - bufferStripIndexStart) % EGA_BUFFER_NUM_COLUMNS;
	bufferCoord.y = (bufferIndex - bufferStripIndexStart) / EGA_BUFFER_NUM_COLUMNS;
	return  bufferCoord;
}


StripCoord bufferMapBufferCoordToWorldCoord (Buffer *buffer, StripCoord worldScroll) {
	StripCoord worldCoord;
	worldCoord.column = buffer->scroll.column + worldScroll.column;
	worldCoord.y = buffer->scroll.y + worldScroll.y;
	return  worldCoord;
}
