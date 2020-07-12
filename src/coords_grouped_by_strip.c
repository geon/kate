#include "coords_grouped_by_strip.h"

#include <stdlib.h>


defineVectorStruct(StripCoordVector, StripCoord, stripCoordVector)
defineVector(StripCoordVector, StripCoord, stripCoordVector)
defineVectorStruct(CoordsByStripTableRowsVector, CoordsByStripTableRow, coordsByStripTableRowsVector)
defineVector(CoordsByStripTableRowsVector, CoordsByStripTableRow, coordsByStripTableRowsVector)


typedef struct CoordsByStripTableStruct {
	CoordsByStripTableRowsVector rows;
} CoordsByStripTableStruct;


CoordsByStripTable makeCoordsByStripTable(const int capacity) {
	CoordsByStripTable table = malloc(sizeof(CoordsByStripTable));
	initializeCoordsByStripTable(table, capacity);
	return table;
}


void initializeCoordsByStripTable(CoordsByStripTable table, const int capacity) {
	table->rows = makeCoordsByStripTableRowsVector(capacity);
}


void destroyCoordsByStripTable(CoordsByStripTable table) {
	freeCoordsByStripTableRowsVector(table->rows);
}


void freeCoordsByStripTable(CoordsByStripTable table) {
	destroyCoordsByStripTable(table);
	free(table);
}


void coordsByStripTableAdd(CoordsByStripTable table, const BitplaneStrip key, const StripCoord value) {
	int rowIndex;

	// Scan for an existing key.
	for (rowIndex = 0; rowIndex < coordsByStripTableRowsVectorSize(table->rows); ++rowIndex) {
		if (bitplaneStripAsInt(key) == bitplaneStripAsInt(table->rows->values[rowIndex].key)) {
			break;
		}
	}

	// Check if the key exists.
	if (rowIndex < coordsByStripTableRowsVectorSize(table->rows)) {
		// The key exists.

		// Use the row of the existing key.
		stripCoordVectorPush(table->rows->values[rowIndex].values, value);

		// Bubble the row up, so commonly used rows are quicker to find.
		if (rowIndex > 0) {
			CoordsByStripTableRow tempRow = table->rows->values[rowIndex - 1];
			table->rows->values[rowIndex - 1] = table->rows->values[rowIndex];
			table->rows->values[rowIndex] = tempRow;
		}
	} else {
		// The key is missing.

		// Add a row for the missing key.
		CoordsByStripTableRow row;
		row.key = key;
		row.values = makeStripCoordVector(100);
		stripCoordVectorPush(row.values, value);
		coordsByStripTableRowsVectorPush(table->rows, row);
	}

}


CoordsByStripTableRow* coordsByStripTableBegin(CoordsByStripTable table) {
	return coordsByStripTableRowsVectorBegin(table->rows);
}


CoordsByStripTableRow* coordsByStripTableEnd(CoordsByStripTable table) {
	return coordsByStripTableRowsVectorEnd(table->rows);
}
