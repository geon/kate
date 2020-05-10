#include "map.h"
#include "sprite_struct.h"
#include <stdlib.h>
#include "bmp.h"


typedef struct MapStruct {
	Sprite sprite;
} MapStruct;


Sprite mapLoadSprite (Map map, char *imagePath, char **errorMessage) {
	Image image;
	Sprite sprite;

	image = loadBmp(imagePath, true, errorMessage);
	if (!image) {
		return false;
	}

	sprite = makeSprite(image);
	freeImage(image);

	return sprite;
}


Map makeMap (char **errorMessage) {
    Map map = malloc(sizeof(MapStruct));

    map->sprite = mapLoadSprite(map, "../images/backgr.bmp", errorMessage);
	if (!map->sprite) {
		return NULL;
	}

	return map;
}


void freeMap (Map map) {
    freeSprite(map->sprite);
}


BitplaneStrip getStripAtWorldCoord(Map map, unsigned short int column, unsigned short int y) {
    Sprite backgroundImage = map->sprite;
	unsigned short int sourceStripColumn;
	unsigned short int sourceStripY;
	unsigned short int sourceStripIndex;

	sourceStripColumn = column % backgroundImage->numColumns;
	sourceStripY = y % backgroundImage->height;

	sourceStripIndex = sourceStripColumn + sourceStripY * backgroundImage->numColumns;

	return backgroundImage->bitPlaneStrips[sourceStripIndex];
}
