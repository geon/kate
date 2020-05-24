#ifndef buffer_h
#define buffer_h

#include "platform/dos/ega.h"
#include "strip_coord.h"

#include <stdlib.h>


#define EGA_BUFFER_NUM_COLUMNS 88
#define EGA_BUFFER_NUM_COLUMNS_DEFAULT 80
// Bytes per plane, or number of pixels/8.
#define EGA_PLANE_SIZE 65536
// Half of the 64 kB plane for each of the 2 buffers.
#define EGA_BUFFER_SIZE 32768


typedef struct Buffer {
	StripCoord scroll;
	bool alternateBuffer;
} Buffer;


Buffer makeBuffer ();
void updateBuffer (Buffer *buffer);
void setBufferScroll (Buffer *buffer, unsigned short int column, unsigned short int y);
unsigned short int stripWorldCoordToBufferIndex (unsigned short int column, unsigned short int y, bool alternateBuffer);
StripCoord mapBufferIndexToBufferCoord (unsigned short int bufferIndex, StripCoord worldScroll, bool alternateBuffer);
StripCoord mapBufferCoordToWorldCoord (Buffer *buffer, StripCoord worldScroll);

#endif
