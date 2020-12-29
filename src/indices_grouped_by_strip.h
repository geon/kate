#ifndef coords_grouped_by_strip_h
#define coords_grouped_by_strip_h

#include "grouped_table.h"
#include "platform/dos/bitplane_strip.h"
#include "strip_coord.h"

#include <stdint.h>


declareGroupedTable(IndicesByStripTable, uint16_t, StripCoord, StripCoordVector, indicesByStripTable)
declareVector(IndicesByStripTableRowsVector, IndicesByStripTableRow, indicesByStripTableRowsVector)


#endif
