#ifndef coords_grouped_by_strip_h
#define coords_grouped_by_strip_h

#include "grouped_table.h"
#include "uint16_vector.h"


declareGroupedTable(IndicesByStripTable, uint16_t, uint16_t, Uint16Vector, indicesByStripTable)
declareVector(IndicesByStripTableRowsVector, IndicesByStripTableRow, indicesByStripTableRowsVector)


#endif
