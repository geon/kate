#include "coord_conversion.h"

PixelCoord makePixelCoordFromStripCoord (StripCoord stripCoord) {

	PixelCoord pixelCoord;
	pixelCoord.x = stripCoord.column*8;
	pixelCoord.y = stripCoord.y;

	return pixelCoord;
}

StripCoord makeStripCoordFromPixelCoord (PixelCoord pixelCoord) {

	StripCoord stripCoord;
	stripCoord.column =pixelCoord.x/8;
	stripCoord.y = pixelCoord.y;

	return stripCoord;
}

StripCoord makeStripCoordFromEgaScrollCoord (EgaScrollCoord egascrollCoord) {

	StripCoord stripCoord;
	stripCoord.column =egascrollCoord.column;
	stripCoord.y = egascrollCoord.y;

	return stripCoord;
}

PixelCoord makePixelCoordFromEgaScrollCoord (EgaScrollCoord egascrollCoord) {

	PixelCoord pixelCoord;
	pixelCoord.x = egascrollCoord.column*8 + egascrollCoord.restX;
	pixelCoord.y = egascrollCoord.y;

	return pixelCoord;
}

EgaScrollCoord makeEgaScrollCoordFromPixelCoord (PixelCoord pixelCoord) {

	EgaScrollCoord egascrollCoord;
	egascrollCoord.column =pixelCoord.x/8;
	egascrollCoord.restX = pixelCoord.x%8;
	egascrollCoord.y = pixelCoord.y;

	return egascrollCoord;
}
