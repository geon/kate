#include "indices_grouped_by_strip.h"

#include <stdlib.h>


defineVectorStruct(IndicesByStripTableRowsVector, IndicesByStripTableRow, indicesByStripTableRowsVector)
defineVector(IndicesByStripTableRowsVector, IndicesByStripTableRow, indicesByStripTableRowsVector)
#define bitplaneStripIndicesEqual(a, b) a == b
defineGroupedTable(IndicesByStripTable, uint16_t, StripCoord, StripCoordVector, stripCoordVector, indicesByStripTable, bitplaneStripIndicesEqual)
