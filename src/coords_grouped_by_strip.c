#include "coords_grouped_by_strip.h"

#include <stdlib.h>


defineVectorStruct(StripCoordVector, StripCoord, stripCoordVector)
defineVector(StripCoordVector, StripCoord, stripCoordVector)
defineVectorStruct(CoordsByStripTableRowsVector, CoordsByStripTableRow, coordsByStripTableRowsVector)
defineVector(CoordsByStripTableRowsVector, CoordsByStripTableRow, coordsByStripTableRowsVector)
#define bitplaneStripsEqual(a, b) bitplaneStripAsInt(a) == bitplaneStripAsInt(b)
defineGroupedTable(CoordsByStripTable, BitplaneStrip, StripCoord, StripCoordVector, stripCoordsVector, coordsByStripTable, bitplaneStripsEqual)
