#include "indices_grouped_by_strip.h"

#include <stdlib.h>


defineVectorStruct(StripCoordVector, StripCoord, stripCoordVector)
defineVector(StripCoordVector, StripCoord, stripCoordVector)
defineVectorStruct(IndicesByStripTableRowsVector, IndicesByStripTableRow, indicesByStripTableRowsVector)
defineVector(IndicesByStripTableRowsVector, IndicesByStripTableRow, indicesByStripTableRowsVector)
#define bitplaneStripsEqual(a, b) bitplaneStripAsInt(a) == bitplaneStripAsInt(b)
defineGroupedTable(IndicesByStripTable, BitplaneStrip, StripCoord, StripCoordVector, stripCoordVector, indicesByStripTable, bitplaneStripsEqual)
