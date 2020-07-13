#ifndef coords_grouped_by_strip_h
#define coords_grouped_by_strip_h

#include "grouped_table.h"
#include "vector.h"
#include "platform/dos/bitplane_strip.h"
#include "strip_coord.h"


declareVector(StripCoordVector, StripCoord, stripCoordVector)
declareGroupedTable(CoordsByStripTable, BitplaneStrip, StripCoord, StripCoordVector, coordsByStripTable)
declareVector(CoordsByStripTableRowsVector, CoordsByStripTableRow, coordsByStripTableRowsVector)


#endif
