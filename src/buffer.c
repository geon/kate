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


unsigned short int bufferIndexStart (unsigned short int column, unsigned short int y, bool alternateBuffer) {
	return y*EGA_BUFFER_NUM_COLUMNS + column + (alternateBuffer ? EGA_BUFFER_SIZE : 0);
}


StripCoord bufferMapWorldCoordToBufferCoord (Buffer *buffer, StripCoord worldCoord) {
	StripCoord bufferCoord;
	bufferCoord.column = worldCoord.column - buffer->scroll.column;
	bufferCoord.y = worldCoord.y - buffer->scroll.y;
	return  bufferCoord;
}


unsigned short int bufferMapBufferCoordToBufferIndex (Buffer *buffer, StripCoord bufferCoord) {
	return
		bufferIndexStart(buffer->scroll.column, buffer->scroll.y, buffer->alternateBuffer) +
		bufferCoord.y*EGA_BUFFER_NUM_COLUMNS + bufferCoord.column +
		(buffer->alternateBuffer ? EGA_BUFFER_SIZE : 0);;
}


StripCoord bufferMapBufferIndexToBufferCoord (Buffer *buffer, unsigned short int bufferIndex) {
	unsigned short int bufferStripIndexStart = bufferIndexStart(buffer->scroll.column, buffer->scroll.y, buffer->alternateBuffer);
	StripCoord bufferCoord;
	bufferCoord.column = (bufferIndex - bufferStripIndexStart) % EGA_BUFFER_NUM_COLUMNS;
	bufferCoord.y = (bufferIndex - bufferStripIndexStart) / EGA_BUFFER_NUM_COLUMNS;
	return  bufferCoord;
}


StripCoord bufferMapBufferCoordToWorldCoord (Buffer *buffer, StripCoord bufferCoord) {
	StripCoord worldCoord;
	worldCoord.column = buffer->scroll.column + bufferCoord.column;
	worldCoord.y = buffer->scroll.y + bufferCoord.y;
	return  worldCoord;
}
