#ifndef coords_grouped_by_strip_h
#define coords_grouped_by_strip_h

#include "vector.h"
#include "platform/dos/bitplane_strip.h"
#include "strip_coord.h"


declareVector(StripCoordVector, StripCoord, stripCoordVector)


typedef struct CoordsByStripTableRow {
	BitplaneStrip key;
	StripCoordVector values;
} CoordsByStripTableRow;


declareVector(CoordsByStripTableRowsVector, CoordsByStripTableRow, coordsByStripTableRowsVector)


typedef struct CoordsByStripTableStruct *CoordsByStripTable;


CoordsByStripTable makeCoordsByStripTable(const int capacity);
void initializeCoordsByStripTable(CoordsByStripTable table, const int capacity);
void destroyCoordsByStripTable(CoordsByStripTable table);
void freeCoordsByStripTable(CoordsByStripTable table);
void coordsByStripTableAdd(CoordsByStripTable table, const BitplaneStrip key, const StripCoord value);
CoordsByStripTableRow* coordsByStripTableBegin(CoordsByStripTable table);
CoordsByStripTableRow* coordsByStripTableEnd(CoordsByStripTable table);


#endif
