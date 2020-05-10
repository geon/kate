#include "map.h"
#include "sprite_struct.h"

BitplaneStrip getStripAtWorldCoord(Sprite backgroundImage, unsigned short int column, unsigned short int y) {
	unsigned short int sourceStripColumn;
	unsigned short int sourceStripY;
	unsigned short int sourceStripIndex;

	sourceStripColumn = column % backgroundImage->numColumns;
	sourceStripY = y % backgroundImage->height;

	sourceStripIndex = sourceStripColumn + sourceStripY * backgroundImage->numColumns;

	return backgroundImage->bitPlaneStrips[sourceStripIndex];
}
