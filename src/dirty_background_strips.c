#include "dirty_background_strips.h"

#include "world.h"
#include "buffer.h"
#include "uint16_vector_struct_vector.h"
#include "uint16_vector.h"

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>


typedef struct DirtyBackgroundStripsStruct {
	Uint16VectorStructVectorStruct dirtyBufferIndicesByStripIndex; // Array, indexed by a strip-index, containing arrays of buffer indices where the buffer strips of the respective index are dirty.
	Uint16VectorStruct stripIndicesWithDirtyBufferIndices; // Indices on which dirtyBufferIndicesByStripIndex has dirty buffer indices.
} DirtyBackgroundStripsStruct;


DirtyBackgroundStrips makeDirtyBackgroundStrips (bool alternateBuffer) {
	// TODO: Add an api for using the actual number.
	const int32_t numStrips = 0xFFFF;
	int32_t stripIndex;
	DirtyBackgroundStrips dirtyBackgroundStrips;
	dirtyBackgroundStrips = malloc(sizeof(DirtyBackgroundStripsStruct));
	assert(dirtyBackgroundStrips);

	initializeUint16Vector(&dirtyBackgroundStrips->stripIndicesWithDirtyBufferIndices, 64);
	initializeUint16VectorStructVector(&dirtyBackgroundStrips->dirtyBufferIndicesByStripIndex, numStrips);
	// Initialize dirtyBufferIndicesByStripIndex. It needs to be full of empty arrays.
	for (stripIndex=0; stripIndex<numStrips; ++stripIndex) {
		Uint16VectorStruct vector;
		initializeUint16Vector(&vector, 8);
		uint16VectorStructVectorPush(&dirtyBackgroundStrips->dirtyBufferIndicesByStripIndex, vector);
	}

	return dirtyBackgroundStrips;
}


void freeDirtyBackgroundStrips (DirtyBackgroundStrips dirtyBackgroundStrips) {
	free(dirtyBackgroundStrips);
}


Uint16Vector dirtyBackgroundStripsGetDirtyBufferIndicesByStripIndex (DirtyBackgroundStrips dirtyBackgroundStrips) {
	return dirtyBackgroundStrips->dirtyBufferIndicesByStripIndex.values;
}


uint16_t * dirtyBackgroundStripsBegin (DirtyBackgroundStrips dirtyBackgroundStrips) {
	return uint16VectorBegin(&dirtyBackgroundStrips->stripIndicesWithDirtyBufferIndices);
}


uint16_t * dirtyBackgroundStripsEnd (DirtyBackgroundStrips dirtyBackgroundStrips) {
	return uint16VectorEnd(&dirtyBackgroundStrips->stripIndicesWithDirtyBufferIndices);
}


void dirtyBackgroundStripsClear (DirtyBackgroundStrips dirtyBackgroundStrips) {
	// Go through all strip indices with dirt.
	uint16_t *indexWithDirt;
	vectorForeach(
		uint16VectorBegin(&dirtyBackgroundStrips->stripIndicesWithDirtyBufferIndices),
		uint16VectorEnd(&dirtyBackgroundStrips->stripIndicesWithDirtyBufferIndices),
		indexWithDirt
	) {
		// Empty the list of dirty indices.
		Uint16Vector dirtyBufferIndices = &dirtyBackgroundStrips->dirtyBufferIndicesByStripIndex.values[*indexWithDirt];
		uint16VectorClear(dirtyBufferIndices);
	}

	// The dirt has been cleared, so clear the list of indexes where it was found.
	uint16VectorClear(&dirtyBackgroundStrips->stripIndicesWithDirtyBufferIndices);
}


void dirtyBackgroundStripsMark (DirtyBackgroundStrips dirtyBackgroundStrips, uint16_t bufferIndex, uint16_t cleanStripIndex) {
	Uint16Vector dirtyBufferIndices = &dirtyBackgroundStrips->dirtyBufferIndicesByStripIndex.values[cleanStripIndex];

	// If there was no dirt on this index previously, add it to the list of indices with dirt.
	if(!uint16VectorSize(dirtyBufferIndices)) {
		uint16VectorPush(&dirtyBackgroundStrips->stripIndicesWithDirtyBufferIndices, cleanStripIndex);
	}

	// Mark the buffer index as dirty.
	uint16VectorPush(dirtyBufferIndices, bufferIndex);
}



// TODO: This can't be here.
typedef struct BufferStruct {
	EgaScrollCoord scroll;
} BufferStruct;

void dirtyBackgroundStripsMarkRectangle (DirtyBackgroundStrips dirtyBackgroundStrips, StripCoord topLeftWorldCoord, StripCoord bottomRightWorldCoord, Map map, Buffer buffer, bool alternateBuffer) {
	StripCoord worldCoord;
	uint16_t y, column;
	uint16_t bufferIndex, rowBegin;

	rowBegin = bufferMapWorldCoordToBufferIndex(
		topLeftWorldCoord,
		buffer->scroll,
		alternateBuffer
	);

	for (y=topLeftWorldCoord.y; y<bottomRightWorldCoord.y; ++y) {
		bufferIndex = rowBegin;
		worldCoord.y = y;
		for (column=topLeftWorldCoord.column; column<bottomRightWorldCoord.column; ++column) {
			worldCoord.column = column;

			dirtyBackgroundStripsMark(
				dirtyBackgroundStrips,
				bufferIndex,
				getMapStripAtWorldCoord(map, worldCoord)
			);

			++bufferIndex;
		}

		rowBegin += EGA_BUFFER_NUM_COLUMNS;
	}
}
